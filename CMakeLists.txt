cmake_minimum_required(VERSION 3.5)

project(files VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

add_executable(files)

file(GLOB_RECURSE SRC_FILES src/* include/*)
target_sources(files PRIVATE ${SRC_FILES})
target_include_directories(files PRIVATE include)


add_subdirectory(external/cpp-httplib)
find_library(httplib httplib)

target_link_libraries(files Qt5::Widgets Qt5::Network httplib)
if (MINGW)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-mwindows")
    message("Windows mingw")
endif()
