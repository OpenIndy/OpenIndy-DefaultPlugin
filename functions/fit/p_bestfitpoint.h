#ifndef P_BESTFITPOINT_H
#define P_BESTFITPOINT_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;
using namespace std;

/*!
 * \brief The BestFitPoint class
 */
class BestFitPoint : public FitFunction
{
    Q_OBJECT

public:

    BestFitPoint(QObject *parent = 0) : FitFunction(parent),
        key("disable all observations but last one"), value_no("no"), value_yes("yes") {
    }

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

    const QString key; // disable all observations but last one
    const QString value_no;
    const QString value_yes;

};

#endif // P_BESTFITPOINT_H
