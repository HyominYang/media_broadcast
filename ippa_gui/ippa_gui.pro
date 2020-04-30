QT += quick core

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS ZMQ_STATIC

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../util/protocol/protocol.cpp \
        env.cpp \
        main.cpp \
        main_control.cpp \
        network/broadcast.cpp \
        network/client.cpp

RESOURCES += qml.qrc

TRANSLATIONS += \
    ippa_gui_ko_KR.ts

INCLUDEPATH += \
        /usr/include/glib-2.0 \
        /usr/include/gstreamer-1.0 \
        /home/wind/workspace/private/media_broadcast \

QMAKE_LIBDIR += \


LIBS += -lzmq -lglog

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
  ../util/protocol/protocol.h \
  main_control.h \
  network/ControlMessage.h \
  network/broadcast.h \
  network/client.h
