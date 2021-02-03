#ifndef TREEUTIL_H
#define TREEUTIL_H

#include <QString>
#include <QList>
#include <QPointer>
#include <QDebug>

class Node: public QObject {

    Q_OBJECT

public:
    virtual QString getName() = 0;
    virtual QString prettyPrint() = 0;
    virtual QList<QPointer<Node> > getSubnodes() = 0;

};

class NodeVisitor: public QObject {

    Q_OBJECT

public:
    virtual void pre(QPointer<Node> node, int index, int level) = 0;
    virtual void post(QPointer<Node> node, int index, int level) = 0;

    void debug(QString label, QString message, int level) {
        qDebug() << QString("%1%2: %3").arg(QString(level*2, QChar(' '))).arg(label).arg(message);
    }
};

class PrintVisitor: public NodeVisitor {

public:
    void pre(QPointer<Node> node, int index, int level) override {
        debug(QString("pre L%1 I%2").arg(level).arg(index), node->prettyPrint(), level);
    }

    void post(QPointer<Node> node, int index, int level) override {
        debug(QString("post L%1 I%2").arg(level).arg(index), node->prettyPrint(), level);
    }
};

class ListVisitor: public NodeVisitor {
public:
    void pre(QPointer<Node> node, int index, int level) override {
        foreach(QPointer<NodeVisitor> v, list) {
            v->pre(node, index, level);
        }
    }

    void post(QPointer<Node> node, int index, int level) override {
        foreach(QPointer<NodeVisitor> v, list) {
            v->post(node, index, level);
        }
    }

    QList<QPointer<NodeVisitor> > list;
};

class TreeUtil {
public:
    static void traversal(QPointer<Node> node, NodeVisitor &h, int index = 0, int level = 0) {
        h.pre(node, index, level);

        for(int idx = 0; idx < node->getSubnodes().size(); idx++) {
            int lvl = level;
            traversal(node->getSubnodes()[idx], h, idx, ++lvl);
        }

        h.post(node, index, level);
    }
};

#endif // TREEUTIL_H

