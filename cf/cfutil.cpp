#include <stdexcept>
#include "cfutil.h"

FeatureTypes CFUtil::featureTypesForName(QString name) {

    if (name.compare("Circle") == 0 ) {
        return eCircleFeature;
    } else if (name.compare("Cone") == 0 ) {
        return eConeFeature;
    } else if (name.compare("Cylinder") == 0 ) {
        return eCylinderFeature;
    } else if (name.compare("Ellipse") == 0 ) {
        return eEllipseFeature;
    } else if (name.compare("Ellipsoid") == 0 ) {
        return eEllipsoidFeature;
    } else if (name.compare("Hyperboloid") == 0 ) {
        return eHyperboloidFeature;
    } else if (name.compare("Line") == 0 ) {
        return eLineFeature;
    } else if (name.compare("Nurbs") == 0 ) {
        return eNurbsFeature;
    } else if (name.compare("Paraboloid") == 0 ) {
        return eParaboloidFeature;
    } else if (name.compare("Plane") == 0 ) {
        return ePlaneFeature;
    } else if (name.compare("Point") == 0 ) {
        return ePointFeature;
    } else if (name.compare("PointCloud") == 0 ) {
        return ePointCloudFeature;
    } else if (name.compare("ScalarEntityAngle") == 0 ) {
        return eScalarEntityAngleFeature;
    } else if (name.compare("ScalarEntityDistance") == 0 ) {
        return eScalarEntityDistanceFeature;
    } else if (name.compare("ScalarEntityMeasurementSeries") == 0 ) {
        return eScalarEntityMeasurementSeriesFeature;
    } else if (name.compare("ScalarEntityTemperature") == 0 ) {
        return eScalarEntityTemperatureFeature;
    } else if (name.compare("SlottedHole") == 0 ) {
        return eSlottedHoleFeature;
    } else if (name.compare("Sphere") == 0 ) {
        return eSphereFeature;
    } else if (name.compare("Torus") == 0 ) {
        return eTorusFeature;
    } else if (name.compare("CoordinateSystem") == 0 ) {
        return eCoordinateSystemFeature;
    } else if (name.compare("Station") == 0 ) {
        return eStationFeature;
    } else if (name.compare("TrafoParam") == 0 ) {
        return eTrafoParamFeature;
    }
    throw std::logic_error(QString("no FeatureTypes for \"%1\" found").arg(name).toLocal8Bit().data());
}

