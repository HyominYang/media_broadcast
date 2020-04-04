#include "broadcast.h"
#include <QDebug>
#include <zmq/zmq.hpp>
#include "ControlMessage.h"

namespace network {
Broadcast::Broadcast(QThread *parent) : QThread(parent)
{}

void Broadcast::run()
{
    qDebug()<<"Hello broadcast thread";
    while (1) {
        zmq::context_t zmq_ctx;
        zmq::socket_t socket(zmq_ctx, zmq::socket_type::sub);
        qDebug()<<"Try connect to broadcast-server";
        socket.connect("tcp://192.168.0.21:5556");
        qDebug()<<"Broadcast-server is connected";

        while (1) {
            zmq::message_t message;
            socket.recv(message);
            if (!protocol::CheckValidation(message)) {
                sleep(1);
                continue;
            }
            QString id(protocol::GetID(message));
            if (id != "ZZZZZZZZ") {
                continue;
            }
            qDebug()<<"Broadcast: "<<protocol::CodeToString(protocol::GetCode(message)).c_str();
        }
    }
}
}
