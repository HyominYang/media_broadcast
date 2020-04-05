#include "broadcast.h"
#include <QDebug>
#include <mutex>
#include <deque>
#include <zmq/zmq.hpp>
#include "glog/logging.h"
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
Broadcast::Broadcast(QThread *parent) : QThread(parent)
{}
inline bool check_id(const char* request_id, const size_t &id_size)
{
    if (!request_id || id_size != IPPA_ID_SIZE) {
        return false;
    }
    const char *ippa_id = IPPA_ID;
    bool result = true;
    for (int i=IPPA_ID_SIZE-1; i>= 0; --i) {
        if (request_id[i] == '*') {
            continue;
        }
        if (ippa_id[i] == request_id[i]) {
            continue;
        }
        result = false;
        break;
    }
    return result;
}
void Broadcast::run()
{
    qDebug()<<"Hello broadcast thread";
    while (1) {
        zmq::context_t zmq_ctx;
        zmq::socket_t socket(zmq_ctx, zmq::socket_type::sub);
        socket.set(zmq::sockopt::subscribe, "");
        std::string server_ip = Environment::instance().ip();
        qDebug()<<"Try connect to broadcast-server ("<<server_ip.c_str()<<")";
        std::string server_addr = "tcp://" + server_ip + ":5556";
        socket.connect(server_addr);
        qDebug()<<"Broadcast-server is connected";

        while (1) {
            zmq::message_t message;
            socket.recv(message);
            qDebug()<<"broadcast-message received ("<<message.size()<<")";
            if (!protocol::CheckValidation(message)) {
                sleep(1);
                continue;
            }
            std::string id(protocol::GetID(message), protocol::Code::ID_SIZE);
            qDebug()<<"id: "<<id.c_str();
            if (!check_id(id.c_str(), id.size())) {
                qDebug()<<"B|"<<id.c_str()<<"|"<<protocol::CodeToString(protocol::GetCode(message)).c_str();
                continue;
            }
            uint32_t code = protocol::GetCode(message);
            const size_t data_size = protocol::GetDataSize(message);
            qDebug()<<"Broadcast: "<<protocol::CodeToString(code).c_str();
            if (code == protocol::Code::kBroadcastTextType1) {
                if (data_size) {
                    std::string print_message(protocol::GetData(message), data_size);
                    sig_request_message(print_message.c_str());
                }
            }
        }
    }
}
void Broadcast::slot_request_message(QString message)
{
    qDebug()<<">>"<<message;
    Data::instance().push_message(message);
}
}
