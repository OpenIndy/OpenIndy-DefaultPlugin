#ifndef P_TRANSLATEBYVALUE_H
#define P_TRANSLATEBYVALUE_H

#include <QObject>
#include <QPointer>

#include "objecttransformation.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

class TranslateByValue : public ObjectTransformation
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

    bool exec(Point &point, const FunctionContext &ctx = FunctionContext{});
    bool exec(Line &line, const FunctionContext &ctx = FunctionContext{});
    bool exec(Plane &plane, const FunctionContext &ctx = FunctionContext{});
    bool exec(Sphere &sphere, const FunctionContext &ctx = FunctionContext{});

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Point &point);
    bool setUpResult(Line &line);
    bool setUpResult(Plane &plane);
    bool setUpResult(Sphere &sphere);

};

#endif // P_TRANSLATEBYVALUE_H
