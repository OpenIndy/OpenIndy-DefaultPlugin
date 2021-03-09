#ifndef CONFIGUREDFUNCTIONCONFIG_H
#define CONFIGUREDFUNCTIONCONFIG_H

#include "function.h"
#include "constructfunction.h"
#include "featurewrapper.h"
#include "types.h"
#include "treeutil.h"


using namespace oi;

struct InputElementMapping {
    int functionIndex;
    int srcInputElementIndex;
    int dstInputElementIndex;
};


class ConfiguredFunctionConfig {

public:
    ConfiguredFunctionConfig();

public:
    bool isNeededElement(QString name);
    bool isFunction(QString name);
    /**
     * @brief getFunctionNames
     * @return all necessary funktions for this config
     */
    QList<QString> getFunctionNames();
    void collectFunctionNames(QList<QPointer<Node> > &parameter, QList<QString> &names);
    QList<NeededElement> getNeededElements();
    QList<QString> getNeededElementNames();

public:
    int version;
    QString name;
    QList<QPair<QString, NeededElement> > neededElements;
    QList<FeatureTypes> applicableFor;

    // versionn 1
    QList<QString> functionNames;
    QMultiMap<int, InputElementMapping> inputElementsMapping;

    // version 2
    QPointer<Node> parameter;
};

#endif // CONFIGUREDFUNCTIONCONFIG_H

