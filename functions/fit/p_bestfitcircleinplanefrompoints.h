#ifndef P_BESTFTICIRCLEINPLANEFROMPOINTS_H
#define P_BESTFTICIRCLEINPLANEFROMPOINTS_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;
using namespace std;


/*!
 * \brief The BestFitCircleInPlaneFromPoints class
 */
class BestFitCircleInPlaneFromPoints : public FitFunction
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

    bool exec(Circle &circle);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Circle &circle);

};

#endif // P_BESTFTICIRCLEINPLANEFROMPOINTS_H
