#include "p_bestfitplane.h"

/*!
 * \brief BestFitPlane::init
 */
void BestFitPlane::init(){

    //set plugin meta data
    this->metaData.name = "BestFitPlane";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates an adjusted plane.")
            .arg("You can input as many observations as you want which are then used to find the best fit plane.");
    this->metaData.iid = FitFunction_iidd;

    //set needed elements
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least three observations to calculate the best fit plane.";
    param1.infinite = true;
    param1.typeOfElement = eObservationElement;
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(ePlaneFeature);

}

/*!
 * \brief BestFitPlane::exec
 * \param plane
 * \return
 */
bool BestFitPlane::exec(Plane &plane){
    this->statistic.reset();
    return this->setUpResult(plane);
}

/*!
 * \brief BestFitPlane::setUpResult
 * \param plane
 * \return
 */
bool BestFitPlane::setUpResult(Plane &plane){

    //get and check input observations
    if(!this->inputElements.contains(0) || this->inputElements[0].size() < 3){
        emit this->sendMessage(QString("Not enough valid observations to fit the plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }
    QList<QPointer<Observation> > inputObservations;
    foreach(const InputElement &element, this->inputElements[0]){
        if(!element.observation.isNull() && element.observation->getIsSolved() && element.observation->getIsValid()
                && element.shouldBeUsed){
            inputObservations.append(element.observation);
            this->setIsUsed(0, element.id, true);
            continue;
        }
        this->setIsUsed(0, element.id, false);
    }
    if(inputObservations.size() < 3){
        emit this->sendMessage(QString("Not enough valid observations to fit the plane %1").arg(plane.getFeatureName()), eWarningMessage);
        return false;
    }

    //centroid
    OiVec centroid(4);
    foreach(const QPointer<Observation> &obs, inputObservations){
        centroid = centroid + obs->getXYZ();
    }
    centroid = centroid * (1.0/inputObservations.size());
    centroid.removeLast();

    //principle component analysis
    OiMat a(inputObservations.size(), 3);
    for(int i = 0; i < inputObservations.size(); i++){
        a.setAt(i, 0, inputObservations.at(i)->getXYZ().getAt(0) - centroid.getAt(0));
        a.setAt(i, 1, inputObservations.at(i)->getXYZ().getAt(1) - centroid.getAt(1));
        a.setAt(i, 2, inputObservations.at(i)->getXYZ().getAt(2) - centroid.getAt(2));
    }
    OiMat ata = a.t() * a;
    OiMat u(3,3);
    OiVec d(3);
    OiMat v(3,3);
    ata.svd(u, d, v);

    //get smallest eigenvector which is n vector
    int eigenIndex = -1;
    double eVal = 0.0;
    for(int i = 0; i < d.getSize(); i++){
        if(d.getAt(i) < eVal || i == 0){
            eVal = d.getAt(i);
            eigenIndex = i;
        }
    }
    OiVec n(3);
    u.getCol(n, eigenIndex);
    n.normalize();

    //calculate smallest distance of the plane from the origin
    double dOrigin = n.getAt(0) * centroid.getAt(0) + n.getAt(1) * centroid.getAt(1) + n.getAt(2) * centroid.getAt(2);

    //calculate display residuals for each observation
    double distance = 0.0;
    OiVec v_plane(3);
    for(int i = 0; i < inputObservations.size(); i++){

        //calculate residual vector
        distance = n.getAt(0) * inputObservations[i]->getXYZ().getAt(0) + n.getAt(1) * inputObservations[i]->getXYZ().getAt(1)
                + n.getAt(2) * inputObservations[i]->getXYZ().getAt(2) - dOrigin;
        v_plane = distance * n;

        //set up display residual
        Residual residual;
        residual.elementId = inputObservations[i]->getId();
        residual.dimension = eMetric;
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayVX), v_plane.getAt(0));
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayVY), v_plane.getAt(1));
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayVZ), v_plane.getAt(2));
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayV), distance);
        this->statistic.addDisplayResidual(residual);

    }

    //check that the normal vector of the plane is defined by the first three points A, B and C (cross product)
    OiVec ab = inputObservations.at(1)->getXYZ() - inputObservations.at(0)->getXYZ();
    ab.removeLast();
    OiVec ac = inputObservations.at(2)->getXYZ() - inputObservations.at(0)->getXYZ();
    ac.removeLast();
    OiVec direction(3);
    OiVec::cross(direction, ab, ac);
    direction.normalize();
    double angle = 0.0; //angle between n and direction
    OiVec::dot(angle, n, direction);
    angle = qAbs(qAcos(angle));
    if(angle > (PI/2.0)){
        n = n * -1.0;
    }

    //set result
    Position planePosition;
    planePosition.setVector(centroid);
    Direction planeDirection;
    planeDirection.setVector(n);
    plane.setPlane(planePosition, planeDirection);

    //set statistic
    this->statistic.setIsValid(true);
    this->statistic.setStdev(qSqrt(eVal/(inputObservations.size()-3.0)));
    plane.setStatistic(this->statistic);

    return true;

}
