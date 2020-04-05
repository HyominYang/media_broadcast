#include "client.h"
#include <QDebug>
#include <mutex>
#include <deque>
#include "util/protocol/protocol.h"
#include "ippa_id.h"
#include "env.h"

namespace {
class Data {
public:
    static Data& instance() {
        static Data data;
        return data;
    }
    void set_force_quit(bool flag = true) {
        std::lock_guard<std::mutex> guard(lock_);
        flag_quit_ = flag;
    }
    bool force_quit() const {
        return flag_quit_;
    }
    size_t message_count() {
        std::lock_guard<std::mutex> guard(lock_);
        size_t count = message_queue_.size();
        return count;
    }
    QString get_message()
    {
        std::lock_guard<std::mutex> guard(lock_);
        if (message_queue_.size() == 0) {
            return QString();
        }
        QString message = message_queue_.front();
        message_queue_.pop_front();
        return message;
    }
    void push_message(const QString &message) {
        std::lock_guard<std::mutex> guard(lock_);
        message_queue_.push_back(message);
    }
private:
    Data() : flag_quit_(false)
    {}
    std::mutex lock_;
    std::deque<QString> message_queue_;
    volatile bool flag_quit_;
};
}
namespace network {
Client::Client(QThread *parent) : QThread(parent)
{}

void Client::run()
{
    qDebug()<<"Hello client thread!!";
    while(!Data::instance().force_quit()) {
        // connection routine
        zmq::context_t zmq_ctx;
        zmq::socket_t socket(zmq_ctx, zmq::socket_type::req);
        std::string server_ip = Environment::instance().ip();
        qDebug()<<"Try connect to server ("<<server_ip.c_str()<<")";
        std::string server_addr = "tcp://" + server_ip + ":5555";
//        socket.connect("tcp://192.168.0.21:5555");
        socket.connect(server_addr);
        qDebug()<<"Control-server is connected";

        zmq::message_t *msg = protocol::MakeRequest(protocol::Code::kIdentification, IPPA_ID);
        if (!msg) {
            sleep(1);
            continue;
        }
        qDebug()<<"send message - "<<protocol::CodeToString(protocol::Code::kKeepAlive).c_str();
        socket.send(*msg);
        delete msg;
        zmq::message_t rep;
        socket.recv(rep);
        RequestProcedure(socket);
        socket.close();
        zmq_ctx.close();
        qDebug()<<"Reconnect to control-server";
        sleep(10);
    }
}
uint32_t StringToCode(QString &code_str) {
    if (code_str == "TEXT") {
        return protocol::Code::kBroadcastTextType1;
    }
    return protocol::Code::kProtocolError;
}
void Client::RequestProcedure(zmq::socket_t &socket)
{
    while (!Data::instance().force_quit()) {
        if (!Data::instance().message_count()) {
            sleep(1);
            continue;
        }
        QString message = Data::instance().get_message();
        if (!message.size()) {
            sleep(1);
            continue;
        }
        QStringList token_list = message.split("|", QString::SplitBehavior::SkipEmptyParts);
        if (token_list.size() < 2) {
            qDebug()<<"invalid token count ("<<token_list.size()<<")";
            continue;
        }
        uint32_t code = StringToCode(token_list[1]);
        if (code == protocol::Code::kProtocolError) {
            qDebug()<<"invalid code-string ("<<token_list[1]<<")";
            continue;
        }
        zmq::message_t *request_message = NULL;
        if (token_list.size() > 2) {
            request_message = protocol::MakeRequest(
                        code,
                        token_list[0].toLocal8Bit().data(),
                        token_list[2].toLocal8Bit().data(), token_list[2].length());
        }
        else {
            request_message = protocol::MakeRequest(
                        code,
                        token_list[0].toLocal8Bit().data());
        }
        if (request_message) {
            socket.send(*request_message);
            delete request_message;
            zmq::message_t rep;
            socket.recv(rep);
            if (rep.size() < protocol::Code::REP_MINIMUM_SIZE) {
                qDebug()<<"invalid reply";
                continue;
            }
            qDebug()<<"reply code: "<<*(uint32_t*)rep.data();
        }
    }
}
void Client::slot_request_message(QString message)
{
    qDebug()<<">>"<<message;
    Data::instance().push_message(message);
}
}
