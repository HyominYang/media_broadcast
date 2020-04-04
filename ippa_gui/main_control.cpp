#include "main_control.h"
#include <QDebug>
#include <QQmlApplicationEngine>

struct MainControl::Data
{
    QObject *gui_object;
    QQmlApplicationEngine *engine;
};

MainControl::MainControl(QThread *parent) : QThread(parent)
{
  data_ = new MainControl::Data;
}
MainControl::~MainControl()
{
  delete data_;
}
void MainControl::set_engine(QQmlApplicationEngine *engine)
{
  data_->engine = engine;
  data_->gui_object = data_->engine->rootObjects().first();
  connect(data_->gui_object, SIGNAL(qmls_request(QString)), this, SLOT(gui_request(QString)));
}
void MainControl::gui_request(QString request_message)
{
  qDebug()<<request_message;
}
void MainControl::gui_message(QString msg)
{
    QMetaObject::invokeMethod(data_->gui_object, "qmlf_message", Q_ARG(QVariant, QVariant::fromValue(msg)));
}
void MainControl::run()
{
  while(1)
  {
    sleep(1);
    gui_message("...");
  }
}
