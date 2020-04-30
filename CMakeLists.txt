cmake_minimum_required(VERSION 3.5.1)
project(media_broadcast)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DASIO_STANDALONE)

include_directories(${CMAKE_CURRENT_LIST_DIR})
#include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/include)
#link_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/libs/zmq)
#link_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/libs/glog)
#include_directories(/usr/include/glib-2.0)
#include_directories(/usr/include/gstreamer-1.0)
#link_libraries("./x86_64-linux-gnu/")

add_subdirectory(ippa_server)
add_subdirectory(ippa_client)
