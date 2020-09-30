#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T16:06:26
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OMPDevelopersStatistics
TEMPLATE = app


SOURCES += main.cpp\
    Document/HTMLGenerator/tabledata.cpp \
    Document/developerdatamanager.cpp \
    WizardUI/generalsettingspage.cpp \
    WizardUI/wizardmain.cpp \
    WizardUI/developereditor.cpp \
    Document/documentdatamanager.cpp \
    WizardUI/wizardpagedocumentsupport.cpp \
    WizardUI/localsettingspage.cpp \
    Document/developerworkdata.cpp \
    Document/developerdata.cpp \
    Document/gitanalyzer.cpp \
    WizardUI/generationpage.cpp \
    Document/redmineanalyzer.cpp \
    Document/generalsettingsholder.cpp \
    Document/DevStatistics/devstatisticsdocument.cpp \
    Document/Calendar/CalendarData.cpp \
    Document/HTMLGenerator/HTMLGenerator.cpp \
    Document/HTMLGenerator/piechartdata.cpp \
    WizardUI/calendarperioddialog.cpp

HEADERS  += \
    Document/HTMLGenerator/tabledata.h \
    Document/developerdatamanager.h \
    apputils.h \
    WizardUI/generalsettingspage.h \
    WizardUI/wizardmain.h \
    WizardUI/developereditor.h \
    Document/documentdatamanager.h \
    WizardUI/wizardpagedocumentsupport.h \
    WizardUI/localsettingspage.h \
    Document/developerworkdata.h \
    Document/developerdata.h \
    Document/gitanalyzer.h \
    WizardUI/generationpage.h \
    Document/redmineanalyzer.h \
    Document/generalsettingsholder.h \
    Document/DevStatistics/devstatisticsdocument.h \
    Document/Calendar/CalendarData.h \
    Document/HTMLGenerator/HTMLGenerator.h \
    Document/HTMLGenerator/piechartdata.h \
    WizardUI/calendarperioddialog.h

FORMS    += \
    WizardUI/generalsettingspage.ui \
    WizardUI/wizardmain.ui \
    WizardUI/developereditor.ui \
    WizardUI/localsettingspage.ui \
    WizardUI/generationpage.ui \
    WizardUI/calendarperioddialog.ui

DISTFILES +=
