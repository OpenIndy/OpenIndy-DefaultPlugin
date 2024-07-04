#include "configuredfunction.h"
#include "types.h"
#include "treeutil.h"
#include "cfvisitor.h"

ConfiguredFunction::ConfiguredFunction(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent): SpecialFunction(parent), functions(functions), config(config) {
}

void ConfiguredFunction::init() {
    this->metaData.name = this->config.name;
    this->metaData.iid = SpecialFunction_iidd; // or differentiate?
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "";
    this->metaData.description = this->config.description;

    this->applicableFor = this->config.applicableFor;
    this->neededElements = config.getNeededElements();
}

bool ConfiguredFunction::exec(const QPointer<FeatureWrapper> &feature, const FunctionContext &ctx) {
    try {
        ListVisitor visitors;

        QPointer<NodeVisitor> debug = new PrintVisitor();
        visitors.list.append(debug);

        CFContext ctx; // contains references not copies !
        ctx.config = this->config;
        ctx.baseFunction = this;
        ctx.baseFeature = feature;
        ctx.functions = this->functions; // all necessary funktions

        for(auto function : ctx.functions) {
            function->clear(); // remove inner data e.g. inputElements
        }

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
