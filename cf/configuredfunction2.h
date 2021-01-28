#ifndef CONFIGUREDFUNCTION2_H
#define CONFIGUREDFUNCTION2_H

#include <QObject>

#include "function.h"
#include "constructfunction.h"
#include "featurewrapper.h"
#include "types.h"
#include "treeutil.h"
#include "configuredfunctioncommon.h"
#include "configuredfunctionconfig.h"
#include "cfvisitor2.h"
#include "cfvisitor1.h"

using namespace oi;

class ConfiguredFunction2 : public Function {
    friend class Feature;
    friend class CFVisitor;

    Q_OBJECT

public:
    ConfiguredFunction2(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent = 0);
    void init() override;
    bool exec(const QPointer<FeatureWrapper> &feature) override;

    QString prettyPrint();

public:
    QMap<int, QList<InputElement> > global_inputElements;
    QPointer<FeatureWrapper> global_feature;

private:
    ConfiguredFunctionConfig config;
    // all necessary funktions
    QList<QPointer<Function> > functions;

};

#endif // CONFIGUREDFUNCTION2_H
