#ifndef DISTANCEBETWEENTWOPOINTS_H
#define DISTANCEBETWEENTWOPOINTS_H

#include "constructfunction.h"

using namespace oi;

class DistanceBetweenTwoPoints : public ConstructFunction
{
    Q_OBJECT
public:
    void init() override;

    bool exec(ScalarEntityDistance &distance, const FunctionContext &ctx = FunctionContext{}) override;

};

#endif // DISTANCEBETWEENTWOPOINTS_H
