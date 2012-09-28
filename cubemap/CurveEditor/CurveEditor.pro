#-------------------------------------------------
#
# Project created by QtCreator 2011-06-08T13:14:24
#
#-------------------------------------------------

QT       += core gui

TARGET = CurveEditor
TEMPLATE = lib
CONFIG += static
DEPENDPATH += .
INCLUDEPATH += .

include(CurveEditor.pri)

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




