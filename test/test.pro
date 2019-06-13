#-------------------------------------------------
#
# Project created by QtCreator 2019-06-12T15:53:07
#
#-------------------------------------------------

QT       += testlib

QT       += core gui widgets serialport xml

TARGET = tst_defaultplugintest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_defaultplugintest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

# test dependencies
INCLUDEPATH += \
    $$PWD/.. \
    $$PWD/../exchange \
    $$PWD/../lib/OpenIndy-Core/include/plugin/exchange \
    $$PWD/../lib/OpenIndy-Core/include/plugin \
    $$PWD/../lib/OpenIndy-Core/include/util \
    $$PWD/../lib/OpenIndy-Core/include \
    $$PWD/../lib/OpenIndy-Core/lib/OpenIndy-Math/include \
    $$PWD/../lib/OpenIndy-Core/include/plugin/simulation \
    $$PWD/../lib/OpenIndy-Core/include/plugin/sensor \
    $$PWD/../lib/OpenIndy-Core/include/geometry \
    $$PWD/../lib/OpenIndy-Core/include/plugin/function

CONFIG(debug, debug|release) {
LIBS += \
    "$$PWD/../bin/debug/.obj/*.obj" \
    "$$PWD/../lib/OpenIndy-Core/bin/debug/.obj/*.obj" \
    "$$PWD/../lib/OpenIndy-Core/lib/OpenIndy-Math/bin/debug/.obj/*.obj"

} else {
LIBS += \
    "$$PWD/../bin/release/.obj/*.obj" \
    "$$PWD/../lib/OpenIndy-Core/bin/release/.obj/*.obj" \
    "$$PWD/../lib/OpenIndy-Core/lib/OpenIndy-Math/bin/release/.obj/*.obj"

}

win32 {
# x86_64
    contains(QMAKE_HOST.arch, x86_64) {
LIBS += \
    -L$$PWD/../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win64 -lblas_win64_MT \
    -L$$PWD/../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win64 -llapack_win64_MT
    } else {
# x86_32
LIBS += \
    -L$$PWD/../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win32 -lblas_win32_MT \
    -L$$PWD/../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win32 -llapack_win32_MT

    }
}
