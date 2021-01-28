#include "configuredfunction2.h"

ConfiguredFunction2::ConfiguredFunction2(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent): Function(parent), functions(functions), config(config) {
}

void ConfiguredFunction2::init() {
    this->metaData.name = this->config.name;
    this->applicableFor = this->config.applicableFor;
    this->neededElements = config.getNeededElements();
}

bool ConfiguredFunction2::exec(const QPointer<FeatureWrapper> &feature) {
    try {
        ListVisitor visitors;

        QPointer<NodeVisitor> debug = new PrintVisitor();
        visitors.list.append(debug);

        QPointer<NodeVisitor> cf;

        if(true) {
            CFContext ctx;
            ctx.config = this->config;
            ctx.baseFunction = this;
            ctx.baseFeature = feature;
            ctx.functions = this->functions; // all necessary funktions

            ctx.global_inputElements = this->global_inputElements.isEmpty() ? this->getInputElements() :  this->global_inputElements;
            ctx.global_feature = this->global_feature.isNull() ? feature : this->global_feature;

            cf = new CFVisitor2(ctx);
        } else {
            CFFunctionData fd;
            fd.level = 0; // TODO necessary ???
            fd.feature = feature;
            fd.function = this;

            CFData data;
            data.functionData.push(fd);
            data.featurePerLevel.insert(fd.level, fd.feature); // TODO necessary ???

            CFContext ctx;
            ctx.config = config;
            ctx.inputElements = this->getInputElements();
            ctx.functions = this->functions; // all necessary funktions

            ctx.functions.append(this); // TODO necessary ???

            cf = new CFVisitor1(ctx, data);

        }

         visitors.list.append(cf);

        TreeUtil::traversal(this->config.parameter, visitors);

        return true;
    } catch(exception &e) {
        emit sendMessage(e.what(), MessageTypes::eErrorMessage, MessageDestinations::eConsoleMessage);
        return false;
    }
}

QString ConfiguredFunction2::prettyPrint() {
    QString s;
    s += "function: '" + this->getMetaData().name
        + "', featureFunctions: [ ";
    foreach(QPointer<Function> f, this->functions) {
        if(f->inherits("ConfiguredFunction2")) {
             s += "'" + qobject_cast<ConfiguredFunction2 *>(f)->prettyPrint() + "', ";
        } else {
            s += "'" + f->getMetaData().name + "', ";
        }
    }
    s += " ], ";
    return s;
}
