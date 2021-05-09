cmake_minimum_required(VERSION 3.4.1)

project(MemorySocket)

include_directories(include)
set(MemorySocket_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT COMMAND testBuilder_build)
    add_subdirectory(testBuilder)
    testBuilder_set_current_working_directory_to_default_binary_directory()
endif()

add_subdirectory(populate_gtest)

# Now simply link against gtest or gtest_main as needed. Eg

testBuilder_add_source(MemorySocket src/MemorySocket.cpp)
testBuilder_add_source(MemorySocket src/Log.cpp)
testBuilder_add_library(MemorySocket atomic)
testBuilder_build_shared_library(MemorySocket)
add_subdirectory(tests)