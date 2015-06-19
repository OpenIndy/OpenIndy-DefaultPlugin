#ifndef P_REGISTER_H
#define P_REGISTER_H

#include <QObject>
#include <QPointer>

#include "objecttransformation.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The Register class
 */
class Register : public ObjectTransformation
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
    bool exec(Sphere &sphere);
    bool exec(Circle &circle);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Point &point);
    bool setUpResult(Sphere &sphere);
    bool setUpResult(Circle &circle);

};

#endif // P_REGISTER_H
