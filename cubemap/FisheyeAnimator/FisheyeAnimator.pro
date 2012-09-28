#-------------------------------------------------
#
# Project created by QtCreator 2011-06-22T13:58:52
# Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php
#-------------------------------------------------

QT       += core gui

TARGET = CubeAnimator
TEMPLATE = app
#QMAKE_LFLAGS += -static-libgcc
ICON = macos_icon.icns

DEPENDPATH += . ../CubeMapPreview ../CubeMapLoader ../CurveEditor ../TimeLine ../ExportModule
INCLUDEPATH += . ../CubeMapPreview ../CubeMapLoader ../CurveEditor ../TimeLine ../ExportModule

include(../CubeMapPreview/CubeMapPreview.pri)
include(../CubeMapLoader/CubeMapLoader.pri)
include(../CurveEditor/CurveEditor.pri)
include(../TimeLine/TimeLine.pri)
include(../ExportModule/ExportModule.pri)


SOURCES += main.cpp\
        mainwindow.cpp \
    mainwidget.cpp \
    ../Common/keyframe.cpp

HEADERS  += mainwindow.h \
    mainwidget.h \
    ../Common/keyframe.h

HEADERS += ruler.h \
    curveitem.h \
    curvehandleitem.h \
    curvevariable.h \
    variablecolorpalette.h

SOURCES += ruler.cpp \
    curveitem.cpp\
    curvehandleitem.cpp \
    curvevariable.cpp \
    variablecolorpalette.cpp
