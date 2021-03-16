#ifndef CFCONTEXT_H
#define CFCONTEXT_H

#include <QString>
#include <QPointer>
#include <QList>
#include <QMultiMap>

#include "featurewrapper.h"
#include "function.h"
#include "types.h"
#include "configuredfunction.h"

using namespace oi;

class CFContext
{
public:

    CFContext();

    /**
     * @brief getFunction
     * @param name
     * @return function instance and removes function from list
     */
    QPointer<Function> getFunction(QString name);

    InputElement getInputElement(QString name);

    /**
     * @brief createApplicableFeature
     * @param featureType
     * @return feature for element of featuresTypes
     */
    QPointer<FeatureWrapper> createApplicableFeature(FeatureTypes featureType);
    InputElement createInputElement(QPointer<FeatureWrapper> feature);

public:
    ConfiguredFunctionConfig config;
    QMap<int, QList<InputElement> > inputElements;
    // all necessary funktions
    QList<QPointer<Function> > functions;

    QPointer<Function> baseFunction;
    QPointer<FeatureWrapper> baseFeature;

    QMap<int, QList<InputElement> > global_inputElements;
    QPointer<FeatureWrapper> global_feature;
};

#endif // CFCONTEXT_H
