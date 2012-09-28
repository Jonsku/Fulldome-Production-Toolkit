#/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
DEPENDPATH += . ../Common
INCLUDEPATH += . ../Common

SOURCES +=\
    timeline.cpp \
    framesscene.cpp

HEADERS  += \
    timeline.h \
    framesscene.h

include(../Common/Common.pri)

RESOURCES += \
    ressources.qrc
