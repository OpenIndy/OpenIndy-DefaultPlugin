#ifndef CONFIGUREDFUNCTIONCONFIG
#define CONFIGUREDFUNCTIONCONFIG

#include "function.h"
#include "constructfunction.h"
#include "featurewrapper.h"
#include "types.h"

using namespace oi;

struct InputElementMapping {
    int functionIndex;
    int srcInputElementIndex;
    int dstInputElementIndex;
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
#endif // CONFIGUREDFUNCTIONCONFIG

