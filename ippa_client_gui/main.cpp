#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <glog/logging.h>
#include "network/broadcast.h"
#include "network/client.h"
#include "main_control.h"
#include "util/env.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    if (!Environment::instance().Load()) {
        LOG(ERROR)<<"get settings error";
        return 0;
    }

    network::Client c;
    c.start();

    network::Broadcast b;
    b.start();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    MainControl m;
    QObject::connect(&m, SIGNAL(sig_request_message(QString)), &c, SLOT(slot_request_message(QString)));
    QObject::connect(&b, SIGNAL(sig_request_message(QString)), &m, SLOT(gui_message(QString)));
    m.set_engine(&engine);
    m.start();

    return app.exec();
}
