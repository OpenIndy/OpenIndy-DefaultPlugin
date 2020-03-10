#include "p_bestfitcircleinplane.h"

/*!
 * \brief BestFitCircleInPlane::init
 */
void BestFitCircleInPlane::init(){

    //set plugin meta data
    this->metaData.name = "BestFitCircleInPlane";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1 %2 %3")
            .arg("This function calculates an adjusted circle.")
            .arg("The observations are registered into a best fit plane first and afterward a 2D circle is approximated inside the plane.")
            .arg("You can input as many observations as you want which are then used to find the best fit circle.");
    this->metaData.iid = FitFunction_iidd;

    //set needed elements
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least three observations to calculate the best fit circle.";
    param1.infinite = true;
    param1.typeOfElement = eObservationElement;
    this->neededElements.append(param1);

    NeededElement param2;
    param2.description = "Dummy point to indicate circle normal.";
    param2.infinite = true;
    param2.typeOfElement = eObservationElement;
    param2.key = InputElementKey::eDummyPoint;
    this->neededElements.append(param2);

    //set spplicable for
    this->applicableFor.append(eCircleFeature);

}

/*!
 * \brief BestFitCircleInPlane::exec
 * \param circle
 * \return
 */
bool BestFitCircleInPlane::exec(Circle &circle){
    this->statistic.reset();
    return this->setUpResult(circle);
}

/*!
 * \brief BestFitCircleInPlane::setUpResult
 * \param circle
 * \return
 */
bool BestFitCircleInPlane::setUpResult(Circle &circle){

    //get and check input observations
    if(!this->inputElements.contains(0) || this->inputElements[0].size() < 3){
        emit this->sendMessage(QString("Not enough valid observations to fit the circle %1").arg(circle.getFeatureName()), eWarningMessage);
        return false;
    }

    QList<IdPoint> usablePoints;
    QList<IdPoint> points;
    {
        QList<QPointer<Observation> > allUsableObservations;
        QList<QPointer<Observation> > inputObservations;
        filterObservations(allUsableObservations, inputObservations);
        if(inputObservations.size() < 3){
            emit this->sendMessage(QString("Not enough valid observations to fit the plane %1").arg(circle.getFeatureName()), eWarningMessage);
            return false;
        }

        foreach(const QPointer<Observation> &obs, allUsableObservations) {
            IdPoint point;
            point.id = obs->getId();
            point.xyz = obs->getXYZ();
            usablePoints.append(point);
        }
        foreach(const QPointer<Observation> &obs, inputObservations) {
            IdPoint point;
            point.id = obs->getId();
            point.xyz = obs->getXYZ();
            points.append(point);
        }
    }

    return bestFitCircleInPlane(this, circle, points, usablePoints);
}
