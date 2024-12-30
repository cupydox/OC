cmake_minimum_required(VERSION 3.10)
project(gtest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Добавление опций компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=maybe-uninitialized")

# Установка Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
  TLS_VERIFY false
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(math1 SHARED src/math1.cpp)
add_library(math2 SHARED src/math2.cpp)

# Установка путей для заголовочных файлов
include_directories(include)

add_executable(main1 src/main1.cpp)
target_link_libraries(main1 math1)

add_executable(main2 src/main2.cpp)
target_link_libraries(main2 ${CMAKE_DL_LIBS})

# Тесты
add_executable(tests test/test_4.cpp)
target_link_libraries(tests math1 math2 gtest_main)
