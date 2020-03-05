#ifndef P_BESTFTICIRCLEINPLANE_H
#define P_BESTFTICIRCLEINPLANE_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;
using namespace std;


struct IdPoint {
    int id;
    OiVec xyz;
};
inline bool operator==(const IdPoint& p1, const IdPoint& p2){
    return p1.id == p2.id;
}

/*!
 * \brief The BestFitCircleInPlane class
 */
class BestFitCircleInPlane : public FitFunction
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

#endif // P_BESTFTICIRCLEINPLANE_H
