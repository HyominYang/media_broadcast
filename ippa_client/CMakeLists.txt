
set(MODULE_NAME ippa_client)

set(SRC main.cpp ../util/protocol/protocol.cpp)
add_executable(${MODULE_NAME} ${SRC})
target_link_libraries(${MODULE_NAME} zmq pthread glog gstreamer-1.0 gobject-2.0 glib-2.0 gio-2.0 pthread)
