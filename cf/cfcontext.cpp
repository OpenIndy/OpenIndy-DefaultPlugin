#include "cfcontext.h"

CFContext::CFContext() {

}

/**
 * @brief getFunction
 * @param name
 * @return function instance and removes function from list
 */
QPointer<Function> CFContext::getFunction(QString name) {
    for(int index = 0; index < functions.size(); index++) {
        if(!functions[index].isNull() && functions[index]->getMetaData().name.compare(name) == 0) {
            QPointer<Function> f = functions.takeAt(index); // remove from list
            return f;
        }
    }

    throw logic_error(QString("no function found: %1").arg(name).toLocal8Bit().data());
}

InputElement CFContext::getInputElement(QString name) {
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
QPointer<FeatureWrapper> CFContext::createApplicableFeature(FeatureTypes featureType) {
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

InputElement CFContext::createInputElement(QPointer<FeatureWrapper> feature) {
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
