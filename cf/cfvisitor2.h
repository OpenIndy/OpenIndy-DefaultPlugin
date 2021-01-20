#ifndef CFVISITOR2_H
#define CFVISITOR2_H

#include "treeutil.h"
#include "configuredfunctioncommon.h"
#include "configuredfunction2.h"

using namespace oi;

class CFVisitor2: public NodeVisitor {

public:
    CFVisitor2(CFContext ctx);

    void pre(QPointer<Node> node, int index, int level) override;
    void post(QPointer<Node> node, int index, int level) override;

private:
    CFContext ctx;
    CFData data;
};
#endif // CFVISITOR2_H
