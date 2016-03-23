#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T16:06:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OMPDevelopersStatistics
TEMPLATE = app


SOURCES += main.cpp\
        wizardmain.cpp \
    Document/developerdatamanager.cpp \
    Document/gitsettingsdatamanager.cpp \
    Document/redminesettingsdatamanager.cpp

HEADERS  += wizardmain.h \
    Document/developerdatamanager.h \
    Document/gitsettingsdatamanager.h \
    Document/redminesettingsdatamanager.h \
    apputils.h

FORMS    += wizardmain.ui
