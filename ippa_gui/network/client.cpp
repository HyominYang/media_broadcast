#include "client.h"
#include <QDebug>
#include <zmq/zmq.hpp>
#include "ControlMessage.h"

namespace network {
Client::Client(QThread *parent) : QThread(parent)
{}

void Client::run()
{
    qDebug()<<"Hello client thread!!";
    while(1) {
        zmq::context_t zmq_ctx;
        zmq::socket_t socket(zmq_ctx, zmq::socket_type::req);
        qDebug()<<"Try connect to server";
        socket.connect("tcp://192.168.0.21:5555");
        qDebug()<<"Control-server is connected";

//        zmq::message_t *msg = protocol::MakeRequest(protocol::Code::kIdentification, "Z0000000");
        zmq::message_t *msg = protocol::MakeRequest(protocol::Code::kBroadcastMicOpenSuccess, "Z0000000");
        if (!msg) {
            sleep(1);
            continue;
        }
        qDebug()<<"send message - "<<protocol::CodeToString(protocol::Code::kKeepAlive).c_str();
        socket.send(*msg);
        delete msg;
        zmq::message_t rep;
        socket.recv(rep);
        qDebug()<<"Reconnect to control-server";
        sleep(60);
    }
}
}
