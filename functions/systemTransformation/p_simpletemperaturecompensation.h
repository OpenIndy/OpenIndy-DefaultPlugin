#ifndef P_SIMPLETEMPERATURECOMPENSATION_H
#define P_SIMPLETEMPERATURECOMPENSATION_H

#include "systemtransformation.h"
#include "pluginmetadata.h"
#include "materials.h"

using namespace oi;

/*!
 * \brief The SimpleTemperatureCompensation class is a system transformation for temperature compensation.
 * By giving it a reference temperature, a actual temperature and a material it will calculate the compensation values.
 * They are equal for x, y and z component.
 */
class SimpleTemperatureCompensation : QObject
{
    Q_OBJECT

public:

    SimpleTemperatureCompensation(double act, double ref, QString material);

    double calcScaleFromTemperature();

protected:

    //############
    //exec methods
    //############

private:

    double refTemp;
    double actTemp;
    QString material;
};

#endif // P_SIMPLETEMPERATURECOMPENSATION_H
