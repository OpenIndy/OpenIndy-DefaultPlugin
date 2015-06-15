#-------------------------------------------------
#
# Project created by QtCreator 2013-10-12T15:43:14
#
#-------------------------------------------------

QT       += gui
QT += widgets
QT += serialport
QT += xml

TEMPLATE = lib
#TEMPLATE = app
TARGET = $$qtLibraryTarget(p_defaultPlugin)

CONFIG += plugin
CONFIG += c++11

DEFINES += DEFAULT_LIBRARY
DEFINES += WITH_FITTING_UTILS

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/bin/debug
} else {
    DESTDIR = $$PWD/bin/release
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

#--------------
# OpenIndy-Core
#--------------

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/OpenIndy-Core/bin/release/ -lopenIndyCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/OpenIndy-Core/bin/debug/ -lopenIndyCore
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/OpenIndy-Core/bin/release/ -lopenIndyCore
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/OpenIndy-Core/bin/debug/ -lopenIndyCore

INCLUDEPATH += -L$$PWD/lib/OpenIndy-Core/bin/debug
DEPENDPATH += -L$$PWD/lib/OpenIndy-Core/bin/debug

INCLUDEPATH += -L$$PWD/lib/OpenIndy-Core/bin/release
DEPENDPATH += -L$$PWD/lib/OpenIndy-Core/bin/release

#--------------
# OpenIndy-Math
#--------------

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/OpenIndy-Core/lib/OpenIndy-Math/bin/release/ -lopenIndyMath
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/OpenIndy-Core/lib/OpenIndy-Math/bin/debug/ -lopenIndyMath
else:unix:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/OpenIndy-Core/lib/OpenIndy-Math/bin/release/ -lopenIndyMath
else:unix:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/OpenIndy-Core/lib/OpenIndy-Math/bin/debug/ -lopenIndyMath

INCLUDEPATH += -L$$PWD/lib/OpenIndy-Core/lib/OpenIndy-Math/bin/debug
DEPENDPATH += -L$$PWD/lib/OpenIndy-Core/lib/OpenIndy-Math/bin/debug

INCLUDEPATH += -L$$PWD/lib/OpenIndy-Core/lib/OpenIndy-Math/bin/release
DEPENDPATH += -L$$PWD/lib/OpenIndy-Core/lib/OpenIndy-Math/bin/release

INCLUDEPATH += $$PWD

INCLUDEPATH += $$PWD/sensors/laserTracker/pseudoTracker
INCLUDEPATH += $$PWD/sensors/tachymeter/LeicaGeoCom
INCLUDEPATH += $$PWD/sensors/undefinedSensor/pseudoSensor
INCLUDEPATH += $$PWD/functions/construct
INCLUDEPATH += $$PWD/functions/fit
INCLUDEPATH += $$PWD/functions/objectTransformation
INCLUDEPATH += $$PWD/functions/systemTransformation
INCLUDEPATH += $$PWD/functions/generateFeature
INCLUDEPATH += $$PWD/functions/generateFeature/pointcloud_segmentation
INCLUDEPATH += $$PWD/simulations/simplePolarMeasurement
INCLUDEPATH += $$PWD/exchange

INCLUDEPATH += $$PWD/lib/fitting
INCLUDEPATH += $$PWD/lib/fitting/util

INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include/geometry
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include/util
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include/plugin
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include/plugin/exchange
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include/plugin/function
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include/plugin/networkAdjustment
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include/plugin/sensor
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include/plugin/simulation
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/include/plugin/tool
INCLUDEPATH += $$PWD/lib/OpenIndy-Core/lib/OpenIndy-Math/include

SOURCES += \
    functions/construct/p_intersectlineplane.cpp \
    functions/construct/p_intersectplaneplane.cpp \
    functions/construct/p_linefrompoints.cpp \
    functions/construct/p_planefrompoints.cpp \
    functions/construct/p_pointbetweentwopoints.cpp \
    functions/construct/p_pointfrompoints.cpp \
    functions/construct/p_spherefrompoints.cpp \
    functions/fit/p_bestfitcircle.cpp \
    functions/fit/p_bestfitline.cpp \
    functions/fit/p_bestfitplane.cpp \
    functions/fit/p_bestfitpoint.cpp \
    functions/objectTransformation/p_changeradius.cpp \
    functions/systemTransformation/materials.cpp \
    functions/systemTransformation/p_extendedtemperaturecompensation.cpp \
    functions/systemTransformation/p_helmert6param.cpp \
    functions/systemTransformation/p_helmert7Param.cpp \
    functions/systemTransformation/p_simpletemperaturecompensation.cpp \
    exchange/p_oiexchangeascii.cpp \
    sensors/laserTracker/pseudoTracker/p_pseudotracker.cpp \
    sensors/tachymeter/LeicaGeoCom/p_leicatachymeter.cpp \
    simulations/simplePolarMeasurement/simplepolarmeasurement.cpp \
    functions/objectTransformation/p_register.cpp \
    functions/objectTransformation/p_translatebyvalue.cpp \
    lib/fitting/cfitting_approxfunc.cpp \
    lib/fitting/cfitting_circle.cpp \
    lib/fitting/cfitting_ellipse.cpp \
    lib/fitting/cfitting_it.cpp \
    lib/fitting/cfitting_matrix.cpp \
    lib/fitting/cfitting_plane.cpp \
    lib/fitting/cfitting_sphere.cpp \
    lib/fitting/cfitting_util.cpp \
    lib/fitting/util/Getopt.cpp \
    lib/fitting/util/iskeyword.cpp \
    lib/fitting/util/microtim.cpp \
    lib/fitting/util/mystring.cpp \
    lib/fitting/util/rho.cpp \
    p_factory.cpp \
    main.cpp \
    functions/objectTransformation/p_rectifytopoint.cpp \
    functions/objectTransformation/p_shift.cpp \
    functions/objectTransformation/p_rectifytovector.cpp \
    functions/fit/p_bestfitcylinder.cpp \
    functions/fit/p_bestfitcircleinplane.cpp \
    functions/fit/p_bestfitplane_kern.cpp \
    functions/fit/p_bestfitsphere_kern.cpp \
    functions/fit/p_bestfitsphere.cpp

HEADERS += \
    functions/construct/p_intersectlineplane.h \
    functions/construct/p_intersectplaneplane.h \
    functions/construct/p_linefrompoints.h \
    functions/construct/p_planefrompoints.h \
    functions/construct/p_pointbetweentwopoints.h \
    functions/construct/p_pointfrompoints.h \
    functions/construct/p_spherefrompoints.h \
    functions/fit/p_bestfitcircle.h \
    functions/fit/p_bestfitline.h \
    functions/fit/p_bestfitplane.h \
    functions/fit/p_bestfitpoint.h \
    functions/objectTransformation/p_changeradius.h \
    functions/systemTransformation/materials.h \
    functions/systemTransformation/p_extendedtemperaturecompensation.h \
    functions/systemTransformation/p_helmert6param.h \
    functions/systemTransformation/p_helmert7Param.h \
    functions/systemTransformation/p_simpletemperaturecompensation.h \
    exchange/p_oiexchangeascii.h \
    sensors/laserTracker/pseudoTracker/p_pseudotracker.h \
    sensors/tachymeter/LeicaGeoCom/p_leicatachymeter.h \
    simulations/simplePolarMeasurement/simplepolarmeasurement.h \
    functions/objectTransformation/p_translatebyvalue.h \
    functions/objectTransformation/p_register.h \
    lib/fitting/cfitting_approxfunc.h \
    lib/fitting/cfitting_circle.h \
    lib/fitting/cfitting_ellipse.h \
    lib/fitting/cfitting_it.h \
    lib/fitting/cfitting_matrix.h \
    lib/fitting/cfitting_plane.h \
    lib/fitting/cfitting_sphere.h \
    lib/fitting/cfitting_util.h \
    lib/fitting/util/getopt.h \
    lib/fitting/util/iskeyword.h \
    lib/fitting/util/microtim.h \
    lib/fitting/util/mystring.h \
    lib/fitting/util/rho.h \
    p_factory.h \
    functions/objectTransformation/p_rectifytopoint.h \
    functions/objectTransformation/p_shift.h \
    functions/objectTransformation/p_rectifytovector.h \
    functions/fit/p_bestfitcylinder.h \
    functions/fit/p_bestfitcircleinplane.h \
    functions/fit/p_bestfitplane_kern.h \
    functions/fit/p_bestfitsphere_kern.h \
    functions/fit/p_bestfitsphere.h \
    lib/OpenIndy-Core/lib/OpenIndy-Math/include/chooselalib.h \
    lib/OpenIndy-Core/lib/OpenIndy-Math/include/global.h \
    lib/OpenIndy-Core/lib/OpenIndy-Math/include/linearalgebra.h \
    lib/OpenIndy-Core/lib/OpenIndy-Math/include/oimat.h \
    lib/OpenIndy-Core/lib/OpenIndy-Math/include/oivec.h

OTHER_FILES += metaInfo.json

FORMS += \
    functions/generateFeature/ps_loadingdialog.ui
