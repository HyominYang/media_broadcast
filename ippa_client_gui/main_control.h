#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include <QThread>

class QQmlApplicationEngine;
class MainControl : public QThread
{
  Q_OBJECT
public:
  explicit MainControl(QThread *parent = nullptr);
  virtual ~MainControl();
  void set_engine(QQmlApplicationEngine *engine);
public slots:
  void gui_message(QString msg);
private:
  void run() override;
  struct Data;
  struct Data *data_;
private slots:
  void gui_request(QString message);

signals:
  void sig_request_message(QString request_message);

};

#endif // MAINCONTROL_H
