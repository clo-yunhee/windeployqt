option(host_build)
QT = core-private
DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII QT_NO_FOREACH

SOURCES += main.cpp qmlutils.cpp utils.cpp

CONFIG += force_bootstrap

win32: LIBS += -lshlwapi

INCLUDEPATH += /usr/include/parser-library
LIBS += -lpe-parser-library

QMAKE_TARGET_DESCRIPTION = "Qt Windows Deployment Tool"
