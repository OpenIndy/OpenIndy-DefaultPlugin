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

     QMap<int, QList<InputElement> > global_inputElements;
     QPointer<FeatureWrapper> global_feature;
     QPointer<Function> baseFunction;
     QPointer<FeatureWrapper> baseFeature;

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

     InputElement getInputElementsByName(QString name) {
         QList<InputElement> ies = this->global_inputElements.value(config.getNeededElementNames().indexOf(name));
         if(ies.size() == 0) {
             throw logic_error(QString("invalid count (%1) of InputElements: %2").arg(ies.size()).arg(name).toLocal8Bit().data());
         }
         return ies.first();
     }

     InputElement getFeatureInputElementsByName(QPointer<FeatureWrapper> feat) {
         switch(feat->getFeatureTypeEnum()) {
         case ePointFeature: {
             InputElement ie;
             ie.label = QString("%1").arg("point");
             ie.typeOfElement = ePointElement;
             ie.point = feat->getPoint();
             ie.geometry = feat->getGeometry();

             return ie;
         }
         }

         throw logic_error(QString("not implemented: FeatureTypeEnum: %1").arg(feat->getFeatureTypeString()).toLocal8Bit().data());

     }

     QPointer<FeatureWrapper> _createApplicableFeature(QList<FeatureTypes> fts) {
         QPointer<FeatureWrapper> fw = new FeatureWrapper();

         FeatureTypes ft = fts.first();
         switch (ft) {
         case ePointFeature: {
             QPointer<Point> point = new Point(false);
             fw->setPoint(point);
             break;
         }
         default:
             throw logic_error(QString("not implemented: FeatureTypes: %1").arg(ft).toLocal8Bit().data());
         }

         return fw;
     }

     bool exec(QPointer<FeatureWrapper> feature, QList<QPointer<Function> > functions) {
         foreach(QPointer<Function> func, functions){
             // break if the function pointer is not valid
             if(func.isNull()){
                 return false;
             }
             if(!func->getApplicableFor().contains(feature->getFeatureTypeEnum())) {
                 throw logic_error(QString("function '%1' is not applicable for feature type '%2'").arg(func->getMetaData().name).arg(feature->getFeatureTypeString()).toLocal8Bit().data());
             }

             int depth = 0;
             qDebug() << QString("%1exec function: %2 with feature: %3").arg(QString(depth*2, QChar(' '))).arg(func->getMetaData().name).arg(feature->getFeature()->getFeatureName());
             // try to solve the current function
             if(!func->exec(feature)){
                 qDebug() << "false";
                 return false;
             }
         }

         return true;
     }

     /**
      * @brief createApplicableFeature
      * @param featureTypes
      * @return feature for first element of featuresTypes
      */
     QPointer<FeatureWrapper> createApplicableFeature(QList<FeatureTypes> featureTypes) {
            QList<FeatureTypes>::iterator featureType;
            for (featureType = featureTypes.begin(); featureType != featureTypes.end(); ++featureType) {
                return createApplicableFeature(*featureType);
            }

            throw logic_error(QString("can not create applicable feature").toLocal8Bit().data());
     }
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
    int level;
    // current (this) function
    QPointer<Function> function;
    // feature returned by this function
    QPointer<FeatureWrapper> feature;
    QList<QPointer<Function> > featureFunctions;

    QString prettyPrint() {
        QString s;
        s += "function: '" + (this->function.isNull() ? "null" : this->function->getMetaData().name)
            + "', feature: '" + (this->feature.isNull() ? "null" : this->feature->getFeature()->getFeatureName() + "', featureType: '" + this->feature->getFeatureTypeString())
            + "', featureFunctions: [ ";
        foreach(QPointer<Function> f, this->featureFunctions) {
            s += "'" + f->getMetaData().name + "', ";
        }
        s += " ], ";
        return s;
    }

};

struct CFData {
    QStack<CFFunctionData> functionData;
    QStack<QPointer<FeatureWrapper> > baseFeature;// TODO necessary ???
    QMap<int, QPointer<FeatureWrapper> > featurePerLevel;// TODO necessary ???

    QPointer<FeatureWrapper> findFeatureForLevel(int level) { // TODO necessary ???
        for(int i=functionData.size()-1; i>=0; i--) {
            if(functionData[i].level == level) {
                return functionData[i].feature;
            }
        }
        return 0;
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

