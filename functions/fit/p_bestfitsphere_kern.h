#ifndef P_BESTFITSPHERE_KERN_H
#define P_BESTFITSPHERE_KERN_H

#include <QObject>
#include <QPointer>
#include <QtMath>

#include "fitfunction.h"
#include "oivec.h"
#include "oimat.h"
#include "cfitting_sphere.h"

/*!
 * \brief The BestFitSphere_kern class
 */
class BestFitSphere_kern : public FitFunction
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

    bool exec(Sphere &sphere);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Sphere &sphere);

};

#endif // P_BESTFITSPHERE_KERN_H
