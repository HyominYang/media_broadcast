
set(MODULE_NAME ippa_server)

set(SRC main.cpp ../util/protocol/protocol.cpp)
add_executable(${MODULE_NAME} ${SRC})
target_link_libraries(${MODULE_NAME} zmq pthread glog)