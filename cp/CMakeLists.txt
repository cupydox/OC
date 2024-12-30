cmake_minimum_required(VERSION 3.10)
project(MemoryAllocators) 

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2  
  TLS_VERIFY false 
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)  

add_library(BuddyAllocator src/BuddyAllocator.cpp)  
add_library(BinaryBuddyAllocator src/BinaryBuddyAllocator.cpp)  

include_directories(include)

add_executable(MemoryAllocators main.cpp)
target_link_libraries(MemoryAllocators BuddyAllocator BinaryBuddyAllocator)

add_executable(tests test/test_cp.cpp)
target_link_libraries(tests BuddyAllocator BinaryBuddyAllocator gtest_main)

enable_testing()
add_test(NAME tests COMMAND tests)
