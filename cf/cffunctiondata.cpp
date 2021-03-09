#include "cffunctiondata.h"

CFFunctionData::CFFunctionData()
{

}

QString CFFunctionData::prettyPrint() {
    QString s;
    s += "function: '" + (this->function.isNull() ? "null" : this->function->getMetaData().name)
        + "', feature: '" + (this->feature.isNull() ? "null" : this->feature->getFeature()->getFeatureName() + "', featureType: '" + this->feature->getFeatureTypeString());
    return s;
}
