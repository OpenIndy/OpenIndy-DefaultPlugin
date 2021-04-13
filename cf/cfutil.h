#ifndef CFUTIL_H
#define CFUTIL_H

#include <QString>
#include "types.h"

using namespace oi;

class CFUtil
{
public:
    CFUtil();

    static FeatureTypes featureTypesForName(QString name);
};

#endif
