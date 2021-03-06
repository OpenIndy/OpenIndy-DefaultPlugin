#include <QList>
#include <stdexcept>

#include "cfvisitor.h"
#include "configuredfunctionconfig.h"
#include "configuredfunction.h"
#include "cfcparameter.h"
#include "cfutil.h"
#include "cffunctiondata.h"


CFVisitor::CFVisitor(CFContext ctx): ctx(ctx) {

}

void CFVisitor::pre(QPointer<Node> n, int index, int level) {
    CFCParameter *node = static_cast <CFCParameter*>(n.data());

    QString pre = QString("pre L%1 I%2").arg(level).arg(index);

    if(level == 0) { // level 0, first level

        CFFunctionData fd;
        fd.function = this->ctx.baseFunction;
        fd.feature = this->ctx.baseFeature; // == ctx.global_feature
        this->data.push(fd);

    } else if(ctx.config.isFunction(node->getName())) { // level 1-n, all other levels

        QPointer<Function> parentFunction = data.top().function;

        CFFunctionData fd;
        fd.function = this->ctx.getFunction(node->getName());

        // set feature for function
        if(node->getFeature().compare("global") == 0) {
            fd.feature = ctx.global_feature;
            debug(pre, QString("set global feature: %2 to function: %1").arg(fd.function->getMetaData().name).arg(fd.feature->getFeature()->getFeatureName()), level);
        } else if(node->getFeature().compare("global.copy") == 0) {
            QPointer<FeatureWrapper> copy = new FeatureWrapper();
            switch (ctx.global_feature->getFeatureTypeEnum()) {
            case ePointFeature: {
                QPointer<Point> src = ctx.global_feature->getPoint();
                QPointer<Point> dest = new Point(false, src->getPosition());
                copy->setPoint(dest);
                break;
            }
            default:
                throw std::logic_error(QString("no FeatureTypes for \"%1\" found").arg(ctx.global_feature->getFeatureTypeString()).toLocal8Bit().data());
                break;
            }
            fd.feature = copy;

            debug(pre, QString("set global.copy feature: %2 to function: %1").arg(fd.function->getMetaData().name).arg(fd.feature->getFeature()->getFeatureName()), level);
        } else {
            QPointer<FeatureWrapper> feature = this->ctx.createApplicableFeature(CFUtil::featureTypesForName(node->getFeature()));
            fd.feature = feature;
            debug(pre, QString("set feature: %2 to function: %1").arg(fd.function->getMetaData().name).arg(fd.feature->getFeature()->getFeatureName()), level);
        }

        // add input elements to parent, but not for level 1 or lower, because that overwrites you base input elements
        if(level > 1){
            InputElement ie = ctx.createInputElement(fd.feature);

            debug(pre, QString("add input element id: %2, %3, %4 to function: %1 (%5)").arg(parentFunction->getMetaData().name).arg(ie.id).arg(ie.typeOfElement).arg(ie.label).arg(index), level);
            parentFunction->addInputElement(ie, index);
        }

        this->data.push(fd);

    } else if(ctx.config.isNeededElement(node->getName())) {

        CFFunctionData parentFd = data.top();
        QPointer<Function> parentFunction = parentFd.function;

        InputElement ie = ctx.getInputElement(node->getName());

        debug(pre, QString("add input element id: %2, %3, %4 to function: %1 (%5)").arg(parentFunction->getMetaData().name).arg(ie.id).arg(ie.typeOfElement).arg(ie.label).arg(index), level);
        parentFunction->addInputElement(ie, index);
    }

}

void CFVisitor::post(QPointer<Node> n, int index, int level) {
    CFCParameter *node = static_cast <CFCParameter*>(n.data());

    QString post = QString("post L%1 I%2").arg(level).arg(index);

   if ( ctx.config.isFunction(node->getName())
           && level > 0 /* skip level 0 because this is the base function */) {

        CFFunctionData fd = data.pop();

        ConfiguredFunction *cf = qobject_cast<ConfiguredFunction*>(fd.function); // inner ConfiguredFunction
        if(cf) {
            cf->global_inputElements = ctx.global_inputElements;
            // maybe later there is a need for "global.global"
            // cf->global_feature = ctx.global_feature;
        }

        debug(post, QString("exec featureData: %1").arg(fd.prettyPrint()), level);
        if(!fd.function->exec(fd.feature)) {
            throw std::logic_error(QString("execution failed for function: '%1'' with feature: '%2'").arg(fd.function->getMetaData().name).arg(fd.feature->getFeature()->getFeatureName()).toLocal8Bit().data());
        }

    }
}

