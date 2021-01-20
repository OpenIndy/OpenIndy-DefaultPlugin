#ifndef CONFIGUREDFUNCTION
#define CONFIGUREDFUNCTION

#include <QString>
#include <QPointer>
#include <QList>
#include <QMultiMap>

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

class CFCParameter: public Node {

public:
    CFCParameter(QString name, QString comment, QList<QPointer<Node> > subnodes, QObject *parent = 0): name(name), comment(comment), subnodes(subnodes) {

    }

    QList<QPointer<Node> > getSubnodes() override {
        return subnodes;
    }

    QString getName() override {
        return name;
    }

    QString prettyPrint() override {
        QString s;
        s += "{ name: '" + this->name + "', comment: '" + this->comment + "'";
        if(!this->subnodes.isEmpty()) {
            s += ", parameter: [ ";
            foreach(QPointer<Node> p, this->subnodes) {
                s += " " + p->prettyPrint();
            }
            s += " ], ";
        }
        s += "}, ";
        return s;
    }

private:
    QString name;
    QString comment;
    QList<QPointer<Node> > subnodes;

};


struct ConfiguredFunctionConfig {
    int version;
    QString name;
    QList<QPair<QString, NeededElement> > neededElements;
    QList<FeatureTypes> applicableFor;

    // versionn 1
    QList<QString> functionNames;
    QMultiMap<int, InputElementMapping> inputElementsMapping;

    // version 2
    QPointer<Node> parameter;

    bool isNeededElement(QString name) {
        for(int i=0; i<this->neededElements.size(); i++) {
            if(neededElements[i].first.compare(name) == 0) {
                return true;
            }
        }
        return false;
    }

    bool isHelperElement(QString name) {
        return name.compare("ZX") == 0;
    }

    bool isFunction(QString name) {
        return !isNeededElement(name) && !isHelperElement(name);
    }

    /**
     * @brief getFunctionNames
     * @return all necessary funktions for this config
     */
    QList<QString> getFunctionNames() {
        switch(version) {
        case 1:
            return functionNames;
        case 2:
            QList<QString> list;

            collectFunctionNames(parameter->getSubnodes(), list);
            return list;
        }

        throw logic_error("illegal version");

    }

    void collectFunctionNames(QList<QPointer<Node> > &parameter, QList<QString> &names) {
        foreach(QPointer<Node> p, parameter) {
            if(!isNeededElement(p->getName()) && !isHelperElement(p->getName())) {
                names.append(p->getName());
            }
            collectFunctionNames(p->getSubnodes(), names);
        }
    }

    QList<NeededElement> getNeededElements() {
        QList<NeededElement> list;
        for(int i=0; i<this->neededElements.size(); i++) {
            list.append(this->neededElements[i].second);
        }
        return list;
    }

    QList<QString> getNeededElementNames() {
        QList<QString> list;
        for(int i=0; i<this->neededElements.size(); i++) {
            list.append(this->neededElements[i].first);
        }
        return list;
    }

};

class DistanceBetweenTwoPoints : public ConstructFunction
{
    Q_OBJECT
public:
    void init() override {
        this->metaData.name = "DistanceBetweenTwoPoints";
        this->applicableFor.append(eScalarEntityDistanceFeature);
    }

   /* bool DistanceBetweenTwoPoints::exec(const QPointer<FeatureWrapper> &feature){
        if(!feature->getScalarEntityDistance().isNull()){
            return this->exec(*feature->getScalarEntityDistance());
        }
    }*/

    bool DistanceBetweenTwoPoints::exec(ScalarEntityDistance &distance)
    {
        if(this->inputElements[0].isEmpty()) {
            emit this->sendMessage("inputElements[0] is empty", MessageTypes::eWarningMessage);
            return false;
        }
        if(this->inputElements[1].isEmpty()) {
            emit this->sendMessage("inputElements[1] is empty", MessageTypes::eWarningMessage);
            return false;
        }
        OiVec p1 = this->inputElements[0].first().geometry->getPosition().getVector();
        OiVec p2 = this->inputElements[1].first().geometry->getPosition().getVector();
        OiVec v = p2-p1;
        double dot;
        OiVec::dot(dot, v, v);
        distance.setDistance(qSqrt(dot));

        return true;
    }

};

class ZXPlane: public ConstructFunction
{
    Q_OBJECT
public:
    void init() override {
        this->metaData.name = "ZXPlane";
        this->applicableFor.append(ePlaneFeature);
    }
    bool ZXPlane::exec(Plane &plane)
    {
        OiVec p(3);
        p.setAt(0, 0.);
        p.setAt(1, 0.);
        p.setAt(2, 0.);

        OiVec n(3);
        n.setAt(0, 0.);
        n.setAt(1, 1.);
        n.setAt(2, 0.);

        Position planePosition;
        planePosition.setVector(p);
        Direction planeDirection;
        planeDirection.setVector(n);
        plane.setPlane(planePosition, planeDirection);

        return true;
    }

};

class ConfiguredFunction : public Function {
    friend class Feature;
    Q_OBJECT

public:
    ConfiguredFunction(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent = 0): Function(parent), functions(functions), config(config) {
    }

    void init() override {
        this->metaData.name = this->config.name;
        this->applicableFor = this->config.applicableFor;
        this->neededElements = config.getNeededElements();
    }

    bool exec(const QPointer<FeatureWrapper> &feature) override {

        //execute all functions in the specified order
        for(int functionIndex = 0; functionIndex < this->functions.size(); functionIndex++){

            // break if the function pointer is not valid
            if(functions[functionIndex].isNull()){
                return false;
            }

            foreach(InputElementMapping mapping, this->config.inputElementsMapping.values(functionIndex)) {
                foreach(InputElement inputElement, this->inputElements.value(mapping.srcInputElementIndex)) {
                    functions[mapping.functionIndex]->addInputElement(inputElement, mapping.dstInputElementIndex);
                }
            }

            // try to solve the current function
            if(!functions[functionIndex]->exec(feature)){
                return false;
            }
        }

        return true;
    }


private:
    ConfiguredFunctionConfig config;

    QList<QPointer<Function> > functions;
};

struct CFContext {

     ConfiguredFunctionConfig config;
     QMap<int, QList<InputElement> > inputElements;
     // all necessary funktions
     QList<QPointer<Function> > functions;

     QMap<int, QList<InputElement> > global_inputElements;
     QPointer<Function> baseFunction;
     QPointer<FeatureWrapper> baseFeature;

     /**
      * @brief getFunction
      * @param name
      * @return function instance and removes function from list
      */
     QPointer<Function> getFunction(QString name) {
         for(int index = 0; index < functions.size(); index++) {
             if(!functions[index].isNull() && functions[index]->getMetaData().name.compare(name) == 0) {
                 QPointer<Function> f = functions.takeAt(index); // remove from list
                 return f;
             }
         }

         throw logic_error(QString("no function found: %1").arg(name).toLocal8Bit().data());
     }

     InputElement getInputElementsByName(QString name) {
         QList<InputElement> ies = this->global_inputElements.value(config.getNeededElementNames().indexOf(name));
         if(ies.size() == 0) {
             throw logic_error(QString("invalid count (%1) of InputElements: %2").arg(ies.size()).arg(name).toLocal8Bit().data());
         }
         return ies.first();
     }

     InputElement getHelperInputElementsByName(QString name) {
         if(name.compare("ZX") == 0) {
             double x = 0., y = 0., z = 0., i = 0., j = 1., k=0.;

             OiVec * p = new OiVec(4);
             p->setAt(0, x);
             p->setAt(1, y);
             p->setAt(2, z);
             p->setAt(3, 1.0);
             Position * xyz = new Position(*p);

             OiVec * a = new OiVec(4);
             a->setAt(0, i);
             a->setAt(1, j);
             a->setAt(2, k);
             a->setAt(3, 1.0);
             Direction * ijk = new Direction(*a);

             QPointer<Plane> feature = new Plane(false, *xyz, *ijk);
             feature->setIsSolved(true);
             feature->setFeatureName(name);

             InputElement element;
             element.label = name;
             element.typeOfElement = ePlaneElement;
             element.plane = feature;
             element.geometry = feature;
             return element;
         }

         throw logic_error("no helper input element found");
     }

     InputElement getFeatureInputElementsByName(QPointer<FeatureWrapper> feat) {
         switch(feat->getFeatureTypeEnum()) {
         case ePointFeature: {
             InputElement ie;
             ie.label = QString("%1").arg("point");
             ie.typeOfElement = ePointElement;
             ie.point = feat->getPoint();
             ie.geometry = feat->getGeometry();

             return ie;
         }
         }

         throw logic_error(QString("not implemented: FeatureTypeEnum: %1").arg(feat->getFeatureTypeString()).toLocal8Bit().data());

     }

     QPointer<FeatureWrapper> _createApplicableFeature(QList<FeatureTypes> fts) {
         QPointer<FeatureWrapper> fw = new FeatureWrapper();

         FeatureTypes ft = fts.first();
         switch (ft) {
         case ePointFeature: {
             QPointer<Point> point = new Point(false);
             fw->setPoint(point);
             break;
         }
         default:
             throw logic_error(QString("not implemented: FeatureTypes: %1").arg(ft).toLocal8Bit().data());
         }

         return fw;
     }

     bool exec(QPointer<FeatureWrapper> feature, QList<QPointer<Function> > functions) {
         foreach(QPointer<Function> func, functions){
             // break if the function pointer is not valid
             if(func.isNull()){
                 return false;
             }
             if(!func->getApplicableFor().contains(feature->getFeatureTypeEnum())) {
                 throw logic_error(QString("function '%1' is not applicable for feature type '%2'").arg(func->getMetaData().name).arg(feature->getFeatureTypeString()).toLocal8Bit().data());
             }

             int depth = 0;
             qDebug() << QString("%1exec function: %2 with feature: %3").arg(QString(depth*2, QChar(' '))).arg(func->getMetaData().name).arg(feature->getFeature()->getFeatureName());
             // try to solve the current function
             if(!func->exec(feature)){
                 qDebug() << "false";
                 return false;
             }
         }

         return true;
     }

     /**
      * @brief createApplicableFeature
      * @param featureTypes
      * @return feature for first element of featuresTypes
      */
     QPointer<FeatureWrapper> createApplicableFeature(QList<FeatureTypes> featureTypes) {
            QPointer<FeatureWrapper> featureWrapper = new FeatureWrapper();

            QList<FeatureTypes>::iterator featureType;
            for (featureType = featureTypes.begin(); featureType != featureTypes.end(); ++featureType) {
                switch(*featureType){
                    case ePointFeature: {
                        QPointer<Point> feature = new Point(false);
                        feature->setFeatureName("generated Point");
                        featureWrapper->setPoint(feature);
                        return featureWrapper;
                    }
                    case eScalarEntityDistanceFeature: {
                        QPointer<ScalarEntityDistance> feature = new ScalarEntityDistance(false);
                        feature->setFeatureName("generated ScalarEntityDistance");
                        featureWrapper->setScalarEntityDistance(feature);
                        return featureWrapper;
                    }
                }
            }

            throw logic_error(QString("can not create applicable feature").toLocal8Bit().data());
     }

     InputElement createInputElement(QPointer<FeatureWrapper> feature) {
        InputElement element;
        element.label = QString("generated InputElement: %1").arg(feature->getFeatureTypeString());

        switch (feature->getFeatureTypeEnum()) {
        case ePointFeature:
            element.typeOfElement = ePointElement;
            element.point = feature->getPoint();
            element.geometry = feature->getPoint();
            break;
        case eScalarEntityDistanceFeature:
            element.typeOfElement = eScalarEntityDistanceElement;
            element.scalarEntityDistance = feature->getScalarEntityDistance();
            element.geometry = feature->getScalarEntityDistance();
            break;
        default:
            throw logic_error(QString("can not create input element for feature type: %1").arg(feature->getFeatureTypeString()).toLocal8Bit().data());
        }
        return element;
     }
};

/**
 *
 * @brief The CFFunctionData struct
 */
struct CFFunctionData {
    int level;
    // current (this) function
    QPointer<Function> function;
    // feature returned by this function
    QPointer<FeatureWrapper> feature;
    QList<QPointer<Function> > featureFunctions;

    QString prettyPrint() {
        QString s;
        s += "function: '" + (this->function.isNull() ? "null" : this->function->getMetaData().name)
            + "', feature: '" + (this->feature.isNull() ? "null" : this->feature->getFeature()->getFeatureName() + "', featureType: '" + this->feature->getFeatureTypeString())
            + "', featureFunctions: [ ";
        foreach(QPointer<Function> f, this->featureFunctions) {
            s += "'" + f->getMetaData().name + "', ";
        }
        s += " ], ";
        return s;
    }

};

struct CFData {
    QStack<CFFunctionData> functionData;
    QStack<QPointer<FeatureWrapper> > baseFeature;// TODO necessary ???
    QMap<int, QPointer<FeatureWrapper> > featurePerLevel;// TODO necessary ???

    QPointer<FeatureWrapper> findFeatureForLevel(int level) { // TODO necessary ???
        for(int i=functionData.size()-1; i>=0; i--) {
            if(functionData[i].level == level) {
                return functionData[i].feature;
            }
        }
        return 0;
    }
};

class CFVisitor: public NodeVisitor {

public:
    CFVisitor(CFContext ctx, CFData data): ctx(ctx), data(data) {}

    void pre(QPointer<Node> node, int index, int level) override {
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

    void post(QPointer<Node> node, int index, int level) override {
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

private:
    CFContext ctx;
    CFData data;
};


class CFVisitor2: public NodeVisitor {

public:
    CFVisitor2(CFContext ctx): ctx(ctx) {}

    void pre(QPointer<Node> node, int index, int level) override {
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

    void post(QPointer<Node> node, int index, int level) override {
        QString post = QString("post L%1 I%2").arg(level).arg(index);

       if(ctx.config.isFunction(node->getName())) {

            CFFunctionData fd = data.functionData.pop();
            debug(post, QString("exec featureData: %1").arg(fd.prettyPrint()), level);
            fd.function->setProperty("OI_CF_GLOBAL_INPUTELEMENTS", ctx.baseFunction->property("OI_CF_GLOBAL_INPUTELEMENTS"));
            if(!fd.function->exec(fd.feature)) {
                throw logic_error(QString("execution failed for function: '%1'' with feature: '%2'").arg(fd.function->getMetaData().name).arg(fd.feature->getFeature()->getFeatureName()).toLocal8Bit().data());
            }

        }
    }

private:
    CFContext ctx;
    CFData data;
};
class CFHelperFunction: public Function {

public:
    void init() override {
        this->metaData.name = "CFHelperFunction";
    }

    bool exec(const QPointer<FeatureWrapper> &feature) override {
        //feature = this->inputElements[0].first().
        return true;
    }
};

class ConfiguredFunction2 : public Function {
    friend class Feature;
    friend class CFVisitor;

    Q_OBJECT

public:
    ConfiguredFunction2(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent = 0): Function(parent), functions(functions), config(config) {
    }

    void init() override {
        this->metaData.name = this->config.name;
        this->applicableFor = this->config.applicableFor;
        this->neededElements = config.getNeededElements();
    }

    bool exec(const QPointer<FeatureWrapper> &feature) override {
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
                ctx.global_inputElements = this->getInputElements();

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

                cf = new CFVisitor(ctx, data);

            }

             visitors.list.append(cf);

            TreeUtil::traversal(this->config.parameter, visitors);

            return true;
        } catch(exception &e) {
            emit sendMessage(e.what(), MessageTypes::eErrorMessage, MessageDestinations::eConsoleMessage);
            return false;
        }
    }

    QString prettyPrint() {
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

public:
    QMap<int, QList<InputElement> > global_inputElements;

private:
    ConfiguredFunctionConfig config;
    // all necessary funktions
    QList<QPointer<Function> > functions;

};


class FunctionConfigParser {

public:
    QList<ConfiguredFunctionConfig> readConfigFromJson(QString filename) {

        QList<ConfiguredFunctionConfig> configs;

        QString val;
        QFile file;
        file.setFileName(QDir::currentPath() + "/" + filename);
        if(!file.exists()) {
            return configs;
        }
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        val = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject json = doc.object();

        switch(json["version"].toInt()) {
        case 1: {
            foreach(QJsonValue function, json["functions"].toArray()) {
                // qDebug() << function;

                ConfiguredFunctionConfig config;
                config.version = 1;
                QJsonObject object = function.toObject();
                config.name = object["name"].toString();
                config.applicableFor = applicableFor(object["applicableFor"]);

                config.neededElements = neededElements(object["neededElements"]);

                config.functionNames = functionNames(object["innerFunctions"]);

                config.inputElementsMapping = inputElementsMapping(config, object["inputElementMappings"]);

                configs.append(config);
            }

        } case 2: {
             foreach(QJsonValue function, json["functions"].toArray()) {
                ConfiguredFunctionConfig config;
                config.version = 2;
                QJsonObject object = function.toObject();
                config.name = object["name"].toString();
                config.applicableFor = applicableFor(object["applicableFor"]);

                config.neededElements = neededElements(object["neededElements"]);

                QPointer<Node> node = new CFCParameter(config.name, "configured function", parameter(object["parameter"]) /* parameter tree */);
                config.parameter = node;

                configs.append(config);
             }

        }

        } // switch



        return configs;
    }

private:
    /**
     * @brief parameter
     * @param value
     * @return tree with parameter CFCParameter and sub nodes (CFCParameter)
     */
    QList<QPointer<Node> > parameter(QJsonValue value) {
        QList<QPointer<Node> > list;
        foreach(QJsonValue element, value.toArray()) {
            QJsonObject object = element.toObject();

            QPointer<Node> p = new CFCParameter(
                object["name"].toString(),
                object["comment"].toString(),
                parameter(object["parameter"]));

            list.append(p);
        }

        return list;
    }

    QMultiMap<int, InputElementMapping> inputElementsMapping(ConfiguredFunctionConfig config, QJsonValue value) {
        if(value.isNull() || !value.isArray()) {
            throw logic_error("can not parse json");
        }

        QMultiMap<int, InputElementMapping> inputElementsMapping;
        foreach(QJsonValue element, value.toArray()) {
            QJsonObject object = element.toObject();

            InputElementMapping mapping;
            mapping.functionIndex = -1;
            mapping.srcInputElementIndex = -1;
            mapping.dstInputElementIndex = -1;

            QString fromNeededElementName = object["from"].toObject()["neededElement"].toString();
            for(int i=0; i < config.neededElements.size(); i++) {
                if(config.neededElements[i].first.compare(fromNeededElementName) == 0) {
                    mapping.srcInputElementIndex = i;
                    break;
                }
            }

            QString toFunctionName = object["to"].toObject()["function"].toString();
            mapping.functionIndex = config.functionNames.indexOf(toFunctionName);

            mapping.dstInputElementIndex = object["to"].toObject()["index"].toInt();


            inputElementsMapping.insert(mapping.functionIndex, mapping);

        }

        return inputElementsMapping;
    }

    QList<QString> functionNames(QJsonValue value) {
        if(value.isNull() || !value.isArray()) {
            throw logic_error("can not parse json");
        }
        QList<QString> list;
        foreach(QJsonValue element, value.toArray()) {
            QJsonObject object = element.toObject();
            list.append(object["name"].toString());
        }
        return list;
    }

    ElementTypes typeOfElement(QJsonValue value) {
        if(value.isNull()) {
            throw logic_error("can not parse json");
        }
        QString s = value.toString();
        if (s.compare("Circle") == 0 ) {
            return eCircleElement;
        } else if (s.compare("Cone") == 0 ) {
            return eConeElement;
        } else if (s.compare("Cylinder") == 0 ) {
            return eCylinderElement;
        } else if (s.compare("Ellipse") == 0 ) {
            return eEllipseElement;
        } else if (s.compare("Ellipsoid") == 0 ) {
            return eEllipsoidElement;
        } else if (s.compare("Hyperboloid") == 0 ) {
            return eHyperboloidElement;
        } else if (s.compare("Line") == 0 ) {
            return eLineElement;
        } else if (s.compare("Nurbs") == 0 ) {
            return eNurbsElement;
        } else if (s.compare("Paraboloid") == 0 ) {
            return eParaboloidElement;
        } else if (s.compare("Plane") == 0 ) {
            return ePlaneElement;
        } else if (s.compare("Point") == 0 ) {
            return ePointElement;
        } else if (s.compare("PointCloud") == 0 ) {
            return ePointCloudElement;
        } else if (s.compare("ScalarEntityAngle") == 0 ) {
            return eScalarEntityAngleElement;
        } else if (s.compare("ScalarEntityDistance") == 0 ) {
            return eScalarEntityDistanceElement;
        } else if (s.compare("ScalarEntityMeasurementSeries") == 0 ) {
            return eScalarEntityMeasurementSeriesElement;
        } else if (s.compare("ScalarEntityTemperature") == 0 ) {
            return eScalarEntityTemperatureElement;
        } else if (s.compare("SlottedHole") == 0 ) {
            return eSlottedHoleElement;
        } else if (s.compare("Sphere") == 0 ) {
            return eSphereElement;
        } else if (s.compare("Torus") == 0 ) {
            return eTorusElement;
        } else if (s.compare("Direction") == 0 ) {
            return eDirectionElement;
        } else if (s.compare("Position") == 0 ) {
            return ePositionElement;
        } else if (s.compare("Radius") == 0 ) {
            return eRadiusElement;
        } else if (s.compare("CoordinateSystem") == 0 ) {
            return eCoordinateSystemElement;
        } else if (s.compare("Station") == 0 ) {
            return eStationElement;
        } else if (s.compare("TrafoParam") == 0 ) {
            return eTrafoParamElement;
        } else if (s.compare("Observation") == 0 ) {
            return eObservationElement;
        } else if (s.compare("ReadingCartesian") == 0 ) {
            return eReadingCartesianElement;
        } else if (s.compare("ReadingPolar") == 0 ) {
            return eReadingPolarElement;
        } else if (s.compare("ReadingDistance") == 0 ) {
            return eReadingDistanceElement;
        } else if (s.compare("ReadingDirection") == 0 ) {
            return eReadingDirectionElement;
        } else if (s.compare("ReadingTemperature") == 0 ) {
            return eReadingTemperatureElement;
        } else if (s.compare("ReadingLevel") == 0 ) {
            return eReadingLevelElement;
        } else {
            return eUndefinedElement;
        }
    }

    QList<QPair<QString, NeededElement> > neededElements(QJsonValue value) {
        if(value.isNull() || !value.isArray()) {
            throw logic_error("can not parse json");
        }
        QList<QPair<QString, NeededElement> > list;
        foreach(QJsonValue element, value.toArray()) {
            QJsonObject object = element.toObject();

            NeededElement neededElement;
            neededElement.description = object["description"].toString();
            neededElement.infinite = object["infinite"].toBool();
            neededElement.typeOfElement = typeOfElement(object["typeOfElement"]);
            QPair<QString, NeededElement> ne;
            ne.first = object["name"].toString();
            ne.second = neededElement;
            list.append(ne);
        }

        return list;
    }

    QList<FeatureTypes> applicableFor(QJsonValue value) {
        if(value.isNull() || !value.isArray()) {
            throw logic_error("can not parse json");
        }
        QList<FeatureTypes> list;
        foreach(QJsonValue element, value.toArray()) {
            QString s = element.toString();

            if (s.compare("Circle") == 0 ) {
                list.append(eCircleFeature);
            } else if (s.compare("Cone") == 0 ) {
                list.append(eConeFeature);
            } else if (s.compare("Cylinder") == 0 ) {
                list.append(eCylinderFeature);
            } else if (s.compare("Ellipse") == 0 ) {
                list.append(eEllipseFeature);
            } else if (s.compare("Ellipsoid") == 0 ) {
                list.append(eEllipsoidFeature);
            } else if (s.compare("Hyperboloid") == 0 ) {
                list.append(eHyperboloidFeature);
            } else if (s.compare("Line") == 0 ) {
                list.append(eLineFeature);
            } else if (s.compare("Nurbs") == 0 ) {
                list.append(eNurbsFeature);
            } else if (s.compare("Paraboloid") == 0 ) {
                list.append(eParaboloidFeature);
            } else if (s.compare("Plane") == 0 ) {
                list.append(ePlaneFeature);
            } else if (s.compare("Point") == 0 ) {
                list.append(ePointFeature);
            } else if (s.compare("PointCloud") == 0 ) {
                list.append(ePointCloudFeature);
            } else if (s.compare("ScalarEntityAngle") == 0 ) {
                list.append(eScalarEntityAngleFeature);
            } else if (s.compare("ScalarEntityDistance") == 0 ) {
                list.append(eScalarEntityDistanceFeature);
            } else if (s.compare("ScalarEntityMeasurementSeries") == 0 ) {
                list.append(eScalarEntityMeasurementSeriesFeature);
            } else if (s.compare("ScalarEntityTemperature") == 0 ) {
                list.append(eScalarEntityTemperatureFeature);
            } else if (s.compare("SlottedHole") == 0 ) {
                list.append(eSlottedHoleFeature);
            } else if (s.compare("Sphere") == 0 ) {
                list.append(eSphereFeature);
            } else if (s.compare("Torus") == 0 ) {
                list.append(eTorusFeature);
            } else if (s.compare("CoordinateSystem") == 0 ) {
                list.append(eCoordinateSystemFeature);
            } else if (s.compare("Station") == 0 ) {
                list.append(eStationFeature);
            } else if (s.compare("TrafoParam") == 0 ) {
                list.append(eTrafoParamFeature);
            }
        }

        return list;
    }
};


#endif // CONFIGUREDFUNCTION

