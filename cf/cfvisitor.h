#ifndef CFVISITOR_H
#define CFVISITOR_H

#include "treeutil.h"
#include "cfcontext.h"
#include "cffunctiondata.h"

using namespace oi;

/**
 * @brief Visitor for CFCParameter
 */
class CFVisitor: public NodeVisitor {

    Q_OBJECT

public:

    CFVisitor(CFContext ctx);

    void pre(QPointer<Node> node, int index, int level) override;
    void post(QPointer<Node> node, int index, int level) override;

private:

    CFContext ctx;
    // data for each function stack is pushed / poped while traversal the node tree
    QStack<CFFunctionData> data;
};

#endif // CFVISITOR_H
