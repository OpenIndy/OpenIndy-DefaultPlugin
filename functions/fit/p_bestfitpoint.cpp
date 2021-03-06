#include "p_bestfitpoint.h"

/*!
 * \brief BestFitPoint::init
 */
void BestFitPoint::init(){

    //set plugin meta data
    this->metaData.name = "BestFitPoint";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates an adjusted point.")
            .arg("You can input as many observations as you want which are then used to find the best fit 3D point.");
    this->metaData.iid = FitFunction_iidd;

    //set needed elements
    this->neededElements.clear();
    NeededElement param1;
    param1.description = "Select at least one observation to calculate the best fit point.";
    param1.infinite = true;
    param1.typeOfElement = eObservationElement;
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(ePointFeature);

    this->stringParameters.insert(key, value_no);
    this->stringParameters.insert(key, value_yes);
    this->scalarInputParams.isValid = true;
    this->scalarInputParams.stringParameter.insert(key, value_no); // default
}

/*!
 * \brief BestFitPoint::exec
 * \param point
 * \return
 */
bool BestFitPoint::exec(Point &point){
    this->statistic.reset();
    return this->setUpResult(point);
}

/*!
 * \brief BestFitPoint::setUpResult
 * \param point
 * \return
 */
bool BestFitPoint::setUpResult(Point &point){

    //get and check input observations
    if(!this->inputElements.contains(0) || this->inputElements[0].size() < 1){
        emit this->sendMessage(QString("Not enough valid observations to fit the point %1").arg(point.getFeatureName()), eWarningMessage);
        return false;
    }

    if(this->scalarInputParams.stringParameter.contains(key)
        && this->scalarInputParams.stringParameter.value(key).compare(value_yes) == 0){

        for ( int sideInt = SensorFaces::eFrontSide; sideInt <= SensorFaces::eUndefinedSide; sideInt++ ) {
           SensorFaces side = static_cast<SensorFaces>(sideInt);
           QList<int> obsIds; // observation ids
           foreach(InputElement e, this->inputElements[0]) {
               if(e.observation->getReading()->getFace() == side) {
                    obsIds.append(e.id);
               }
           }
           std::sort(obsIds.begin(), obsIds.end()); // sort by id
           for(int i=0; i< (obsIds.count() -1); ++i ) {
               // disable all observations but last one
               this->setShouldBeUsed(0, obsIds[i], false);
           }
        }

    }

    QList<QPointer<Observation> > allUsableObservations;
    QList<QPointer<Observation> > inputObservations;
    filterObservations(allUsableObservations, inputObservations);
    if(inputObservations.size() < 1){
        emit this->sendMessage(QString("Not enough valid observations to fit the point %1").arg(point.getFeatureName()), eWarningMessage);
        return false;
    }

    //fill l vector
    OiVec l;
    foreach(const QPointer<Observation> &obs, inputObservations){
        l.add( obs->getXYZ().getAt(0) );
        l.add( obs->getXYZ().getAt(1) );
        l.add( obs->getXYZ().getAt(2) );
    }

    //fill A matrix
    OiMat a(l.getSize(), 3);
    for(int i = 0; i < l.getSize(); i++){
        if( (i%3) == 0 ){
            a.setAt(i, Point::unknownCenterX, 1.0);
        }else if( (i%3) == 1 ){
            a.setAt(i, Point::unknownCenterY, 1.0);
        }else if( (i%3) == 2 ){
            a.setAt(i, Point::unknownCenterZ, 1.0);
        }
    }

    //adjust
    OiMat n = a.t() * a;
    OiVec c = a.t() * l;
    OiMat qxx;
    OiVec x, v;
    try{
        qxx = n.inv();
        x = qxx * c;
        v = a * x - l;
    }catch(const logic_error &e){
        emit this->sendMessage(e.what(), eErrorMessage);
        return false;
    }catch(const runtime_error &e){
        emit this->sendMessage(e.what(), eErrorMessage);
        return false;
    }

    //calculate point based corrections
    OiVec corr;

    foreach(const QPointer<Observation> &observation, allUsableObservations){

        double _vx = x.getAt(0) - observation->getXYZ().getAt(0);
        double _vy = x.getAt(1) - observation->getXYZ().getAt(1);
        double _vz = x.getAt(2) - observation->getXYZ().getAt(2);

        double _corr = qSqrt(
                            qPow(_vx, 2)
                          + qPow(_vy, 2)
                          + qPow(_vz, 2)
                          );

        if(inputObservations.contains(observation)) {
            corr.add(_corr);
        }

        addDisplayResidual(observation->getId(), _vx, _vy, _vz, _corr);
    }

    //calculate standard deviation
    double stdev = 0.0;
    OiVec::dot(stdev, corr, corr);
    stdev = qSqrt(stdev / (inputObservations.size() - 1.0));

    //set result
    Position position;
    position.setVector(x);
    point.setPoint(position);

    //set statistic
    this->statistic.setIsValid(true);
    this->statistic.setQxx(qxx);
    this->statistic.setV(corr);
    this->statistic.setStdev(stdev);
    point.setStatistic(this->statistic);

    return true;

}
