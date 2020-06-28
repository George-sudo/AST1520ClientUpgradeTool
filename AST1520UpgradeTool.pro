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
    mydialog.cpp \
    checkout.cpp \
    unzip_ufw.cpp

HEADERS  += mainwindow.h \
    loginwindow.h \
    mydialog.h \
    checkout.h \
    unzip_ufw.h

INCLUDEPATH += $$PWD/include

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/lib -lquazipd
} else {
    LIBS += -L$$PWD/lib -lquazip
}

FORMS    += mainwindow.ui \
    loginwindow.ui \
    mydialog.ui

RESOURCES += \
    image.qrc

DISTFILES +=
