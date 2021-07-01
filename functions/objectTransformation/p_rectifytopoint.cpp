#include "p_rectifytopoint.h"

/*!
 * \brief RectifyToPoint::init
 */
void RectifyToPoint::init(){

    //set plugin meta data
    this->metaData.name = "RectifyToPoint";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1")
            .arg("This function changes the orientation of a geometry according to the specified position.");
    this->metaData.iid = ObjectTransformation_iidd;

    //set needed elements
    NeededElement param1;
    param1.description = "Select a position used for the orientation of the target geometry.";
    param1.infinite = false;
    param1.typeOfElement = ePositionElement;
    this->neededElements.append(param1);

    NeededElement param2;
    param2.description = "Select a station used for the orientation of the target geometry.";
    param2.infinite = false;
    param2.typeOfElement = eStationElement;
    this->neededElements.append(param2);

    //set spplicable for
    this->applicableFor.append(ePlaneFeature);
    this->applicableFor.append(eCircleFeature);
    this->applicableFor.append(eLineFeature);
    this->applicableFor.append(eCylinderFeature);

    //set string parameter
    this->stringParameters.insert("sense", "negative");
    this->stringParameters.insert("sense", "positive");

}

/*!
 * \brief RectifyToPoint::exec
 * \param plane
 * \return
 */
bool RectifyToPoint::exec(Plane &plane, const FunctionContext &ctx){
    return this->setUpResult(plane);
}

/*!
 * \brief RectifyToPoint::exec
 * \param circle
 * \return
 */
bool RectifyToPoint::exec(Circle &circle, const FunctionContext &ctx){
    return this->setUpResult(circle);
}

/*!
 * \brief RectifyToPoint::exec
 * \param line
 * \return
 */
bool RectifyToPoint::exec(Line &line, const FunctionContext &ctx)
{
    return this->setUpResult(line);
}

/*!
 * \brief RectifyToPoint::exec
 * \param Cylinder
 * \return
 */
bool RectifyToPoint::exec(Cylinder &cylinder, const FunctionContext &ctx)
{
    return this->setUpResult(cylinder);
}

/*!
 * \brief RectifyToPoint::point
 * \param point
 * \return if valid point was found
 */
bool RectifyToPoint::point(OiVec &point) {

    // 1. get and check position of rectify geometry if available
    if(this->inputElements.contains(0) && this->inputElements[0].size() == 1){
        QPointer<Geometry> rectifyGeometry = this->inputElements[0].at(0).geometry;
        if(!rectifyGeometry.isNull() && rectifyGeometry->getIsSolved() && rectifyGeometry->hasPosition()){
            point = rectifyGeometry->getPosition().getVector();
            return true;
        }
    } else {// 2. get and check position of rectify station if available
        if(this->inputElements.contains(1) && this->inputElements[1].size() == 1) {
            QPointer<Station> rectifyStation = this->inputElements[1].at(0).station;
            if(!rectifyStation.isNull() && !rectifyStation->getPosition().isNull() && rectifyStation->getPosition()->hasPosition()) {
                point = rectifyStation->getPosition()->getPosition().getVector();
                return true;
            }
        }
    }

    return false;
}

/*!
 * \brief RectifyToPoint::setUpResult
 * \param plane
 * \return
 */
bool RectifyToPoint::setUpResult(Geometry &geometry){

    OiVec p;
    if(!point(p)) {
        return false;
    }

    //get the sense (positive or negative)
    double sense = 1.0;
    if(this->scalarInputParams.stringParameter.contains("sense")){
        if(this->scalarInputParams.stringParameter.value("sense").compare("negative") == 0){
            sense = -1.0;
        }
    }

    //get the position of the point and the plane and the normal vector
    OiVec n = geometry.getDirection().getVector();
    n.normalize();
    OiVec x = geometry.getPosition().getVector();

    //calculate the distance of the plane from the origin
    double d;
    OiVec::dot(d, x, n);
    if(d < 0.0){
        n = -1.0 * n;
        d = -d;
    }

    //calculate the distance of the point position from the plane
    double s;
    OiVec::dot(s, p, n);
    s = s - d;

    //invert the distance if sense is negative
    s = s * sense;

    //invert the normal vector of the plane if it has the wrong orientation
    if(s < 0.0){
        n = -1.0 * n;
    }

    //set result
    Direction direction = geometry.getDirection();
    direction.setVector(n);
    geometry.setDirection(direction);

    return true;

}
