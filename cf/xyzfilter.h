#ifndef XYZFILTER_H
#define XYZFILTER_H

#include <QObject>

#include "constructfunction.h"
#include "featurewrapper.h"
#include "oivec.h"

using namespace oi;

/**
 * @brief set axis to 0 and keeps others
 */
class XYZFilter : public ConstructFunction
{
    Q_OBJECT
public:
    XYZFilter(QString name, bool removeX, bool removeY, bool removeZ, QObject *parent = 0);

    void init() override;

    bool exec(Point &point) override;

private:

    bool removeX;
    bool removeY;
    bool removeZ;
};

#endif // XYZFILTER_H
