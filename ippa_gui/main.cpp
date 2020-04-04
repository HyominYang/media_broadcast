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
    m.set_engine(&engine);
    m.start();

//GUI* GUI::get_instance()
//{
//    if(__gui == NULL)
//    {
//        __gui = new GUI();
//        Q_ASSERT(__gui != NULL);
//        __gui->__qml = __gui->rootContext();
//        Q_ASSERT(__gui->__qml != NULL);
//    }
//    return __gui;
//}
//    m_gui = GUI::get_instance();
//    connect(this, SIGNAL(sig_page()), this, SLOT(gui_page_changed()));
//    if(m_gui) regist_resource();
//    m_gui->setModel("gui", this);
//    m_gui->setModel("patient_model", &m_patient_model);
//    m_gui->setModel("wifi", &m_wifi);
//    m_gui->setModel("hp1", &m_hp[0]);
//    m_gui->setModel("hp2", &m_hp[1]);
//    m_gui->setModel("hp3", &m_hp[2]);
//    m_gui->setModel("hp4", &m_hp[3]);
//    m_gui->setModel("cs_logviewModel", &m_cs_logview);
//    m_gui->setModel("cs_logview_recordModel", &m_cs_logview_record);
//    m_gui->setProperty("passwd", m_conf_settings.get("passwd"));
//    m_gui->setProperty("cs_temp0", QString("0"));
//    m_gui->setProperty("cs_temp1", QString("0"));
//    m_gui->setProperty("cs_temp2", QString("0"));
//    m_gui->setProperty("cs_temp3", QString("0"));
//    m_gui->setProperty("cs_temp4", QString("0"));

//    m_gui->load(QUrl(QStringLiteral("qrc:/Slimus.qml"))); //  ///home/app/qml/Slimus.qml
//    QObject *gui_obj = m_gui->get_root();
    return app.exec();
}
