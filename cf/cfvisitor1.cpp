#include "cfvisitor1.h"
#include "featurewrapper.h"
#include "function.h"

using namespace oi;

CFVisitor1::CFVisitor1(CFContext ctx, CFData data): ctx(ctx), data(data) {}

void CFVisitor1::pre(QPointer<Node> node, int index, int level) {
    QString pre = QString("pre L%1 I%2").arg(level).arg(index);
    if(ctx.config.isNeededElement(node->getName())) {
        QPointer<Function> function = data.functionData.top().function;
        InputElement ie = ctx.getInputElementsByName(node->getName());
        debug(pre, QString("add input element id: %2 (%3) to function: %1 (%4)").arg(function->getMetaData().name).arg(ie.id).arg(ie.label).arg(index), level);
        function->addInputElement(ie, index);
    } else if(ctx.config.isHelperElement(node->getName())) {
        QPointer<Function> function = data.functionData.top().function;
        InputElement ie = ctx.getHelperInputElementsByName(node->getName());
        debug(pre, QString("add helper input element id: %2 (%3) to function: %1 (%4)").arg(function->getMetaData().name).arg(ie.id).arg(ie.label).arg(index), level);
        function->addInputElement(ie, index);
    } else if(ctx.config.isFunction(node->getName())) {
        QPointer<Function> function = ctx.getFunction(node->getName());
        QPointer<FeatureWrapper> feat = data.featurePerLevel.value(level);
        if(feat.isNull()){
            feat = ctx.createApplicableFeature(function->getApplicableFor());
        }
        data.featurePerLevel.insert(level, feat);
        debug(pre, QString("append function: %1 to featureData: %2").arg(function->getMetaData().name).arg(data.functionData.top().prettyPrint()), level);
        data.functionData.top().featureFunctions.append(function);

        CFFunctionData d;
        d.level = level;
        d.function = function;
        d.feature = feat;

        debug(pre, QString("push functionData: %1").arg(d.prettyPrint()), level);
        data.functionData.push(d);
    }
}

void CFVisitor1::post(QPointer<Node> node, int index, int level) {
    QString post = QString("post L%1 I%2").arg(level).arg(index);
    if(false && ctx.config.isNeededElement(node->getName())) {
        QPointer<Function> function = data.functionData.top().function;
        InputElement ie = ctx.getInputElementsByName(node->getName());
        debug(post, QString("add input element id: %2 (%3) to function: %1 (%4)").arg(function->getMetaData().name).arg(ie.id).arg(ie.label).arg(index), level);
        function->addInputElement(ie, index);
    } else if(false && ctx.config.isHelperElement(node->getName())) {
        QPointer<Function> function = data.functionData.top().function;
        InputElement ie = ctx.getHelperInputElementsByName(node->getName());
        debug(post, QString("add helper input element id: %2 (%3) to function: %1 (%4)").arg(function->getMetaData().name).arg(ie.id).arg(ie.label).arg(index), level);
        function->addInputElement(ie, index);
    } else if(ctx.config.isFunction(node->getName())) {
        {
            if(true) {
                CFFunctionData fd = data.functionData.pop();
                debug(post, QString("exec featureData: %1").arg(fd.prettyPrint()), level);

                if(!fd.featureFunctions.isEmpty()) {
                    if(!ctx.exec(fd.feature, fd.featureFunctions)) {
                        qDebug() << fd.function->getMetaData().name << ", " << fd.feature->getFeature()->getFeatureName();
                        throw logic_error("execution failed");
                    }

                    if(data.functionData.size() > 1) {
                        InputElement ie = ctx.getFeatureInputElementsByName(fd.feature);
                        debug(post, QString("add function input element id: %2 (%3) to function: %1 (%4)").arg(data.functionData.top().function->getMetaData().name).arg(ie.id).arg(ie.label).arg(index), level);
                        data.functionData.top().function->addInputElement(ie, index);
                    }
                }
            }
        }

    } else {
        // throw logic_error("is not a 'needed element', 'helper element', 'function'");
    }
}



