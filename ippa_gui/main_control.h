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
private:
  void gui_message(QString msg);
  void run() override;
  struct Data;
  struct Data *data_;
private slots:
  void gui_request(QString request_message);
signals:

};

#endif // MAINCONTROL_H
