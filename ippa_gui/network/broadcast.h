#ifndef BROADCAST_H
#define BROADCAST_H

#include <QObject>
#include <QThread>

namespace network {
class Broadcast : public QThread
{
    Q_OBJECT
public:
    explicit Broadcast(QThread *parent = nullptr);
private:
    void run() override;
signals:

};
}
#endif // BROADCAST_H
