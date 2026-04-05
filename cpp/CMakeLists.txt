cmake_minimum_required(VERSION 3.14)
project(DTS_ALG)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(main.out main.cpp src/java/Object.cpp)

target_include_directories(main.out PRIVATE include)

enable_testing()

add_executable(
  test.out
  test/iterable/collection/list/arrayList/ArrayListTest.cpp
  test/iterable/collection/list/arrayList/ArrayListIteratorTest.cpp
  test/util/CollectionsTest.cpp
)
target_link_libraries(
  test.out
  GTest::gtest_main
)

target_include_directories(test.out PRIVATE include)

include(GoogleTest)
gtest_discover_tests(test.out)