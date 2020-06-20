#-------------------------------------------------
#
# Project created by QtCreator 2020-06-11T14:07:26
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AST1520UpgradeTool
TEMPLATE = app

RC_FILE = logo.rc
SOURCES += main.cpp\
        mainwindow.cpp \
    loginwindow.cpp \
    crc.cpp \
    mydialog.cpp

HEADERS  += mainwindow.h \
    loginwindow.h \
    crc.h \
    mydialog.h

FORMS    += mainwindow.ui \
    loginwindow.ui \
    mydialog.ui

RESOURCES += \
    image.qrc

DISTFILES +=
