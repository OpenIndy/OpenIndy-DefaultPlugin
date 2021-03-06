#-------------------------------------------------
#
# Project created by QtCreator 2019-06-12T15:53:07
#
#-------------------------------------------------
CONFIG += c++11
QT       += testlib

QT       += core gui widgets serialport xml

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_function.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

# test dependencies
INCLUDEPATH += \
    $$PWD/../.. \
    $$PWD/../../functions/objectTransformation \
    $$PWD/../../functions/fit \
    $$PWD/../../functions/construct \
    $$PWD/../../lib/OpenIndy-Core/include/plugin \
    $$PWD/../../lib/OpenIndy-Core/include/util \
    $$PWD/../../lib/OpenIndy-Core/include \
    $$PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/include \
    $$PWD/../../lib/OpenIndy-Core/include/geometry \
    $$PWD/../../lib/OpenIndy-Core/include/plugin/function \
    $$PWD/../../lib/OpenIndy-Core/include/plugin/simulation \
    $$PWD/../../lib/OpenIndy-Core/include/plugin/sensor \
    $$PWD/../../lib/OpenIndy-Core/include/plugin/networkAdjustment \
    $$PWD/../../lib/OpenIndy-Core/include/plugin/tool \
    $$PWD/../../lib/OpenIndy-Core/include/plugin/exchange \
    $$PWD/../../functions/systemTransformation \
    $$PWD/../../sensors/laserTracker/pseudoTracker \
    $$PWD/../../sensors/tachymeter/LeicaGeoCom \
    $$PWD/../../simulations/simplePolarMeasurement \
    $$PWD/../../exchange \
    $$PWD/../../cf

CONFIG(debug, debug|release) {
    BUILD_DIR=debug
} else {
    BUILD_DIR=release
}

linux-g++ {
LIBS += \
    "$$PWD/../../bin/$$BUILD_DIR/.obj/*.o"

LIBS += \
    -L$$PWD/../../lib/OpenIndy-Core/bin/$$BUILD_DIR -lopenIndyCore \
    -L$$PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/bin/$$BUILD_DIR -lopenIndyMath

} else : win32-g++ {
LIBS += \
    "$$PWD/../../bin/$$BUILD_DIR/.obj/*.o"

LIBS += \
    -L$$PWD/../../lib/OpenIndy-Core/bin/$$BUILD_DIR -lopenIndyCore1 \
    -L$$PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/bin/$$BUILD_DIR -lopenIndyMath1

} else : win32 {
LIBS += \
    "$$PWD/../../bin/$$BUILD_DIR/.obj/*.obj"

LIBS += \
    -L$$PWD/../../lib/OpenIndy-Core/bin/$$BUILD_DIR -lopenIndyCore1 \
    -L$$PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/bin/$$BUILD_DIR -lopenIndyMath1
}

win32 {
# x86_64
    contains(QMAKE_HOST.arch, x86_64) {
LIBS += \
    -L$$PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win64 -lblas_win64_MT \
    -L$$PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win64 -llapack_win64_MT
    } else {
# x86_32
LIBS += \
    -L$$PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win32 -lblas_win32_MT \
    -L$$PWD/../../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win32 -llapack_win32_MT

    }
}
