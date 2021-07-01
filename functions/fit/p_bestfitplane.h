#ifndef P_BESTFITPLANE_H
#define P_BESTFITPLANE_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The BestFitPlane class
 */
class BestFitPlane : public FitFunction
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

    bool exec(Plane &plane, const FunctionContext &ctx = FunctionContext{});

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Plane &plane);

};

#endif // P_BESTFITPLANE_H
