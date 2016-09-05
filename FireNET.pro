QT += core
QT -= gui
QT += network

CONFIG += c++11

TARGET = FireNET
CONFIG += console
CONFIG -= app_bundle

MOC_DIR += $$PWD/temp/moc/server
OBJECTS_DIR += $$PWD/temp/obj/server

TEMPLATE = app

SOURCES += src/server/clientquerys.cpp \
    src/server/main.cpp \
    src/server/tcpconnection.cpp \
    src/server/tcpserver.cpp \
    src/server/tcpthread.cpp \
    src/server/redisconnector.cpp \
    src/server/global.cpp \
    src/server/helper.cpp \

HEADERS += \
    src/server/clientquerys.h \
    src/server/tcpconnection.h \
    src/server/tcpserver.h \
    src/server/tcpthread.h \
    src/server/redisconnector.h \
    src/server/global.h \

INCLUDEPATH += $$PWD/3rd/includes


win32 {
    INCLUDEPATH += $$PWD/3rd/includes/libssh2

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/3rd/libs/windows/Debug -lqredisclient -llibssh2 -llibeay32 -lssleay32 -lgdi32 -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32
}
CONFIG(release, debug|release) {
    LIBS += -L$$PWD/3rd/libs/windows/Release -lqredisclient -llibssh2 -llibeay32 -lssleay32 -lgdi32 -lws2_32 -lkernel32 -luser32 -lshell32 -luuid -lole32 -ladvapi32
}
}

unix {
    INCLUDEPATH += $$PWD/3rd/includes/libssh2
    LIBS += -L$$PWD/3rd/libs/linux -lqredisclient -lz -lssh2
}