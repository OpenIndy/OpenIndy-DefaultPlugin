#ifndef FUNCTIONCONFIGPARSER_H
#define FUNCTIONCONFIGPARSER_H

#include "constructfunction.h"
#include "featurewrapper.h"
#include "types.h"
#include "treeutil.h"
#include "configuredfunctioncommon.h"
#include "configuredfunctionconfig.h"
#include "cfcparameter.h"

using namespace oi;

class FunctionConfigParser
{

public:
    QList<ConfiguredFunctionConfig> readConfigFromJson(QString filename);

private:
    QList<QPointer<Node> > parameter(QJsonValue value);

    QMultiMap<int, InputElementMapping> inputElementsMapping(ConfiguredFunctionConfig config, QJsonValue value);

    QList<QString> functionNames(QJsonValue value);

    ElementTypes typeOfElement(QJsonValue value);

    QList<QPair<QString, NeededElement> > neededElements(QJsonValue value);

    QList<FeatureTypes> applicableFor(QJsonValue value);
};

#endif // FUNCTIONCONFIGPARSER_H
