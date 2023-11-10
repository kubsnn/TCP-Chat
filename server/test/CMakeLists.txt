cmake_minimum_required(VERSION 3.10)

# set the project name
project(app_test)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# enable testing
enable_testing()

# find the GTest package
find_package(GTest REQUIRED)

# include the GTest headers
include_directories(${GTEST_INCLUDE_DIRS})

# add the executable
add_executable(app_test register_test.cpp)

# link the GTest library
target_link_libraries(app_test ${GTEST_BOTH_LIBRARIES})

# add the test
add_test(NAME MyTest COMMAND app_test)