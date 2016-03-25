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
    Document/developerdatamanager.cpp \
    Document/gitsettingsdatamanager.cpp \
    Document/redminesettingsdatamanager.cpp \
    WizardUI/generalsettingspage.cpp \
    WizardUI/wizardmain.cpp

HEADERS  += \
    Document/developerdatamanager.h \
    Document/gitsettingsdatamanager.h \
    Document/redminesettingsdatamanager.h \
    apputils.h \
    WizardUI/generalsettingspage.h \
    WizardUI/wizardmain.h

FORMS    += \
    WizardUI/generalsettingspage.ui \
    WizardUI/wizardmain.ui
