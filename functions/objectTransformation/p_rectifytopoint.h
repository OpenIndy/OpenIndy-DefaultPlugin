#ifndef P_RECTIFYTOPOINT_H
#define P_RECTIFYTOPOINT_H

#include <QObject>
#include <QPointer>

#include "objecttransformation.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The RectifyToPoint class
 */
class RectifyToPoint : public ObjectTransformation
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
    bool exec(Circle &circle, const FunctionContext &ctx = FunctionContext{});
    bool exec(Line &line, const FunctionContext &ctx = FunctionContext{});
    bool exec(Cylinder &cylinder, const FunctionContext &ctx = FunctionContext{});

private:

    //##############
    //helper methods
    //##############
    bool point(OiVec &point);
    bool setUpResult(Geometry &geometry);

};

#endif // P_RECTIFYTOPOINT_H
