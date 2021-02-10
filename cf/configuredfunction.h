#ifndef CONFIGUREDFUNCTION_H
#define CONFIGUREDFUNCTION_H

#include <QObject>

#include "function.h"
#include "specialfunction.h"
#include "featurewrapper.h"
#include "types.h"
#include "treeutil.h"
#include "configuredfunctioncommon.h"
#include "configuredfunctionconfig.h"
#include "cfvisitor.h"

using namespace oi;

/**
 * @brief The ConfiguredFunction class
 */
class ConfiguredFunction : public SpecialFunction {
    friend class Feature;
    friend class CFVisitor;

    Q_OBJECT

public:
    ConfiguredFunction(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent = 0);
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

#endif // CONFIGUREDFUNCTION_H
