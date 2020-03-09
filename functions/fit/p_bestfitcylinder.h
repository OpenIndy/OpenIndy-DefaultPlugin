#ifndef P_BESTFITCYLINDER_H
#define P_BESTFITCYLINDER_H

#include <QObject>
#include <QPointer>
#include <QtMath>
#include <string>
#include <limits>
#include <math.h>
#include <random>

#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

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

protected:
    bool bestFitCylinder(Cylinder &cylinder, QList<IdPoint> points, QList<IdPoint> usablePoints);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Cylinder &cylinder);

    bool approximateCylinder(Cylinder &cylinder, const QList<IdPoint> &inputObservations, ApproximationTypes approximationType);
    bool approximateCylinder(OiVec pn, const QList<IdPoint> &inputObservations, QString label);
    bool fitCylinder(Cylinder &cylinder, const QList<IdPoint> &inputObservations, const QList<IdPoint> &allUsableObservations, const CylinderApproximation &approximation);

    double getCorrespondingCos(double a);
    double getCorrespondingSin(double a);
    bool compareAngles(double a, double b);

    //################################
    //approximated cylinder parameters
    //################################

    QList<CylinderApproximation> approximations;

};

#endif // P_BESTFITCYLINDER_H
