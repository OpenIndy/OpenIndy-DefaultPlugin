#ifndef P_RECTIFYTOPOINT_H
#define P_RECTIFYTOPOINT_H

#include <QObject>
#include <QPointer>

#include "objecttransformation.h"
#include "oivec.h"
#include "oimat.h"

using namespace oi;

/*!
 * \brief The RectifyToPoint class
 */
class RectifyToPoint : public ObjectTransformation
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

    bool exec(Plane &plane);
    bool exec(Circle &circle);

private:

    //##############
    //helper methods
    //##############

    bool setUpResult(Plane &plane);
    bool setUpResult(Circle &circle);

};

#endif // P_RECTIFYTOPOINT_H
