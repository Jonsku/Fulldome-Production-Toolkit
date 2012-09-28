
#-------------------------------------------------
#
# Project created by QtCreator 2011-07-09T13:38:33
#/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
#-------------------------------------------------

QT       += core

#QT       -= gui

TARGET = cubemap2fisheye
CONFIG   += console
CONFIG   -= app_bundle
QMAKE_LFLAGS += -static-libgcc

CONFIG-=app_bundle #You want to keep this as a command line tool

TEMPLATE = app

DEPENDPATH += . ./libs
INCLUDEPATH += . ./libs

DEPENDPATH += . ../Common ../ExportModule
INCLUDEPATH += . ../Common ../ExportModule

include(./libs/tclap/tclap.pri)

INCLUDEPATH += ../include
LIBS += -lqjson


SOURCES += main.cpp \
    cubefacedata.cpp

HEADERS += cubefacedata.h

SOURCES += jsonio.cpp

HEADERS  += jsonio.h

