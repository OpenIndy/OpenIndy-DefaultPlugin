#include "cfvisitor2.h"

CFVisitor2::CFVisitor2(CFContext ctx): ctx(ctx) {

}

void CFVisitor2::pre(QPointer<Node> n, int index, int level) {
    CFCParameter *node = static_cast <CFCParameter*>(n.data());

    QString pre = QString("pre L%1 I%2").arg(level).arg(index);

    if(level == 0) { // level 0, first level

        CFFunctionData fd;
        fd.function = this->ctx.baseFunction;
        fd.feature = this->ctx.baseFeature;
        this->data.functionData.push(fd);

    } else if(ctx.config.isFunction(node->getName())) { // level 1-n, all other levels

        QPointer<Function> function = data.functionData.top().function;

        CFFunctionData fd;
        fd.function = this->ctx.getFunction(node->getName());
        QPointer<FeatureWrapper> feature = this->ctx.createApplicableFeature(fd.function->getApplicableFor());

        if(index == 0) {
            fd.feature = feature;

            debug(pre, QString("set feature: %2 to function: %1").arg(function->getMetaData().name).arg(fd.feature->getFeature()->getFeatureName()), level);
        } else {
            InputElement ie = ctx.createInputElement(feature);

            debug(pre, QString("add input element id: %2, %3, %4 to function: %1 (%5)").arg(function->getMetaData().name).arg(ie.id).arg(ie.typeOfElement).arg(ie.label).arg(index), level);
            function->addInputElement(ie, index);
        }

        this->data.functionData.push(fd);

    } else if(ctx.config.isNeededElement(node->getName())) {

        CFFunctionData fd = data.functionData.top();
        QPointer<Function> function = fd.function;

        InputElement ie = ctx.getInputElementsByName(node->getName());
        if(index == 0) {

            QPointer<FeatureWrapper> fw = new FeatureWrapper();

            switch (ie.typeOfElement) {
            case ePointElement: {
                fw->setPoint(ie.point);
                break;
            }
            default:
                throw logic_error(QString("not implemented: ElementTypes: %1").arg(ie.typeOfElement).toLocal8Bit().data());
            }

            fd.feature = fw;

            debug(pre, QString("set feature: %2 to function: %1").arg(function->getMetaData().name).arg(fd.feature->getFeature()->getFeatureName()), level);
        } else {
            debug(pre, QString("add input element id: %2, %3, %4 to function: %1 (%5)").arg(function->getMetaData().name).arg(ie.id).arg(ie.typeOfElement).arg(ie.label).arg(index), level);
            function->addInputElement(ie, index);
        }


    } else if(ctx.config.isHelperElement(node->getName())) {

        QPointer<Function> function = data.functionData.top().function;

        InputElement ie = ctx.getHelperInputElementsByName(node->getName());
        debug(pre, QString("add input element id: %2, %3, %4 to function: %1 (%5)").arg(function->getMetaData().name).arg(ie.id).arg(ie.typeOfElement).arg(ie.label).arg(index), level);
        function->addInputElement(ie, index);

    }

}

void CFVisitor2::post(QPointer<Node> n, int index, int level) {
    CFCParameter *node = static_cast <CFCParameter*>(n.data());

    QString post = QString("post L%1 I%2").arg(level).arg(index);

   if(ctx.config.isFunction(node->getName())) {

        CFFunctionData fd = data.functionData.pop();

        ConfiguredFunction2 *cf = qobject_cast<ConfiguredFunction2*>(fd.function);
        if(cf) {
            cf->global_inputElements = ctx.global_inputElements;
            cf->global_feature = ctx.global_feature;
        }

        debug(post, QString("exec featureData: %1").arg(fd.prettyPrint()), level);
        if(!fd.function->exec(fd.feature)) {
            throw logic_error(QString("execution failed for function: '%1'' with feature: '%2'").arg(fd.function->getMetaData().name).arg(fd.feature->getFeature()->getFeatureName()).toLocal8Bit().data());
        }

    }
}

