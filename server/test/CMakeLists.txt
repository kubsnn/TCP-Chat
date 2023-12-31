cmake_minimum_required(VERSION 3.10)

# set the project name
project(app_test)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# enable testing
enable_testing()

# find the GTest package
find_package(GTest REQUIRED)

# set the output directories
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# include the GTest headers
include_directories(${GTEST_INCLUDE_DIRS})

include(FetchContent)

FetchContent_Declare(
  consolecolors
  GIT_REPOSITORY https://github.com/kubsnn/ConsoleColors.git
)

FetchContent_MakeAvailable(consolecolors)

# Get the source directory of the fetched content
FetchContent_GetProperties(consolecolors)
if(NOT consolecolors_POPULATED)
  FetchContent_Populate(consolecolors)
endif()

# Add the include directory from the fetched content
include_directories(${consolecolors_SOURCE_DIR}/include)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/kubsnn/json.git
)

FetchContent_MakeAvailable(json)

# Get the source directory of the fetched content
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
endif()

# Add the include directory from the fetched content
include_directories(${json_SOURCE_DIR}/include)

# include directories
include_directories(${PROJECT_SOURCE_DIR}/../src)
include_directories(${PROJECT_SOURCE_DIR}/../src/include)
include_directories(${PROJECT_SOURCE_DIR}/../src/core)
include_directories(${PROJECT_SOURCE_DIR}/../src/core/database)
include_directories(${PROJECT_SOURCE_DIR}/../src/core/utils)
include_directories(${PROJECT_SOURCE_DIR}/../src/controllers)

# find OpenSSL
find_package(OpenSSL 3 REQUIRED)

# find SQLite3
find_package(SQLite3 REQUIRED)

# compile all the source files into a static library
file(GLOB_RECURSE MY_SOURCES "../src/core/*.cpp" "../src/core/database/*.cpp" "../src/core/utils/*.cpp" "../src/controllers/*.cpp")
add_library(my_lib STATIC ${MY_SOURCES})
target_link_libraries(my_lib sqlite3 OpenSSL::SSL OpenSSL::Crypto pthread)

set(MY_LIBS ${GTEST_BOTH_LIBRARIES} pthread sqlite3 OpenSSL::SSL OpenSSL::Crypto my_lib)

# add the executable
add_executable(account_controller_tests.test account_controller_tests.cpp)
target_link_libraries(account_controller_tests.test ${MY_LIBS})

add_executable(friend_list_tests.test friend_list_tests.cpp)
target_link_libraries(friend_list_tests.test ${MY_LIBS})

add_executable(rsa_tests.test rsa_tests.cpp)
target_link_libraries(rsa_tests.test ${MY_LIBS})

# add the test
add_test(NAME AccountControllerTests COMMAND account_controller_tests.test)
add_test(NAME FriendListTests COMMAND friend_list_tests.test)
add_test(NAME RSATests COMMAND rsa_tests.test)
