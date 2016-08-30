#include "p_simpletemperaturecompensation.h"

/*
void SimpleTemperatureCompensation::calcExpansion(TrafoParam &tp){

    QMap<QString,QString> stringParameter = this->scalarInputParams.stringParameter;
    QMap<QString,double> doubleParameter = this->scalarInputParams.doubleParameter;

    QString material = "";
    double actTemp = 20.0;
    double refTemp = 20.0;
    double expansionCoefficient = 0.0;
    if(stringParameter.contains("material")){
        material = static_cast<QString>(stringParameter.find("material").value());
        protMaterial = material;
        expansionCoefficient = Materials::getExpansionCoefficient(material);
        protExpansionCoeff = QString::number(expansionCoefficient,'f',6);
	}
	if(doubleParameter.contains("actualTemperature")){
        actTemp = static_cast<double>(doubleParameter.find("actualTemperature").value());
        protActTemp = QString::number(actTemp,'f',2);
    }
    if(doubleParameter.contains("referenceTemperature")){
        refTemp = static_cast<double>(doubleParameter.find("referenceTemperature").value());
        protRefTemp = QString::number(refTemp,'f',2);
    }
	
	double expansion = (actTemp-refTemp)*expansionCoefficient;
    protExpansion = QString::number(expansion,'f',6);
    double m = 1.0/(1+ (expansion));

    OiMat eMat(4,4);
    eMat.setAt(0,0,1.0);
    eMat.setAt(1,1,1.0);
    eMat.setAt(2,2,1.0);
    eMat.setAt(3,3,1.0);

    OiMat scale(4,4);
    scale.setAt(0,0,m);
    scale.setAt(1,1,m);
    scale.setAt(2,2,m);
    scale.setAt(3,3,1.0);

    tp.setTransformationParameters(eMat, eMat, scale);
}*/

/*!
 * \brief SimpleTemperatureCompensation::SimpleTemperatureCompensation
 * \param act
 * \param ref
 * \param material
 */
SimpleTemperatureCompensation::SimpleTemperatureCompensation(double act, double ref, QString material)
{
    this->actTemp = act;
    this->refTemp = ref;
    this->material = material;
}

/*!
 * \brief SimpleTemperatureCompensation::calcScaleFromTemperature
 * \return
 */
double SimpleTemperatureCompensation::calcScaleFromTemperature()
{
    double expansionCoeff = Materials::getExpansionCoefficient(this->material);
    double expansion = (this->actTemp-this->refTemp)*expansionCoeff;
    double scale = 1.0/(1+expansion);

    return scale;
}

