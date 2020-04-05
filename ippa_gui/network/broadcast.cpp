#include "broadcast.h"
#include <QDebug>
#include <zmq/zmq.hpp>
#include "util/protocol/protocol.h"
#include "env.h"

namespace network {
Broadcast::Broadcast(QThread *parent) : QThread(parent)
{}

void Broadcast::run()
{
    qDebug()<<"Hello broadcast thread";
    while (1) {
        zmq::context_t zmq_ctx;
        zmq::socket_t socket(zmq_ctx, zmq::socket_type::sub);
        std::string server_ip = Environment::instance().ip();
        qDebug()<<"Try connect to broadcast-server ("<<server_ip.c_str()<<")";
        std::string server_addr = "tcp://" + server_ip + ":5556";
        socket.connect(server_addr);
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
