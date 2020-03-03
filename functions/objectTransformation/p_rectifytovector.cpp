#include "p_rectifytovector.h"

/*!
 * \brief RectifyToVector::init
 */
void RectifyToVector::init(){

    //set plugin meta data
    this->metaData.name = "RectifyToVector";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1")
            .arg("This function changes the orientation of a geometry according to the specified direction.");
    this->metaData.iid = ObjectTransformation_iidd;

    //set needed elements
    NeededElement param1;
    param1.description = "Select a direction used for the orientation of the target geometry.";
    param1.infinite = false;
    param1.typeOfElement = eDirectionElement;
    this->neededElements.append(param1);

    NeededElement param2;
    param2.description = "Select a station used for the orientation of the target geometry.";
    param2.infinite = false;
    param2.typeOfElement = eStationElement;
    this->neededElements.append(param2);

    //set spplicable for
    this->applicableFor.append(ePlaneFeature);
    this->applicableFor.append(eCircleFeature);

    //set string parameter
    this->stringParameters.insert("sense", "negative");
    this->stringParameters.insert("sense", "positive");

}

/*!
 * \brief RectifyToVector::exec
 * \param plane
 * \return
 */
bool RectifyToVector::exec(Plane &plane){
    return this->setUpResult(plane);
}

/*!
 * \brief RectifyToVector::exec
 * \param circle
 * \return
 */
bool RectifyToVector::exec(Circle &circle){
    return this->setUpResult(circle);
}

/*!
 * \brief RectifyToVector::exec
 * \param line
 * \return
 */
bool RectifyToVector::exec(Line &line)
{
    return this->setUpResult(line);
}

/*!
 * \brief RectifyToVector::exec
 * \param cylinder
 * \return
 */
bool RectifyToVector::exec(Cylinder &cylinder)
{
    return this->setUpResult(cylinder);
}

/*!
 * \brief RectifyToPoint::direction
 * \param direction
 * \return if valid direction was found
 */
bool RectifyToVector::direction(OiVec &direction) {

    // 1. get and check position of rectify geometry if available
    if(this->inputElements.contains(0) && this->inputElements[0].size() == 1){
        QPointer<Geometry> rectifyGeometry = this->inputElements[0].at(0).geometry;
        if(!rectifyGeometry.isNull() && rectifyGeometry->getIsSolved() && rectifyGeometry->hasDirection()){
            direction = rectifyGeometry->getDirection().getVector();
            return true;
        }
    } else {// 2. get and check position of rectify station if available
        if(this->inputElements.contains(1) && this->inputElements[1].size() == 1) {
            QPointer<Station> rectifyStation = this->inputElements[1].at(0).station;
            if(!rectifyStation.isNull())  {
                direction = rectifyStation->getDirection().getVector();
                return true;
            }
        }
    }

    return false;
}

/*!
 * \brief RectifyToVector::setUpResult
 * \param plane
 * \return
 */
bool RectifyToVector::setUpResult(Geometry &geometry){

    OiVec r_reference;
    if(!direction(r_reference)) {
        return false;
    }

    //get the sense (positive or negative)
    double sense = 1.0;
    if(this->scalarInputParams.stringParameter.contains("sense")){
        if(this->scalarInputParams.stringParameter.value("sense").compare("negative") == 0){
            sense = -1.0;
        }
    }

    //get the direction to compare
    r_reference.normalize();
    OiVec r = geometry.getDirection().getVector();
    r.normalize();

    //calculate the angle between both directions
    double angle = 0.0;
    OiVec::dot(angle, r_reference, r);
    angle = qAbs(qAcos(angle));

    //invert the normal vector if the angle is greater than 90°
    if(angle > PI/2.0){
        r = -1.0 * r;
    }

    //invert the normal vector if sense is negative
    r = sense * r;

    //set result
    Direction direction = geometry.getDirection();
    direction.setVector(r);
    geometry.setDirection(direction);

    return true;

}
