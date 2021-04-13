#ifndef P_BESTFITCYLINDERFROMPOINTS_H
#define P_BESTFITCYLINDERFROMPOINTS_H

#include <QObject>
#include <QPointer>

#include "fitfunction.h"
#include "p_bestfitcylinder.h"

using namespace oi;

/*!
 * \brief The BestFitCylinderFromPoints class
 */
class BestFitCylinderFromPoints : public FitFunction, public BestFitCylinderUtil
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

    bool exec(Cylinder &cylinder) override;

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Cylinder &cylinder);

};

class BestFitCylinderFromPointsAppxDirection: public BestFitCylinderFromPoints
{
    Q_OBJECT

public:
    void init() override;

};

class BestFitCylinderFromPointsAppxDummyPoint: public BestFitCylinderFromPoints
{
    Q_OBJECT

public:
    void init() override;

};

#endif // P_BESTFITCYLINDERFROMPOINTS_H
