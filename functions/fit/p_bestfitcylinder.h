#ifndef P_BESTFITCYLINDER_H
#define P_BESTFITCYLINDER_H

#include <QObject>
#include <QPointer>

#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The BestFitCylinder class
 */
class BestFitCylinder : public FitFunction, public BestFitCylinderUtil
{  
    Q_OBJECT

public:

    //##############################
    //function initialization method
    //##############################

    void init() override;

protected:

    //############
    //exec methods
    //############

    bool exec(Cylinder &cylinder, const FunctionContext &ctx = FunctionContext{}) override;

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Cylinder &cylinder);

};

class BestFitCylinderAppxDirection: public BestFitCylinder
{
    Q_OBJECT

public:
    void init() override;

};

class BestFitCylinderAppxDummyPoint: public BestFitCylinder
{
    Q_OBJECT

public:
    void init() override;

};

#endif // P_BESTFITCYLINDER_H
