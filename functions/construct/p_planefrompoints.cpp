#include "p_planefrompoints.h"

/*!
 * \brief PlaneFromPoints::init
 */
void PlaneFromPoints::init(){

    //set plugin meta data
    this->metaData.name = "PlaneFromPoints";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates an adjusted plane.")
            .arg("You can input as many points as you want which are then used to find the best fit plane.");
    this->metaData.iid = ConstructFunction_iidd;

    //set needed elements
    NeededElement param1;
    param1.description = "Select at least three points to calculate the best fit plane.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(ePlaneFeature);

}

/*!
 * \brief PlaneFromPoints::exec
 * \param plane
 * \return
 */
bool PlaneFromPoints::exec(Plane &plane){
    return this->setUpResult(plane);
}

/*!
 * \brief PlaneFromPoints::setUpResult
 * \param plane
 * \return
 */
bool PlaneFromPoints::setUpResult(Plane &plane){

    //get and check input points
    if(!this->inputElements.contains(0) || this->inputElements[0].size() < 3){
        emit this->sendMessage(QString("Not enough valid points to fit the plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }
    QList<QPointer<Point> > inputPoints;
    foreach(const InputElement &element, this->inputElements[0]){
        if(!element.point.isNull() && element.point->getIsSolved()){
            inputPoints.append(element.point);
            this->setIsUsed(0, element.id, true);
        }
        this->setIsUsed(0, element.id, false);
    }
    if(inputPoints.size() < 3){
        emit this->sendMessage(QString("Not enough valid points to fit the plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }

    //centroid
    QList<IdPoint> points;
    foreach(const QPointer<Point> &p, inputPoints) {
        IdPoint point;
        point.id = p->getId();
        point.xyz = p->getPosition().getVector();
        points.append(point);
    }
    OiVec centroid(3);
    OiVec n(3);
    double eVal = 0.0;
    if(!bestFitPlane(centroid, n, eVal,points)) {
        emit this->sendMessage(QString("Cannot fit plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }


    //check that the normal vector of the plane is defined by the first three points A, B and C (cross product)
    OiVec ab = inputPoints.at(1)->getPosition().getVector() - inputPoints.at(0)->getPosition().getVector();
    OiVec ac = inputPoints.at(2)->getPosition().getVector() - inputPoints.at(0)->getPosition().getVector();
    OiVec direction(3);
    direction.normalize();
    OiVec::cross(direction, ab, ac);
    rectifyNormalToDirection(n, direction);

    //set result
    Position planePosition;
    planePosition.setVector(centroid);
    Direction planeDirection;
    planeDirection.setVector(n);
    plane.setPlane(planePosition, planeDirection);

    //set statistic
    this->statistic.setIsValid(true);
    this->statistic.setStdev(qSqrt(eVal/(inputPoints.size()-3.0)));
    plane.setStatistic(this->statistic);

    return true;

}
