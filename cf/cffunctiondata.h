#ifndef CFFUNCTIONDATA_H
#define CFFUNCTIONDATA_H

#include <QString>
#include "function.h"
#include "featurewrapper.h"

using namespace oi;


class CFFunctionData
{
public:
    CFFunctionData();

    QString prettyPrint();

public:
    QPointer<Function> function;

    // feature returned / modified by this function
    QPointer<FeatureWrapper> feature;

};

#endif // CFFUNCTIONDATA_H
