#ifndef CFVISITOR1_H
#define CFVISITOR1_H

#include "treeutil.h"
#include "configuredfunctioncommon.h"

class CFVisitor1: public NodeVisitor {

public:
    CFVisitor1(CFContext ctx, CFData data);

    void pre(QPointer<Node> node, int index, int level) override;

    void post(QPointer<Node> node, int index, int level) override;

private:
    CFContext ctx;
    CFData data;
};

#endif // CFVISITOR1_H
