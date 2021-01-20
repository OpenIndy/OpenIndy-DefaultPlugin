#ifndef ZXPLANE_H
#define ZXPLANE_H

#include <QObject>
#include "constructfunction.h"

using namespace oi;

class ZXPlane: public ConstructFunction
{
    Q_OBJECT
public:
    void init() override;
    bool ZXPlane::exec(Plane &plane);

};

#endif // ZXPLANE_H
