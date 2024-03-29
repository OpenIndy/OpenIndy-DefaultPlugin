#include "p_bestfitline.h"

void BestFitLine::init(){

    //set plugin meta data
    this->metaData.name = "BestFitLine";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates an adjusted line.")
            .arg("You can input as many observations as you want which are then used to find the best fit 3D line.");
    this->metaData.iid = FitFunction_iidd;

    //set needed elements
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least two observations to calculate the best fit line.";
    param1.infinite = true;
    param1.typeOfElement = eObservationElement;
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(eLineFeature);

}

/*!
 * \brief BestFitLine::exec
 * \param line
 * \return
 */
bool BestFitLine::exec(Line &line){
    this->statistic.reset();
    return this->setUpResult(line);
}

/*!
 * \brief BestFitLine::setUpResult
 * Set up result and statistic for type plane
 * \param p
 */
bool BestFitLine::setUpResult(Line &line){

    //get and check input observations
    if(!this->inputElements.contains(0) || this->inputElements[0].size() < 2){
        emit this->sendMessage(QString("Not enough valid observations to fit the line %1").arg(line.getFeatureName()), eWarningMessage);
        return false;
    }
    QList<QPointer<Observation> > allUsableObservations;
    QList<QPointer<Observation> > inputObservations;
    filterObservations(allUsableObservations, inputObservations);
    if(inputObservations.size() < 2){
        emit this->sendMessage(QString("Not enough valid observations to fit the line %1").arg(line.getFeatureName()), eWarningMessage);
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

    //get largest eigenvector which is r vector and v^T * v as sum of the 2 smaller eigenvectors
    int eigenIndex = -1;
    double eVal = 0.0;
    double vtv = 0.0;
    for(int i = 0; i < d.getSize(); i++){
        if(d.getAt(i) > eVal || i == 0){
            eVal = d.getAt(i);
            eigenIndex = i;
        }else{
            vtv += d.getAt(i);
        }
    }
    OiVec r(3);
    u.getCol(r, eigenIndex);
    r.normalize();

    //check that the orientation of the line is from first to second observation
    OiVec pos1 = inputObservations.at(0)->getXYZ();
    pos1.removeLast();
    OiVec pos2 = inputObservations.at(1)->getXYZ();
    pos2.removeLast();
    OiVec direction = pos2 - pos1;
    direction.normalize();
    rectifyNormalToDirection(r, direction);

    //calculate display residuals for each observation
    foreach(const QPointer<Observation> &observation, allUsableObservations){
        double distance = 0.0;
        OiVec v_line(3);
        //calculate perpendicular
        v_line.setAt(0, observation->getXYZ().getAt(0) - centroid.getAt(0));
        v_line.setAt(1, observation->getXYZ().getAt(1) - centroid.getAt(1));
        v_line.setAt(2, observation->getXYZ().getAt(2) - centroid.getAt(2));
        OiVec::dot(distance, r, v_line);
        v_line = centroid + distance * r;

        //calculate residual vector
        v_line.setAt(0, observation->getXYZ().getAt(0) - v_line.getAt(0));
        v_line.setAt(1, observation->getXYZ().getAt(1) - v_line.getAt(1));
        v_line.setAt(2, observation->getXYZ().getAt(2) - v_line.getAt(2));

        //set up display residual
        double dot;
        OiVec::dot(dot, v_line, v_line);
        addDisplayResidual(observation->getId(), v_line.getAt(0), v_line.getAt(1), v_line.getAt(2), qSqrt(dot));

    }

    //set result
    Position linePosition;
    linePosition.setVector(centroid);
    Direction lineDirection;
    lineDirection.setVector(r);
    line.setLine(linePosition, lineDirection);

    //set statistic
    this->statistic.setIsValid(true);
    this->statistic.setStdev(qSqrt(vtv/(inputObservations.size()-2.0)));
    line.setStatistic(this->statistic);

    return true;

}
