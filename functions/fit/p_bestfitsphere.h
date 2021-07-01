#ifndef P_BESTFITSPHERE_H
#define P_BESTFITSPHERE_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The BestFitSphere class
 */
class BestFitSphere : public FitFunction
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

    bool exec(Sphere &sphere, const FunctionContext &ctx = FunctionContext{});

private:

    //##############
    //helper methods
    //##############

    bool approximate(Sphere &sphere, const QList<QPointer<Observation> > &inputObservations);
    bool fit(Sphere &sphere, const QList<QPointer<Observation> > &inputObservations, const QList<QPointer<Observation> > &allUsableObservations);

};

#endif // P_BESTFITSPHERE_H
