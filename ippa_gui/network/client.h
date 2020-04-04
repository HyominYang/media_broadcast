#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QThread>

namespace network {
class Client : public QThread
{
    Q_OBJECT
public:
    explicit Client(QThread *parent = nullptr);
private:
    void run() override;
signals:

};
}

#endif // CLIENT_H
