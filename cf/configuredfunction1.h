#ifndef CONFIGUREDFUNCTION1_H
#define CONFIGUREDFUNCTION1_H

#include <QObject>
#include <QString>
#include <QPointer>
#include <QList>
#include <QMultiMap>

#include "function.h"
#include "constructfunction.h"
#include "featurewrapper.h"
#include "types.h"
#include "treeutil.h"
#include "configuredfunctioncommon.h"
#include "configuredfunctionconfig.h"

class ConfiguredFunction1 : public Function
{
    friend class Feature;
    Q_OBJECT
public:
    ConfiguredFunction1(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent = 0);
    void init() override;
    bool exec(const QPointer<FeatureWrapper> &feature) override;


private:
    ConfiguredFunctionConfig config;

    QList<QPointer<Function> > functions;
};

#endif // CONFIGUREDFUNCTION1_H
