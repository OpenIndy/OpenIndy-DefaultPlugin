#include "p_bestfitcylinder.h"

/*!
 * \brief BestFitCylinder::init
 */
void BestFitCylinder::init(){

    //set plugin meta data
    this->metaData.name = "BestFitCylinder";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
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
 * \brief BestFitCylinder::exec
 * \param cylinder
 * \return
 */
bool BestFitCylinder::exec(Cylinder &cylinder, const FunctionContext &ctx){
    this->statistic.reset();
    return BestFitCylinder::setUpResult(cylinder);
}

/*!
 * \brief BestFitCylinder::setUpResult
 * \param cylinder
 * \return
 */
bool BestFitCylinder::setUpResult(Cylinder &cylinder){

    QList<IdPoint> usablePoints;
    QList<IdPoint> points;
    {
        //get and check input observations
        QList<QPointer<Observation> > allUsableObservations;
        QList<QPointer<Observation> > inputObservations;
        filterObservations(allUsableObservations, inputObservations);
        if(inputObservations.size() < 5){
            emit this->sendMessage(QString("Not enough valid observations to fit the cylinder %1").arg(cylinder.getFeatureName()), eWarningMessage);
            cylinder.setIsSolved(false);
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

    return bestFitCylinder(this, cylinder, points, usablePoints);
}

/**
 * @brief BestFitCylinderAppxDirection::init
 */
void BestFitCylinderAppxDirection::init(){

    //set plugin meta data
    this->metaData.name = "BestFitCylinderAppxDirection";
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

/**
 * @brief BestFitCylinderAppxDummyPoint::init
 */
void BestFitCylinderAppxDummyPoint::init(){

    //set plugin meta data
    this->metaData.name = "BestFitCylinderAppxDummyPoint";
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
    this->scalarInputParams.stringParameter.insert("approximation", "first two dummy points");
}
