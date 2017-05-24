#-------------------------------------------------
#
# Project created by QtCreator 2017-03-07T20:00:11
#
#-------------------------------------------------

QT       += core gui network webkitwidgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iot_desktop_qt
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        logindialog.cpp \
    frmmessagebox.cpp \
    iconhelper.cpp \
    mainwindow.cpp \
    global.cpp \
    softregdialog.cpp \
    yd_t_1363_3_2005/yd_t_1363_3_2005.cpp \
    JasonQt_SerialPort/JasonQt_SerialPort.cpp \
    MyQt_SerialPort/spcomm.cpp \
    BMS/batwidget.cpp \
    SocketClient/TcpClient.cpp \
    DCM/devdircurmeter.cpp

HEADERS  += logindialog.h \
    frmmessagebox.h \
    iconhelper.h \
    myhelper.h \
    mainwindow.h \
    global.h \
    softregdialog.h \
    yd_t_1363_3_2005/yd_t_1363_3_2005.h \
    JasonQt_SerialPort/JasonQt_SerialPort.h \
    MyQt_SerialPort/spcomm.h \
    BMS/batwidget.h \
    SocketClient/TcpClient.h \
    DCM/devdircurmeter.h

FORMS    += \
    frmmessagebox.ui \
    logindialog.ui \
    mainwindow.ui \
    softregdialog.ui

RC_FILE = myapp.rc

RESOURCES += \
    resource.qrc
