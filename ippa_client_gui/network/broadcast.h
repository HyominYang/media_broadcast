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
public slots:
    void slot_request_message(QString message);
private:
    void run() override;
signals:
  void sig_request_message(QString request_message);

};
}
#endif // BROADCAST_H
