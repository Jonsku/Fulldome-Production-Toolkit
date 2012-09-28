#-------------------------------------------------
#
# Project created by QtCreator 2011-07-12T19:06:49
#
#-------------------------------------------------

QT       += core gui

TARGET = ExportModule
TEMPLATE = app

DEPENDPATH += . ../CubeMapLoader ../TimeLine
INCLUDEPATH += . ../CubeMapLoader ../TimeLine

SOURCES += ../Common/keyframe.cpp

HEADERS  += ../Common/keyframe.h

include(ExportModule.pri)

include(../CubeMapLoader/CubeMapLoader.pri)
include(../TimeLine/TimeLine.pri)

OTHER_FILES += \
    test.json
