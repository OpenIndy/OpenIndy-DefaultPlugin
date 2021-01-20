#include "zxplane.h"


void ZXPlane::init() {
    this->metaData.name = "ZXPlane";
    this->applicableFor.append(ePlaneFeature);
}
bool ZXPlane::exec(Plane &plane)
{
    OiVec p(3);
    p.setAt(0, 0.);
    p.setAt(1, 0.);
    p.setAt(2, 0.);

    OiVec n(3);
    n.setAt(0, 0.);
    n.setAt(1, 1.);
    n.setAt(2, 0.);

    Position planePosition;
    planePosition.setVector(p);
    Direction planeDirection;
    planeDirection.setVector(n);
    plane.setPlane(planePosition, planeDirection);

    return true;
}

