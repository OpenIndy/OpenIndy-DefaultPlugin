#-------------------------------------------------
#
# Project created by QtCreator 2013-10-12T15:43:14
#
#-------------------------------------------------
CONFIG += c++11

QT       += core gui widgets serialport xml

TARGET = $$qtLibraryTarget(p_defaultPlugin)

TEMPLATE = lib
#TEMPLATE = app

CONFIG += plugin
CONFIG += c++11

DEFINES += DEFAULT_LIBRARY
DEFINES += WITH_FITTING_UTILS

# create metainfo.json
include($$PWD/metainfo.pri)

# version number from git
include($$PWD/version.pri)

# general build configuration
include($$PWD/config.pri)

# external libraries
include($$PWD/dependencies.pri)

# include paths
include($$PWD/include.pri)

# source files
SOURCES += \
    $$PWD/../functions/construct/p_intersectlineline.cpp \
    $$PWD/../functions/construct/p_intersectlineplane.cpp \
    $$PWD/../functions/construct/p_intersectplaneplane.cpp \
    $$PWD/../functions/construct/p_linefrompoints.cpp \
    $$PWD/../functions/construct/p_planefrompoints.cpp \
    $$PWD/../functions/construct/p_pointbetweentwopoints.cpp \
    $$PWD/../functions/construct/p_pointfrompoints.cpp \
    $$PWD/../functions/construct/p_spherefrompoints.cpp \
    $$PWD/../functions/fit/p_bestfitline.cpp \
    $$PWD/../functions/fit/p_bestfitplane.cpp \
    $$PWD/../functions/fit/p_bestfitpoint.cpp \
    $$PWD/../functions/objectTransformation/p_changeradius.cpp \
    $$PWD/../exchange/p_oiexchangeascii.cpp \
    $$PWD/../sensors/laserTracker/pseudoTracker/p_pseudotracker.cpp \
    $$PWD/../sensors/tachymeter/LeicaGeoCom/p_leicatachymeter.cpp \
    $$PWD/../simulations/simplePolarMeasurement/simplepolarmeasurement.cpp \
    $$PWD/../functions/objectTransformation/p_register.cpp \
    $$PWD/../functions/objectTransformation/p_translatebyvalue.cpp \
    $$PWD/../p_factory.cpp \
    $$PWD/../functions/objectTransformation/p_rectifytopoint.cpp \
    $$PWD/../functions/objectTransformation/p_shift.cpp \
    $$PWD/../functions/objectTransformation/p_rectifytovector.cpp \
    $$PWD/../functions/fit/p_bestfitcylinder.cpp \
    $$PWD/../functions/fit/p_bestfitcylinderfrompoints.cpp \
    $$PWD/../functions/fit/p_bestfitcircleinplane.cpp \
    $$PWD/../functions/fit/p_bestfitsphere.cpp \
    $$PWD/../functions/fit/p_bestfitcircleinplanefrompoints.cpp \
    $$PWD/../cf/cfutil.cpp \
    $$PWD/../cf/cffunctiondata.cpp \
    $$PWD/../cf/configuredfunction.cpp \
    $$PWD/../cf/functionconfigparser.cpp \
    $$PWD/../cf/distancebetweentwopoints.cpp \
    $$PWD/../cf/configuredfunctionconfig.cpp \
    $$PWD/../cf/cfcontext.cpp \
    $$PWD/../cf/cfcparameter.cpp \
    $$PWD/../cf/xyzfilter.cpp \
    $$PWD/../cf/cfvisitor.cpp

# header files
HEADERS += \
    $$PWD/../functions/construct/p_intersectlineline.h \
    $$PWD/../functions/construct/p_intersectlineplane.h \
    $$PWD/../functions/construct/p_intersectplaneplane.h \
    $$PWD/../functions/construct/p_linefrompoints.h \
    $$PWD/../functions/construct/p_planefrompoints.h \
    $$PWD/../functions/construct/p_pointbetweentwopoints.h \
    $$PWD/../functions/construct/p_pointfrompoints.h \
    $$PWD/../functions/construct/p_spherefrompoints.h \
    $$PWD/../functions/fit/p_bestfitline.h \
    $$PWD/../functions/fit/p_bestfitplane.h \
    $$PWD/../functions/fit/p_bestfitpoint.h \
    $$PWD/../functions/objectTransformation/p_changeradius.h \
    $$PWD/../exchange/p_oiexchangeascii.h \
    $$PWD/../sensors/laserTracker/pseudoTracker/p_pseudotracker.h \
    $$PWD/../sensors/tachymeter/LeicaGeoCom/p_leicatachymeter.h \
    $$PWD/../simulations/simplePolarMeasurement/simplepolarmeasurement.h \
    $$PWD/../functions/objectTransformation/p_translatebyvalue.h \
    $$PWD/../functions/objectTransformation/p_register.h \
    $$PWD/../p_factory.h \
    $$PWD/../functions/objectTransformation/p_rectifytopoint.h \
    $$PWD/../functions/objectTransformation/p_shift.h \
    $$PWD/../functions/objectTransformation/p_rectifytovector.h \
    $$PWD/../functions/fit/p_bestfitcylinder.h \
    $$PWD/../functions/fit/p_bestfitcylinderfrompoints.h \
    $$PWD/../functions/fit/p_bestfitcircleinplane.h \
    $$PWD/../functions/fit/p_bestfitsphere.h \
    $$PWD/../functions/fit/p_bestfitcircleinplanefrompoints.h \
    $$PWD/../treeutil.h \
    $$PWD/../cf/cfutil.h \
    $$PWD/../cf/cffunctiondata.h \
    $$PWD/../cf/configuredfunction.h \
    $$PWD/../cf/functionconfigparser.h \
    $$PWD/../cf/distancebetweentwopoints.h \
    $$PWD/../cf/configuredfunctionconfig.h \
    $$PWD/../cf/cfcontext.h \
    $$PWD/../cf/cfcparameter.h \
    $$PWD/../cf/xyzfilter.h \
    $$PWD/../cf/cfvisitor.h

# other files
OTHER_FILES += $$PWD/../metaInfo.json \
    $PWD/../cf/cfconfig.res


# ui files
FORMS += \
    $$PWD/../functions/generateFeature/ps_loadingdialog.ui
