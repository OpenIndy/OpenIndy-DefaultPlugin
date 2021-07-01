#ifndef P_CHANGERADIUS_H
#define P_CHANGERADIUS_H

#include <QObject>
#include <QPointer>

#include "objecttransformation.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The ChangeRadius class
 */
class ChangeRadius : public ObjectTransformation
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

    bool exec(Sphere &sphere, const FunctionContext &ctx = FunctionContext{});
    bool exec(Circle &circle, const FunctionContext &ctx = FunctionContext{});
    bool exec(Cylinder &cylinder, const FunctionContext &ctx = FunctionContext{});

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Sphere &sphere);
    bool setUpResult(Circle &circle);
    bool setUpResult(Cylinder &cylinder);

};

#endif // P_CHANGERADIUS_H
