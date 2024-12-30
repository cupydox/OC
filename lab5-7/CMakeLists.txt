include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        TLS_VERIFY false
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

cmake_minimum_required(VERSION 3.10)

set(SubProjectName "lab_5")

FetchContent_Declare(
        zeromq
        GIT_REPOSITORY https://github.com/zeromq/libzmq.git
        GIT_TAG        v4.3.5  
)

FetchContent_MakeAvailable(zeromq)

FetchContent_Declare(
        cppzmq
        GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
        GIT_TAG        v4.10.0  
)

FetchContent_MakeAvailable(cppzmq)

add_library(src
    src/client.cpp
    src/worker.cpp
)

target_link_libraries(src PRIVATE cppzmq)

add_executable(child
    src/worker.cpp
)

target_link_libraries(child PRIVATE src cppzmq)

add_executable(lab_5-7 src/client.cpp)
target_link_libraries(lab_5-7 PRIVATE src cppzmq)

add_dependencies(lab_5-7 child)

add_executable(lab_5-7_tests test/test_5-7.cpp)
target_link_libraries(lab_5-7_tests PRIVATE src gtest_main)

add_test(NAME lab_5-7_tests COMMAND LW5_test)

