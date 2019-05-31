
QT += core gui widgets serialport xml svg network
QT += testlib

TEMPLATE = app
TARGET = test

INCLUDEPATH += .

include($PWD/../../build/dependencies.pri)
include($PWD/../../build/defaults.pri)

include($PWD/../../lib/OpenIndy-Core/build/dependencies.pri)
include($PWD/../../lib/OpenIndy-Core/build/defaults.pri)

include($PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/build/dependencies.pri)
include($PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/build/defaults.pri)

CONFIG(release, release|debug):DEFINES += P_DEFAULT_PLUGIN_DLL=\\\"OpenIndy-DefaultPlugin/bin/release/p_defaultPlugin1.dll\\\"
else:CONFIG(debug, release|debug):DEFINES += P_DEFAULT_PLUGIN_DLL=\\\"OpenIndy-DefaultPlugin/bin/debug/p_defaultPlugind1.dll\\\"


message($$CONFIG)
message($$DEFINES)

# Input
SOURCES += bestfitpointtest.cpp
