#ifndef P_PLANEFROMPOINTS_H
#define P_PLANEFROMPOINTS_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "constructfunction.h"
#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The PlaneFromPoints class
 */
class PlaneFromPoints : public ConstructFunction, public BestFitPlaneUtil
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

    bool exec(Plane &plane);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Plane &plane);

};

#endif // P_PLANEFROMPOINTS_H
