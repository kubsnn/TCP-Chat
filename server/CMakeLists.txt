cmake_minimum_required(VERSION 3.27)

project(tcp_server)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

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
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/src/core)
include_directories(${PROJECT_SOURCE_DIR}/src/core/database)
include_directories(${PROJECT_SOURCE_DIR}/src/core/utils)
include_directories(${PROJECT_SOURCE_DIR}/src/controllers)

# find OpenSSL
find_package(OpenSSL 3 REQUIRED)

# find SQLITE3
find_package(SQLite3 REQUIRED)

# add the executable
file(GLOB SOURCES "src/*.cpp" "src/core/*.cpp" "src/core/database/*.cpp" "src/core/utils/*.cpp" "src/controllers/*.cpp")
add_executable(app ${SOURCES})

# link libraries
target_link_libraries(app OpenSSL::SSL OpenSSL::Crypto pthread sqlite3)