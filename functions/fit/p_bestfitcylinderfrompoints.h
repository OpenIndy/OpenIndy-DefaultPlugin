#ifndef P_BESTFITCYLINDERFROMPOINTS_H
#define P_BESTFITCYLINDERFROMPOINTS_H

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
#include "p_bestfitcylinder.h"

using namespace oi;

/*!
 * \brief The BestFitCylinderFromPoints class
 */
class BestFitCylinderFromPoints : public BestFitCylinder
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

};

#endif // P_BESTFITCYLINDERFROMPOINTS_H
