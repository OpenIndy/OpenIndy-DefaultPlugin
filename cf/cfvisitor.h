#ifndef CFVISITOR_H
#define CFVISITOR_H

#include "treeutil.h"
#include "configuredfunctioncommon.h"
#include "configuredfunction.h"
#include "cfcparameter.h"

using namespace oi;

/**
 * @brief Visitor for CFCParameter
 */
class CFVisitor: public NodeVisitor {

public:
    CFVisitor(CFContext ctx);

    void pre(QPointer<Node> node, int index, int level) override;
    void post(QPointer<Node> node, int index, int level) override;

private:
    CFContext ctx;
    CFData data;
};

#endif // CFVISITOR_H
