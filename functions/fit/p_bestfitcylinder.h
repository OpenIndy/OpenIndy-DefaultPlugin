#ifndef P_BESTFITCYLINDER_H
#define P_BESTFITCYLINDER_H

#include <QObject>
#include <QPointer>
#include <QtMath>
#include <string>
#include <limits>
#include <math.h>

#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

class CylinderApproximation{
public:
    double approxRadius;
    double approxXm;
    double approxYm;
    double approxAlpha;
    double approxBeta;
};

/*!
 * \brief The BestFitCylinder class
 */
class BestFitCylinder : public FitFunction
{  
    Q_OBJECT

public:

    //##############################
    //function initialization method
    //##############################

    void init();

protected:

    //############
    //exec methods
    //############

    bool exec(Cylinder &cylinder);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Cylinder &cylinder);

    bool approximateCylinder(Cylinder &cylinder, const QList<QPointer<Observation> > &inputObservations);
    bool fitCylinder(Cylinder &cylinder, const QList<QPointer<Observation> > &inputObservations, const CylinderApproximation &approximation);

    double getCorrespondingCos(double a);
    double getCorrespondingSin(double a);
    bool compareAngles(double a, double b);

    //################################
    //approximated cylinder parameters
    //################################

    QList<CylinderApproximation> approximations;

};

#endif // P_BESTFITCYLINDER_H
