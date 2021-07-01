#include "p_bestfitcircleinplanefrompoints.h"


/*!
 * \brief BestFitCircleInPlaneFromPoints::init
 */
void BestFitCircleInPlaneFromPoints::init(){

    //set plugin meta data
    this->metaData.name = "BestFitCircleInPlaneFromPoints";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "esc";
    this->metaData.description = QString("%1 %2 %3 %4")
            .arg("This function calculates an adjusted circle.")
            .arg("The points are registered into a best fit plane first and afterward a 2D circle is approximated inside the plane.")
            .arg("You can input as many points as you want which are then used to find the best fit circle.")
            .arg("There will be no plane feature/calculations vissible, the calculation is performed inside the function");
    this->metaData.iid = FitFunction_iidd;

    //set needed elements
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least three points to calculate the best fit circle.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    this->neededElements.append(param1);

    NeededElement param2;
    param2.description = "Dummy point to indicate circle normal.";
    param2.infinite = true;
    param2.typeOfElement = ePointElement;
    param2.key = InputElementKey::eDummyPoint;
    this->neededElements.append(param2);

    //set spplicable for
    this->applicableFor.append(eCircleFeature);

}

/*!
 * \brief BestFitCircleInPlaneFromPoints::exec
 * \param circle
 * \return
 */
bool BestFitCircleInPlaneFromPoints::exec(Circle &circle, const FunctionContext &ctx){
    this->statistic.reset();
    return this->setUpResult(circle);
}

/*!
 * \brief BestFitCircleInPlaneFromPoints::setUpResult
 * \param circle
 * \return
 */
bool BestFitCircleInPlaneFromPoints::setUpResult(Circle &circle){

    //get and check input observations
    if(!this->inputElements.contains(0) || this->inputElements[0].size() < 3){
        emit this->sendMessage(QString("Not enough valid observations to fit the circle %1").arg(circle.getFeatureName()), eWarningMessage);
        return false;
    }

    QList<IdPoint> usablePoints;
    QList<IdPoint> points;
    {
        QList<QPointer<Point> > allUsablePoints;
        QList<QPointer<Point> > inputPoints;
        foreach(const InputElement &element, this->inputElements[0]){
            allUsablePoints.append(element.point);
            if(!element.point.isNull() && element.point->getIsSolved()){
                inputPoints.append(element.point);
                this->setIsUsed(0, element.id, true);
            }  else {
                this->setIsUsed(0, element.id, false);
            }
        }

        if(inputPoints.size() < 3){
            emit this->sendMessage(QString("Not enough valid observations to fit the plane %1").arg(circle.getFeatureName()), eWarningMessage);
            return false;
        }

        foreach(const QPointer<Point> &p, allUsablePoints) {
            IdPoint point;
            point.id = p->getId();
            point.xyz = p->getPosition().getVectorH();
            usablePoints.append(point);
        }
        foreach(const QPointer<Point> &p, inputPoints) {
            IdPoint point;
            point.id = p->getId();
            point.xyz = p->getPosition().getVectorH();
            points.append(point);
        }
    }

    return bestFitCircleInPlane(this, circle, points, usablePoints);
}
