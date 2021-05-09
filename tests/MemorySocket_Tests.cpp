//
// Created by smallville7123 on 9/08/20.
//

#include <gtest/gtest.h>
#include <memory_socket/MemorySocket>
#include <thread>
#include <memory_socket/timeout>


TEST(MemorySocket_Core, initNoCheck) {
    MemorySocket ms(1);
}

TEST(MemorySocket_Core, initWithCheck) {
    MemorySocket ms(1);
    EXPECT_EQ(ms.capacityInBytes, 1);
}

class memTest : public ::testing::Test {
protected:
    static constexpr int ITEM_NONE = 0;
    static constexpr int ITEM_ACTUAL = 5;
    static constexpr int CMD_NONE = 0;
    static constexpr int CMD_GET_ITEM = 1;

    int UI____data[2];
    int AUDIO_data[2];

    void SetUp() override {
        UI____data[0] = CMD_GET_ITEM;
        UI____data[1] = ITEM_NONE;

        AUDIO_data[0] = CMD_NONE;
        AUDIO_data[1] = ITEM_NONE;
    };
};

TEST_F(memTest, send_and_read_data_that_can_fit_1) {
    MemorySocket memsock(sizeof(int)*2);
    memsock.setGlobalFlags(MS_MSG_BLOCK|MS_MSG_DEBUG);

    std::thread UI_THREAD([&] {
        LOG_INFO("UI       : sending data to Audio thread: %d, %d", UI____data[0], UI____data[1]);
        memsock.send("UI   ", UI____data, sizeof(int)*2, 0);
        LOG_INFO("UI       : sent data to Audio thread");
    });
    std::thread AUDIO_THREAD([&] {
        LOG_INFO("AUDIO    : recieving data from UI thread");
        memsock.recv("AUDIO", AUDIO_data, sizeof(int)*2, 0);
        LOG_INFO("AUDIO    : recieved data from UI thread: %d, %d", AUDIO_data[0], AUDIO_data[1]);
    });

    bool result = std::timeout(5s, [&] {
        return
            UI____data[0] == 1 && UI____data[1] == 0
        ;
    });
    UI_THREAD.join();
    AUDIO_THREAD.join();
    ASSERT_EQ(UI____data[0], 1);
    ASSERT_EQ(UI____data[1], 0);
}

TEST_F(memTest, send_and_read_data_that_can_fit_2) {
    MemorySocket memsock(sizeof(int)*2);
    memsock.setGlobalFlags(MS_MSG_BLOCK|MS_MSG_DEBUG);

    AUDIO_data[0] = CMD_GET_ITEM;

    std::thread UI_THREAD([&] {
        LOG_INFO("UI       : recieving data from Audio thread");
        memsock.recv("UI   ", UI____data, sizeof(int)*2, 0);
        LOG_INFO("UI       : recieved data from Audio thread: %d, %d", UI____data[0], UI____data[1]);
    });
    std::thread AUDIO_THREAD([&] {
        if (AUDIO_data[0] == CMD_GET_ITEM) {
            LOG_INFO("AUDIO    : data contains valid command");
            LOG_INFO("AUDIO    : setting data");
            AUDIO_data[1] = ITEM_ACTUAL;
            LOG_INFO("AUDIO    : set data");
        } else {
            LOG_INFO("AUDIO    : data contains invalid command: %d, expected %d", AUDIO_data[0], CMD_GET_ITEM);
        }
        LOG_INFO("AUDIO    : sending data to UI thread: %d, %d", AUDIO_data[0], AUDIO_data[1]);
        memsock.send("AUDIO", AUDIO_data, sizeof(int)*2, 0);
        LOG_INFO("AUDIO    : sent data to UI thread");
    });

    bool result = std::timeout(5s, [&] {
        return
                AUDIO_data[0] == 1 && AUDIO_data[1] == 5
        ;
    });
    UI_THREAD.join();
    AUDIO_THREAD.join();
    ASSERT_EQ(AUDIO_data[0], 1);
    ASSERT_EQ(AUDIO_data[1], 5);
}

TEST_F(memTest, send_and_read_data_that_can_fit_3) {
    MemorySocket memsock(sizeof(int)*2);
    memsock.setGlobalFlags(MS_MSG_BLOCK|MS_MSG_DEBUG);

    std::thread UI_THREAD([&] {
        LOG_INFO("UI       : sending data to Audio thread: %d, %d", UI____data[0], UI____data[1]);
        LOG_INFO("memsock.send(\"UI    S1\", UI____data, sizeof(int)*2, 0); ENTER");
        memsock.send("UI    S1", UI____data, sizeof(int)*2, 0);
        LOG_INFO("memsock.send(\"UI    S1\", UI____data, sizeof(int)*2, 0); EXIT");
        LOG_INFO("UI       : sent data to Audio thread");
        LOG_INFO("UI       : recieving data from Audio thread");
        LOG_INFO("memsock.recv(\"UI    S2\", UI____data, sizeof(int)*2, 0); ENTER");
        memsock.recv("UI    S2", UI____data, sizeof(int)*2, 0);
        LOG_INFO("memsock.recv(\"UI    S2\", UI____data, sizeof(int)*2, 0); EXIT");
        LOG_INFO("UI       : recieved data from Audio thread: %d, %d", UI____data[0], UI____data[1]);
    });
    std::thread AUDIO_THREAD([&] {
        LOG_INFO("AUDIO    : recieving data from UI thread");
        LOG_INFO("memsock.recv(\"AUDIO S1\", AUDIO_data, sizeof(int)*2, 0); ENTER");
        memsock.recv("AUDIO S1", AUDIO_data, sizeof(int)*2, 0);
        LOG_INFO("memsock.recv(\"AUDIO S1\", AUDIO_data, sizeof(int)*2, 0); EXIT");
        LOG_INFO("AUDIO    : recieved data from UI thread: %d, %d", AUDIO_data[0], AUDIO_data[1]);
        if (AUDIO_data[0] == CMD_GET_ITEM) {
            LOG_INFO("AUDIO    : data contains valid command");
            LOG_INFO("AUDIO    : setting data");
            AUDIO_data[1] = ITEM_ACTUAL;
            LOG_INFO("AUDIO    : set data");
        } else {
            LOG_INFO("AUDIO    : data contains invalid command: %d, expected %d", AUDIO_data[0], CMD_GET_ITEM);
        }
        LOG_INFO("AUDIO    : sending data to UI thread: %d, %d", AUDIO_data[0], AUDIO_data[1]);
        LOG_INFO("memsock.send(\"AUDIO S2\", AUDIO_data, sizeof(int)*2, 0); ENTER");
        memsock.send("AUDIO S2", AUDIO_data, sizeof(int)*2, 0);
        LOG_INFO("memsock.send(\"AUDIO S2\", AUDIO_data, sizeof(int)*2, 0); EXIT");
        LOG_INFO("AUDIO    : sent data to UI thread");
    });

    bool result = std::timeout(5s, [&] {
        return
            UI____data[0] == 1 && UI____data[1] == 5 &&
            AUDIO_data[0] == 1 && AUDIO_data[1] == 5
        ;
    });
    UI_THREAD.join();
    AUDIO_THREAD.join();
    ASSERT_EQ(UI____data[0], 1);
    ASSERT_EQ(UI____data[1], 5);
    ASSERT_EQ(AUDIO_data[0], 1);
    ASSERT_EQ(AUDIO_data[1], 5);
}