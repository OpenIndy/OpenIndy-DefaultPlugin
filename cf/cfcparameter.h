#ifndef CFCPARAMETER_H
#define CFCPARAMETER_H

#include "treeutil.h"

class CFCParameter: public Node {

public:
    CFCParameter(QString name, QString comment, QList<QPointer<Node> > subnodes, QObject *parent = 0);

    QList<QPointer<Node> > getSubnodes() override;

    QString getName() override;

    QString prettyPrint() override;

private:
    QString name;
    QString comment;
    QList<QPointer<Node> > subnodes;

};


#endif // CFCPARAMETER_H
