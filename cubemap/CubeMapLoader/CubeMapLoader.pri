#/* Licensed under the MIT license: http://www.opensource.org/licenses/mit-license.php */
DEPENDPATH += . ../Common
INCLUDEPATH += . ../Common

SOURCES += cubemaploader.cpp cubefacelabel.cpp

HEADERS  += cubemaploader.h cubefacelabel.h

include(../Common/Common.pri)

RESOURCES += LoaderRes.qrc
