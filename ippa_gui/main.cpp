#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "network/client.h"
#include "main_control.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    network::Client c;
    c.start();

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
    m.set_engine(&engine);
    m.start();

    return app.exec();
}
