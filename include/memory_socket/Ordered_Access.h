//
// Created by totalcontrol on 9/5/21.
//

#ifndef EXECUTIONMANAGER_ORDERED_ACCESS_H
#define EXECUTIONMANAGER_ORDERED_ACCESS_H

#include <optional>

struct Ordered_Access_Set_Name {
    const char * name = "no name given";
    Ordered_Access_Set_Name() {}
    Ordered_Access_Set_Name(const char * name) {
        this->name = name;
    }
};

template <typename T>
struct Ordered_Access {
    bool debug_output = true;
    std::atomic<std::optional<T>> value = { std::nullopt };
    std::condition_variable condition_queue_is_empty;
    std::mutex queue_is_empty_mutex;

    // implement ordering
    const int access_order_initialization_value = -1;
    std::atomic<int> order {access_order_initialization_value};
    std::condition_variable order_obtained;
    std::mutex order_obtained_mutex;

    const char * name = "no name given";

    Ordered_Access() {} // default initialization

    Ordered_Access(const Ordered_Access_Set_Name & name) {
        this->name = name.name;
    }

    Ordered_Access(const T & val) {
        order.store(access_order_initialization_value - 1);
        store(val,  access_order_initialization_value - 0, "constructor");
    };

    Ordered_Access(const Ordered_Access_Set_Name & name, const T & val) {
        this->name = name.name;
        order.store(access_order_initialization_value - 1);
        store(val,  access_order_initialization_value - 0, "constructor");
    };

    Ordered_Access(const Ordered_Access & ordered_access) = delete;

    Ordered_Access(Ordered_Access && ordered_access) = delete;

    Ordered_Access & operator=(const Ordered_Access & ordered_access) = delete;

    Ordered_Access & operator=(Ordered_Access && ordered_access) = delete;

    void set_order(const int & access_order, const char * tag) {
        std::unique_lock<std::mutex> lA (order_obtained_mutex);
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] setting order to %d", name, tag, access_order);
        order.store(access_order);
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] set order to %d", name, tag, access_order);
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] notifying all order_obtained", name, tag);
        order_obtained.notify_all();
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] notified all order_obtained", name, tag);
    };

    void reset_order(const char * tag) {
        set_order(access_order_initialization_value, tag);
    };

    void wait_for_order(const int & access_order, const char * tag) {
        std::unique_lock<std::mutex> lA (order_obtained_mutex);
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] waiting for order to increment to %d", name, tag, access_order);
        order_obtained.wait(lA, [&] {
            if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] entered order_obtained.wait()", name, tag);
            if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] checking if order has incremented to %d", name, tag, access_order);
            bool val = (order.load() + 1) == access_order;
            if(debug_output && val) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] order has incremented to %d", name, tag, access_order);
            if(debug_output && !val) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] order has not incremented to %d", name, tag, access_order);
            if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] returning from order_obtained.wait()", name, tag);
            return val;
        });
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] waited for order to increment to %d", name, tag, access_order);
    }

    void store(const T & val, const int & access_order, const char * tag) {
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] storing", name, tag);
        wait_for_order(access_order, tag);
        value.store(val);
        condition_queue_is_empty.notify_one();
        order.fetch_add(1);
        order_obtained.notify_all();
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] stored", name, tag);
    }

    T peek(const int & access_order, const char * tag) {
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] peeking", name, tag);
        wait_for_order(access_order, tag);

        std::unique_lock<std::mutex> lB (queue_is_empty_mutex);
        condition_queue_is_empty.wait(lB, [&] { return value.load().has_value(); });
        T val = value.load().value();
        order.fetch_add(1);
        order_obtained.notify_all();
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] peeked", name, tag);
        return val;
    }

    T load(const int & access_order, const char * tag) {
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] loading", name, tag);
        wait_for_order(access_order, tag);

        std::unique_lock<std::mutex> lB (queue_is_empty_mutex);
        condition_queue_is_empty.wait(lB, [&] { return value.load().has_value(); });
        T val = value.load().value();
        value.store(std::nullopt);
        order.fetch_add(1);
        order_obtained.notify_all();
        if(debug_output) LOG_INFO("[ORDERED ACCESS: %s, TAG: %s] loaded", name, tag);
        return val;
    }

    void store_and_reset_order(const int & access_order, const char * tag) {
        store(access_order, tag);
        reset_order(tag);
    }

    T peek_and_reset_order(const int & access_order, const char * tag) {
        T val = peek(access_order, tag);
        reset_order(tag);
        return val;
    }

    T load_and_reset_order(const int & access_order, const char * tag) {
        T val = load(access_order, tag);
        reset_order(tag);
        return val;
    }
};

#define Ordered_Access_Named_Str(T, name, name_str) Ordered_Access<T> name = Ordered_Access_Set_Name(name_str)
#define Ordered_Access_Named(T, name) Ordered_Access_Named_Str(T, name, #name)

#endif //EXECUTIONMANAGER_ORDERED_ACCESS_H
