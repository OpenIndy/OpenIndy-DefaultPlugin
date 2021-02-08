#include "configuredfunction.h"

ConfiguredFunction::ConfiguredFunction(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent): SpecialFunction(parent), functions(functions), config(config) {
}

void ConfiguredFunction::init() {
    this->metaData.name = this->config.name;
    this->metaData.iid = SpecialFunction_iidd;

    this->applicableFor = this->config.applicableFor;
    this->neededElements = config.getNeededElements();
}

bool ConfiguredFunction::exec(const QPointer<FeatureWrapper> &feature) {
    try {
        ListVisitor visitors;

        QPointer<NodeVisitor> debug = new PrintVisitor();
        visitors.list.append(debug);

        CFContext ctx;
        ctx.config = this->config;
        ctx.baseFunction = this;
        ctx.baseFeature = feature;
        ctx.functions = this->functions; // all necessary funktions

        ctx.global_inputElements = this->global_inputElements.isEmpty() ? this->getInputElements() :  this->global_inputElements;
        ctx.global_feature = this->global_feature.isNull() ? feature : this->global_feature;


        QPointer<NodeVisitor> cf;
        cf = new CFVisitor(ctx);

        visitors.list.append(cf);

        TreeUtil::traversal(this->config.parameter, visitors);

        return true;
    } catch(exception &e) {
        emit sendMessage(e.what(), MessageTypes::eErrorMessage, MessageDestinations::eConsoleMessage);
        return false;
    }
}

QString ConfiguredFunction::prettyPrint() {
    QString s;
    s += "function: '" + this->getMetaData().name
        + "', featureFunctions: [ ";
    foreach(QPointer<Function> f, this->functions) {
        if(f->inherits("ConfiguredFunction")) {
             s += "'" + qobject_cast<ConfiguredFunction *>(f)->prettyPrint() + "', ";
        } else {
            s += "'" + f->getMetaData().name + "', ";
        }
    }
    s += " ], ";
    return s;
}
