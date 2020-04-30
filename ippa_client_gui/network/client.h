#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QThread>
#include <zmq.hpp>

namespace network {
class Client : public QThread
{
    Q_OBJECT
public:
    explicit Client(QThread *parent = nullptr);

public slots:
    void slot_request_message(QString message);
private:
    void run() override;
    void RequestProcedure(zmq::socket_t &socket);
signals:

};
}

#endif // CLIENT_H
