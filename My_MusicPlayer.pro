#-------------------------------------------------
#
# Project created by QtCreator 2018-08-24T17:01:38
#
#-------------------------------------------------

QT       += core gui multimedia network
RC_ICONS = logo.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = My_MusicPlaer
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    kugou.cpp \
    lrc_widget.cpp

HEADERS  += widget.h \
    kugou.h \
    lrc_widget.h

FORMS    += widget.ui \
    lrc_widget.ui

RESOURCES += \
    ic.qrc

DISTFILES +=
