#-------------------------------------------------
#
# Project created by QtCreator 2019-07-12T10:41:46
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += network
QT       += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vending_machine_v3
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    dbhelper.cpp \
    mydrinkbtn.cpp \
    citymanager.cpp \
    paydialog.cpp \
    logindialog.cpp \
    smtp.cpp

HEADERS  += widget.h \
    dbhelper.h \
    mydrinkbtn.h \
    citymanager.h \
    paydialog.h \
    logindialog.h \
    smtp.h

FORMS    += widget.ui \
    paydialog.ui \
    logindialog.ui

RESOURCES += \
    myresource.qrc
CONFIG += C++11
RC_ICONS = drink.ico
