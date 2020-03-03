#ifndef P_INTERSECTLINELINE_H
#define P_INTERSECTLINELINE_H

#include <QObject>
#include <QPointer>

#include "constructfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

enum IntersectionTypes {
    eAtFirstLine,
    eAtSecondLine,
    eMidPoint
};

/*!
 * \brief The IntersectLineline class
 */
class IntersectLineLine : public ConstructFunction
{
    Q_OBJECT

public:

    //##############################
    //function initialization method
    //##############################

    void init();

protected:

    //############
    //exec methods
    //############

    bool exec(Point &point);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Point &point);
    bool isParallel(const QPointer<Line> &line1, const QPointer<Line> &line2);
    IntersectionTypes getIntersectionType();

};

#endif // P_INTERSECTLINELINE_H
