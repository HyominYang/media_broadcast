
set(MODULE_NAME ippa_client)

set(SRC
        main.cpp
        ../util/env.cpp
        ../util/protocol/protocol.cpp)
add_executable(${MODULE_NAME} ${SRC})
target_link_libraries(${MODULE_NAME} zmq pthread glog pthread)
