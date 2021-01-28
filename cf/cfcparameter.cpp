#include "cfcparameter.h"

CFCParameter::CFCParameter(QString name, QString comment, QString feature, QList<QPointer<Node> > subnodes, QObject *parent): name(name), comment(comment), feature(feature), subnodes(subnodes) {

}

QList<QPointer<Node> > CFCParameter::getSubnodes() {
    return subnodes;
}

QString CFCParameter::getName() {
    return name;
}

QString CFCParameter::getFeature(){
    return feature;
}

QString CFCParameter::prettyPrint() {
    QString s;
    s += "{ name: '" + this->name
            + "', feature: '" + this->feature
            + "', comment: '" + this->comment + "'";
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
