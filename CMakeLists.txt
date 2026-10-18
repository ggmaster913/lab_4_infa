cmake_minimum_required(VERSION 3.14)
project(Lab4_LazySequence CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# путь к заголовочным файлам
include_directories(include)

# GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# исполняемый файл для тестов
add_executable(tests
    tests/Tests.cpp
    tests/LazySequenceTests.cpp
    tests/StreamTests.cpp
    tests/StressTests.cpp
)
target_link_libraries(tests gtest_main)

# исполняемый файл для UI
add_executable(ui
    ui/ConsoleUI.cpp
)
