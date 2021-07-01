#ifndef P_RECTIFYTOVECTOR_H
#define P_RECTIFYTOVECTOR_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "objecttransformation.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

enum Axis {
    eXAxis = 0,
    eYAxis,
    eZAxis // ijk
};

/*!
 * \brief The RectifyToVector class
 */
class RectifyToVector : public ObjectTransformation
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

    bool setUpResult(Geometry &plane);
    bool direction(OiVec &direction);
    Axis axis();

};

#endif // P_RECTIFYTOVECTOR_H
