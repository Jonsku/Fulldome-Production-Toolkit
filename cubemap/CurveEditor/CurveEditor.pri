#/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
DEPENDPATH += . ../Common
INCLUDEPATH += . ../Common

HEADERS  += curvesview.h \
    curveeditor.h \
    curveslist.h \
    curvelistitemwidget.h \
    curvecursor.h

SOURCES += curvesview.cpp \
    curveeditor.cpp \
    curveslist.cpp \
    curvelistitemwidget.cpp \
    curvecursor.cpp


include(../Common/Common.pri)

RESOURCES += \
    ../CurveEditor/icons.qrc
