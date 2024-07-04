#include "p_bestfitcylinderfrompoints.h"

/*!
 * \brief BestFitCylinderFromPoints::init
 */
void BestFitCylinderFromPoints::init(){

    //set plugin meta data
    this->metaData.name = "BestFitCylinderFromPoints";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "esc";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates an adjusted cylinder.")
            .arg("You can input as many observations as you want which are then used to find the best fit cylinder.");
    this->metaData.iid = FitFunction_iidd;

    //set needed elements
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least five observations to calculate the best fit cylinder.";
    param1.infinite = true;
    param1.typeOfElement = eObservationElement;
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(eCylinderFeature);

    this->stringParameters.insert("approximation", "first two points");
    this->stringParameters.insert("approximation", "guess axis");

    this->scalarInputParams.isValid = true;
    this->scalarInputParams.stringParameter.insert("approximation", "first two points"); // default
}

/*!
 * \brief BestFitCylinderFromPoints::exec
 * \param cylinder
 * \return
 */
bool BestFitCylinderFromPoints::exec(Cylinder &cylinder, const FunctionContext &ctx){
    this->statistic.reset();
    return BestFitCylinderFromPoints::setUpResult(cylinder);
}

/*!
 * \brief BestFitCylinderFromPoints::setUpResult
 * \param cylinder
 * \return
 */
bool BestFitCylinderFromPoints::setUpResult(Cylinder &cylinder){

    QList<IdPoint> usablePoints;
    QList<IdPoint> points;
    {
        //get and check input observations
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

        if(inputPoints.size() < 5){
            emit this->sendMessage(QString("Not enough valid points to fit the cylinder %1").arg(cylinder.getFeatureName()), eWarningMessage);
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

    return bestFitCylinder(this, cylinder, points, usablePoints);
}

/*!
 * \brief BestFitCylinderFromPointsAppxDirection::init
 */
void BestFitCylinderFromPointsAppxDirection::init(){

    //set plugin meta data
    this->metaData.name = "BestFitCylinderFromPointsAppxDirection";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "esc";
    this->metaData.description = QString("%1 %2 %3")
            .arg("This function calculates an adjusted cylinder.")
            .arg("You can input as many observations as you want which are then used to find the best fit cylinder.")
            .arg("Additionally you should input a direction for approximation, this can be done by all vector geometries");
    this->metaData.iid = FitFunction_iidd;

    //set needed elements
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least five observations to calculate the best fit cylinder.";
    param1.infinite = true;
    param1.typeOfElement = eObservationElement;
    this->neededElements.append(param1);

    NeededElement param2;
    param2.description = "approximation direction.";
    param2.infinite = true;
    param2.typeOfElement = eDirectionElement;
    this->neededElements.append(param2);

    //set spplicable for
    this->applicableFor.append(eCylinderFeature);

    this->scalarInputParams.isValid = true;
    this->scalarInputParams.stringParameter.insert("approximation", "direction"); // default
}

/*!
 * \brief BestFitCylinderFromPointsAppxDummyPoint::init
 */
void BestFitCylinderFromPointsAppxDummyPoint::init(){

    //set plugin meta data
    this->metaData.name = "BestFitCylinderFromPointsAppxDummyPoint";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "esc";
    this->metaData.description = QString("%1 %2 %3")
            .arg("This function calculates an adjusted cylinder.")
            .arg("You can input as many observations as you want which are then used to find the best fit cylinder.")
            .arg("Additionally you should input dummy points for approximation, this can be done by all point geometries");
    this->metaData.iid = FitFunction_iidd;

    //set needed elements
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least five observations to calculate the best fit cylinder.";
    param1.infinite = true;
    param1.typeOfElement = eObservationElement;
    this->neededElements.append(param1);

    NeededElement param3;
    param3.description = "Dummy points to indicate cylinder normal.";
    param3.infinite = true;
    param3.typeOfElement = eObservationElement;
    param3.key = InputElementKey::eDummyPoint;
    this->neededElements.append(param3);

    //set spplicable for
    this->applicableFor.append(eCylinderFeature);

    this->scalarInputParams.isValid = true;
    this->scalarInputParams.stringParameter.insert("approximation", "first two dummy points"); // default
}
