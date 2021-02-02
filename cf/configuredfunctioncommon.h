#ifndef CONFIGUREDFUNCTION
#define CONFIGUREDFUNCTION

#include <QString>
#include <QPointer>
#include <QList>
#include <QMultiMap>

#include "function.h"
#include "constructfunction.h"
#include "featurewrapper.h"
#include "types.h"
#include "treeutil.h"
#include "configuredfunctionconfig.h"

using namespace oi;

struct CFContext {

     ConfiguredFunctionConfig config;
     QMap<int, QList<InputElement> > inputElements;
     // all necessary funktions
     QList<QPointer<Function> > functions;

     QPointer<Function> baseFunction;
     QPointer<FeatureWrapper> baseFeature;

     QMap<int, QList<InputElement> > global_inputElements;
     QPointer<FeatureWrapper> global_feature;

     /**
      * @brief getFunction
      * @param name
      * @return function instance and removes function from list
      */
     QPointer<Function> getFunction(QString name) {
         for(int index = 0; index < functions.size(); index++) {
             if(!functions[index].isNull() && functions[index]->getMetaData().name.compare(name) == 0) {
                 QPointer<Function> f = functions.takeAt(index); // remove from list
                 return f;
             }
         }

         throw logic_error(QString("no function found: %1").arg(name).toLocal8Bit().data());
     }

     InputElement getInputElement(QString name) {
         QList<InputElement> ies = this->global_inputElements.value(config.getNeededElementNames().indexOf(name));
         if(ies.size() == 0) {
             throw logic_error(QString("invalid count (%1) of InputElements: %2").arg(ies.size()).arg(name).toLocal8Bit().data());
         }
         return ies.first();
     }

     /**
      * @brief createApplicableFeature
      * @param featureType
      * @return feature for element of featuresTypes
      */
     QPointer<FeatureWrapper> createApplicableFeature(FeatureTypes featureType) {
            QPointer<FeatureWrapper> featureWrapper = new FeatureWrapper();

            switch(featureType){
                case ePointFeature: {
                    QPointer<Point> feature = new Point(false);
                    feature->setFeatureName("generated Point");
                    featureWrapper->setPoint(feature);
                    return featureWrapper;
                }
                case eScalarEntityDistanceFeature: {
                    QPointer<ScalarEntityDistance> feature = new ScalarEntityDistance(false);
                    feature->setFeatureName("generated ScalarEntityDistance");
                    featureWrapper->setScalarEntityDistance(feature);
                    return featureWrapper;
                }
            }

            throw logic_error(QString("can not create applicable feature").toLocal8Bit().data());
     }
     InputElement createInputElement(QPointer<FeatureWrapper> feature) {
        InputElement element;
        element.label = QString("generated InputElement: %1").arg(feature->getFeatureTypeString());

        switch (feature->getFeatureTypeEnum()) {
        case ePointFeature:
            element.typeOfElement = ePointElement;
            element.point = feature->getPoint();
            element.geometry = feature->getPoint();
            break;
        case eScalarEntityDistanceFeature:
            element.typeOfElement = eScalarEntityDistanceElement;
            element.scalarEntityDistance = feature->getScalarEntityDistance();
            element.geometry = feature->getScalarEntityDistance();
            break;
        default:
            throw logic_error(QString("can not create input element for feature type: %1").arg(feature->getFeatureTypeString()).toLocal8Bit().data());
        }
        return element;
     }
};

/**
 *
 * @brief The CFFunctionData struct
 */
struct CFFunctionData {

    // current (this) function
    QPointer<Function> function;

    // feature returned / modified by this function
    QPointer<FeatureWrapper> feature;

    QString prettyPrint() {
        QString s;
        s += "function: '" + (this->function.isNull() ? "null" : this->function->getMetaData().name)
            + "', feature: '" + (this->feature.isNull() ? "null" : this->feature->getFeature()->getFeatureName() + "', featureType: '" + this->feature->getFeatureTypeString());
        return s;
    }

};

class CFUtil {

public:

    static FeatureTypes featureTypesForName(QString name) {

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
        throw logic_error(QString("no FeatureTypes for \"%1\" found").arg(name).toLocal8Bit().data());
    }
};

#endif // CONFIGUREDFUNCTION

