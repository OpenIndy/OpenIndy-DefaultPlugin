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

DEFINES += SRCDIR=$$shell_quote($$PWD)

# test dependencies
INCLUDEPATH += \
    ../include \
    ../.. \
    ../../functions/objectTransformation \
    ../../functions/fit \
    ../../functions/construct \
    ../../lib/OpenIndy-Core/include/plugin \
    ../../lib/OpenIndy-Core/include/util \
    ../../lib/OpenIndy-Core/include \
    ../../lib/OpenIndy-Core/lib/OpenIndy-Math/include \
    ../../lib/OpenIndy-Core/include/geometry \
    ../../lib/OpenIndy-Core/include/plugin/function \
    ../../lib/OpenIndy-Core/include/plugin/simulation \
    ../../lib/OpenIndy-Core/include/plugin/sensor \
    ../../lib/OpenIndy-Core/include/plugin/networkAdjustment \
    ../../lib/OpenIndy-Core/include/plugin/tool \
    ../../lib/OpenIndy-Core/include/plugin/exchange \
    ../../functions/systemTransformation \
    ../../sensors/laserTracker/pseudoTracker \
    ../../sensors/tachymeter/LeicaGeoCom \
    ../../simulations/simplePolarMeasurement \
    ../../exchange \
    ../../cf

CONFIG(debug, debug|release) {
    BUILD_DIR=debug
} else {
    BUILD_DIR=release
}

linux-g++ {
LIBS += \
    "../../bin/$$BUILD_DIR/.obj/*.o"

LIBS += \
    -L../../lib/OpenIndy-Core/bin/$$BUILD_DIR -lopenIndyCore \
    -L../../lib/OpenIndy-Core/lib/OpenIndy-Math/bin/$$BUILD_DIR -lopenIndyMath

} else : win32-g++ {
LIBS += \
    "../../bin/$$BUILD_DIR/.obj/*.o"

LIBS += \
    -L../../lib/OpenIndy-Core/bin/$$BUILD_DIR -lopenIndyCore1 \
    -L../../lib/OpenIndy-Core/lib/OpenIndy-Math/bin/$$BUILD_DIR -lopenIndyMath1

} else : win32 {
LIBS += \
    "../../bin/$$BUILD_DIR/.obj/*.obj"

LIBS += \
    -L../../lib/OpenIndy-Core/bin/$$BUILD_DIR -lopenIndyCore1 \
    -L../../lib/OpenIndy-Core/lib/OpenIndy-Math/bin/$$BUILD_DIR -lopenIndyMath1
}

win32 {
# x86_64
    contains(QMAKE_HOST.arch, x86_64) {
LIBS += \
    -L../../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win64 -lblas_win64_MT \
    -L../../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win64 -llapack_win64_MT
    } else {
# x86_32
LIBS += \
    -L../../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win32 -lblas_win32_MT \
    -L../../lib/OpenIndy-Core/lib/OpenIndy-Math/lib/armadillo-3.910.0/examples/lib_win32 -llapack_win32_MT

    }
}

QMAKE_EXTRA_TARGETS += run-test
win32{
run-test.commands = $$shell_quote($$OUT_PWD/$$BUILD_DIR/$$TARGET) -o $$shell_path(../reports/$${TARGET}.xml),xml
}else:linux{
run-test.commands = $$shell_quote($$OUT_PWD/$$TARGET) -o $$shell_path(../reports/$${TARGET}.xml),xml
}
