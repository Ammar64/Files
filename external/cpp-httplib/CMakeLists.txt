cmake_minimum_required(VERSION 3.5)
project(httplib LANGUAGES CXX VERSION 0.14.2)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE httplib.cc)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if(WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()