#include "p_pseudotracker.h"

/*!
 * \brief PseudoTracker::init
 */
void PseudoTracker::init(){

    //set plugin meta data
    this->metaData.name = "PseudoTracker";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "mlux";
    this->metaData.description = "Simulates a laser tracker.";
    this->metaData.iid = LaserTracker_iidd;

    //set supported reading types
    this->supportedReadingTypes.append(eCartesianReading);
    this->supportedReadingTypes.append(ePolarReading);
    this->supportedReadingTypes.append(eDirectionReading);
    this->supportedReadingTypes.append(eDistanceReading);
    this->supportedReadingTypes.append(eLevelReading);
    this->supportedReadingTypes.append(eTemperatureReading);

    //set supported sensor actions
    this->supportedSensorActions.append(eHome);
    this->supportedSensorActions.append(eInitialize);
    this->supportedSensorActions.append(eMoveAngle);
    this->supportedSensorActions.append(eMoveXYZ);
    this->supportedSensorActions.append(eToggleSight);
    this->supportedSensorActions.append(eCompensation);
    this->supportedSensorActions.append(eMotorState);
    this->supportedSensorActions.append(eSearch);

    //set supported connection types
    this->supportedConnectionTypes.append(eNetworkConnection);
    this->supportedConnectionTypes.append(eSerialConnection);

    //set double parameter
    this->doubleParameters.insert("lambda [mm]",0.000403);
    this->doubleParameters.insert("mu",0.000005);
    this->doubleParameters.insert("ex [mm]",0.0000122);
    this->doubleParameters.insert("by [mm]",0.0000654);
    this->doubleParameters.insert("bz [mm]",0.0000974);
    this->doubleParameters.insert("alpha [arc sec]",0.128);
    this->doubleParameters.insert("gamma [arc sec]",0.079);
    this->doubleParameters.insert("Aa1 [arc sec]",0.064);
    this->doubleParameters.insert("Ba1 [arc sec]",0.080);
    this->doubleParameters.insert("Aa2 [arc sec]",0.073);
    this->doubleParameters.insert("Ba2 [arc sec]",0.090);
    this->doubleParameters.insert("Ae0 [arc sec]",0.223);
    this->doubleParameters.insert("Ae1 [arc sec]",0.152);
    this->doubleParameters.insert("Be1 [arc sec]",0.183);
    this->doubleParameters.insert("Ae2 [arc sec]",0.214);
    this->doubleParameters.insert("Be2 [arc sec]",0.179);

    //set string parameter
    this->stringParameters.insert("active probe", "0.5''");
    this->stringParameters.insert("active probe", "1.0''");
    this->stringParameters.insert("active probe", "1.5''");

    //set self defined actions
    this->selfDefinedActions.append("echo(Alt+E)");
    this->selfDefinedActions.append("stopMeasure"); // e.g. finish scanning
    this->selfDefinedActions.append("toggle return readings"); // for tests, if set to false then OpenIndy responds with an incorrect measurement

    //set default accuracy
    this->defaultAccuracy.sigmaAzimuth = 0.000001570;
    this->defaultAccuracy.sigmaZenith = 0.000001570;
    this->defaultAccuracy.sigmaDistance = 0.000025;
    this->defaultAccuracy.sigmaXyz.setAt(0, 0.000025);
    this->defaultAccuracy.sigmaXyz.setAt(1, 0.000025);
    this->defaultAccuracy.sigmaXyz.setAt(2, 0.000025);
    this->defaultAccuracy.sigmaTemp = 0.5;
    this->defaultAccuracy.sigmaI = 0.000001570;
    this->defaultAccuracy.sigmaJ = 0.000001570;
    this->defaultAccuracy.sigmaK = 0.000001570;

    //general tracker inits
    this->myAzimuth = 0.00001;
    this->myZenith = 0.00001;
    this->myDistance =0.000001;
    this->myMotor = false;
    this->myInit = false;
    this->myCompIt = false;
    this->isConnected = false;
    this->side = 1;

    this->returnReading = true;

}

/*!
 * \brief PseudoTracker::doSelfDefinedAction
 * \param action
 * \return
 */
bool PseudoTracker::doSelfDefinedAction(const QString &action){
    qDebug() << "PseudoTracker::doSelfDefinedAction(): " << action;
    if(action == "echo"){
        emit this->sensorMessage(action, eInformationMessage, eMessageBoxMessage);
    } else if (action == "stopMeasure") {
        this->isScanning = false;
        emit this->sensorMessage("try to stop / finish measurement", eInformationMessage, eConsoleMessage);
    } else if(action == "toggle return readings") {
        this->returnReading = !this->returnReading; // toggle
        emit this->sensorMessage(QString("return readings: %1").arg(this->returnReading), eInformationMessage, eConsoleMessage);
    }
    return true;
}

/*!
 * \brief PseudoTracker::abortAction
 */
bool PseudoTracker::abortAction(){
    return false;
}

/*!
 * \brief PseudoTracker::connectSensor
 * \return
 */
bool PseudoTracker::connectSensor(){
    this->isConnected = true;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::disconnectSensor
 * \return
 */
bool PseudoTracker::disconnectSensor(){
    this->isConnected = false;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::initialize
 * \return
 */
bool PseudoTracker::initialize(){
    this->myInit = true;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::move
 * \param azimuth
 * \param zenith
 * \param distance
 * \param isRelative
 * \return
 */
bool PseudoTracker::move(const double &azimuth, const double &zenith, const double &distance, const bool &isRelative){
    this->myAzimuth = azimuth;
    this->myZenith = zenith;
    this->myDistance = distance;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::move
 * \param x
 * \param y
 * \param z
 * \return
 */
bool PseudoTracker::move(const double &x, const double &y, const double &z){
    this->myAzimuth = qAtan2(y,x);
    this->myDistance = qSqrt(x*x+y*y+z*z);
    this->myZenith = acos(z/myDistance);
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::home
 * \return
 */
bool PseudoTracker::home(){
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::changeMotorState
 * \return
 */
bool PseudoTracker::changeMotorState(){
    this->myMotor = !this->myMotor;
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::toggleSightOrientation
 * \return
 */
bool PseudoTracker::toggleSightOrientation(){
    if(this->side == 1){
        this->side = 2;
    }else{
        this->side = 1;
    }
    QThread::msleep(1000);
    return true;
}

/*!
 * \brief PseudoTracker::compensation
 * \return
 */
bool PseudoTracker::compensation() {
    QThread::msleep(5000);
    this->myCompIt = true;
    return true;
}

/*!
 * \brief PseudoTracker::measure
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measure(const MeasurementConfig &mConfig){

    QList<QPointer<Reading> > readings;

    if(!this->returnReading) {
        return readings;
    }

    const int faceCount = mConfig.getMeasureTwoSides() ? 2 : 1;

    int scanPointCount = mConfig.getMaxObservations();
    this->isScanning = mConfig.getMeasurementType() == MeasurementTypes::eScanDistanceDependent_MeasurementType;

    do {
        for(int face=0; face<faceCount; face++) {

            switch (mConfig.getTypeOfReading()) {
            case ePolarReading:{
                readings += measurePolar(mConfig);
                break;
            }case eDistanceReading:{
                readings += measureDistance(mConfig);
                break;
            }case eDirectionReading:{
                readings += measureDirection(mConfig);
                break;
            }case eCartesianReading:{
                readings += measureCartesian(mConfig);
                break;
            }
            }

            if(mConfig.getMeasureTwoSides() && face<(faceCount -1)) {
                this->toggleSightOrientation();
            }

        }
        qDebug()<< "isScanning: " << isScanning;

    } while(mConfig.getMeasurementType() == MeasurementTypes::eScanDistanceDependent_MeasurementType && scanPointCount-- > 1 && this->isScanning);
    this->isScanning = false;

    if(readings.size() > 0){

        //delete old last reading
        if(!this->lastReading.second.isNull()){
            delete this->lastReading.second;
        }

        this->lastReading.first = readings.last()->getTypeOfReading();
        QPointer<Reading> r = new Reading(*readings.last().data());
        this->lastReading.second = r;

    }

    return readings;

}

/*!
 * \brief PseudoTracker::readingStream
 * \param streamFormat
 * \return
 */
QVariantMap PseudoTracker::readingStream(const ReadingTypes &streamFormat){

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double distance = 0.0;
    double azimuth = 0.0;
    double zenith = 0.0;

    QVariantMap m;

    QPointer<Reading> r(NULL);

    switch (streamFormat) {
    case ePolarReading:{

        ReadingPolar rPolar;
        rPolar.azimuth = myAzimuth;
        rPolar.zenith = myZenith;
        rPolar.distance = myDistance;
        rPolar.isValid = true;

        this->noisyPolarReading(rPolar);

        r = new Reading(rPolar);

        m.insert("azimuth", rPolar.azimuth);
        m.insert("zenith", rPolar.zenith);
        m.insert("distance", rPolar.distance);

        break;

    }case eCartesianReading:{

        ReadingPolar rPolar;
        rPolar.azimuth = myAzimuth;
        rPolar.zenith = myZenith;
        rPolar.distance = myDistance;
        rPolar.isValid = true;

        this->noisyPolarReading(rPolar);

        r = new Reading(rPolar);

        m.insert("x", r->getCartesianReading().xyz.getAt(0));
        m.insert("y", r->getCartesianReading().xyz.getAt(1));
        m.insert("z", r->getCartesianReading().xyz.getAt(2));

        break;

    }case eDistanceReading:{

        ReadingDistance rDistance;
        rDistance.distance = this->myDistance;
        rDistance.isValid = true;

        r = new Reading(rDistance);

        m.insert("distance", r->getDistanceReading().distance);

        break;

    }case eDirectionReading:{

        ReadingDirection rDirection;
        rDirection.azimuth = this->myAzimuth;
        rDirection.zenith = this->myZenith;
        rDirection.isValid = true;

        r = new Reading(rDirection);

        m.insert("azimuth", r->getDirectionReading().azimuth);
        m.insert("zenith", r->getDirectionReading().zenith);

        break;

    }case eTemperatureReading:{
        break;
    }case eLevelReading:{
        break;
    }case eUndefinedReading:{
        break;
    }default:
        break;
    }

    //delete old last reading
    if(!this->lastReading.second.isNull()){
        delete this->lastReading.second;
    }

    //check reading
    if(r.isNull()){
        return m;
    }

    this->lastReading.first = r->getTypeOfReading();
    this->lastReading.second = r;

    QThread::msleep(300);

    return m;

}

/*!
 * \brief PseudoTracker::getConnectionState
 * \return
 */
bool PseudoTracker::getConnectionState(){
    return isConnected;
}

/*!
 * \brief PseudoTracker::isReadyForMeasurement
 * \return
 */
bool PseudoTracker::getIsReadyForMeasurement(){
    return true;
}

/*!
 * \brief PseudoTracker::getSensorStats
 * \return
 */
QMap<QString, QString> PseudoTracker::getSensorStatus(){

    QMap<QString, QString> stats;

    stats.insert("connected",QString::number(isConnected));
    stats.insert("side", QString::number(side));
    stats.insert("myAzimuth", QString::number(myAzimuth));
    stats.insert("myZenith", QString::number(myZenith));
    stats.insert("myDistance", QString::number(myDistance));
    stats.insert("myMotor", QString::number(myMotor));
    stats.insert("myInit", QString::number(myInit));
    stats.insert("myCompIt", QString::number(myCompIt));

    QThread::msleep(300);

    return stats;

}

/*!
 * \brief PseudoTracker::isBusy
 * \return
 */
bool PseudoTracker::getIsBusy(){
    return false;
}

/*!
 * \brief PseudoTracker::measurePolar
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measurePolar(const MeasurementConfig &mConfig){

    QList<QPointer<Reading> > readings;

    ReadingPolar rPolar;
    rPolar.azimuth = myAzimuth;
    rPolar.zenith = myZenith;
    rPolar.distance = myDistance;
    rPolar.sigmaAzimuth = this->sensorConfiguration.getAccuracy().sigmaAzimuth;
    rPolar.sigmaZenith= this->sensorConfiguration.getAccuracy().sigmaZenith;
    rPolar.sigmaDistance = this->sensorConfiguration.getAccuracy().sigmaDistance;
    rPolar.isValid = true;

    this->noisyPolarReading(rPolar);

    QPointer<Reading> p = new Reading(rPolar);

    p->setSensorFace((SensorFaces)(side -1)); // SensorFaces defined side between 0 and 1 but this class between 1 and 2
    p->setMeasuredAt(QDateTime::currentDateTime());

    QThread::msleep(1000);

    readings.append(p);

    return readings;

}

/*!
 * \brief PseudoTracker::measureDistance
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measureDistance(const MeasurementConfig &mConfig){

    QList<QPointer<Reading> > readings;

    ReadingDistance rDistance;
    double dd = ((double) rand()/RAND_MAX)*(20.0-1.0)+1.0;
    dd = dd/10000;
    rDistance.distance = myDistance + dd;
    rDistance.isValid = true;

    QPointer<Reading> p = new Reading(rDistance);

    p->setSensorFace((SensorFaces)(side -1)); // SensorFaces defined side between 0 and 1 but this class between 1 and 2
    p->setMeasuredAt(QDateTime::currentDateTime());

    QThread::msleep(1000);

    readings.append(p);

    return readings;

}

/*!
 * \brief PseudoTracker::measureDirection
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measureDirection(const MeasurementConfig &mConfig){

    QList<QPointer<Reading> > readings;

    ReadingDirection rDirection;
    double daz = ((double) rand()/RAND_MAX)*(10.0-1.0)+1.0;
    double dze = ((double) rand()/RAND_MAX)*(10.0-1.0)+1.0;
    daz = daz/1000;
    dze = dze/1000;
    rDirection.azimuth = myAzimuth + daz;
    rDirection.zenith = myAzimuth + dze;
    rDirection.isValid = true;

    QPointer<Reading> p = new Reading(rDirection);

    p->setSensorFace((SensorFaces)(side -1)); // SensorFaces defined side between 0 and 1 but this class between 1 and 2
    p->setMeasuredAt(QDateTime::currentDateTime());

    QThread::msleep(1000);

    readings.append(p);

    return readings;

}

/*!
 * \brief PseudoTracker::measureCartesian
 * \param mConfig
 * \return
 */
QList<QPointer<Reading> > PseudoTracker::measureCartesian(const MeasurementConfig &mConfig){

    QList<QPointer<Reading> > readings;

    ReadingCartesian rCartesian;
    double dx = ((double) rand()/RAND_MAX)*(30.0-1.0)+1.0;
    double dy = ((double) rand()/RAND_MAX)*(30.0-1.0)+1.0;
    double dz = ((double) rand()/RAND_MAX)*(30.0-1.0)+1.0;
    dx = dx/10000.0;
    dy = dy/10000.0;
    dz = dz/10000.0;
    rCartesian.xyz.setAt(0, (myDistance * qSin(myZenith) * qCos(myAzimuth))+dx);
    rCartesian.xyz.setAt(1, (myDistance * qSin(myZenith) * qSin(myAzimuth))+dy);
    rCartesian.xyz.setAt(2, (myDistance * qCos(myZenith))+dz);
    rCartesian.isValid = true;

    QPointer<Reading> p = new Reading(rCartesian);

    p->setSensorFace((SensorFaces)(side -1)); // SensorFaces defined side between 0 and 1 but this class between 1 and 2
    p->setMeasuredAt(QDateTime::currentDateTime());

    QThread::msleep(1000);

    readings.append(p);

    return readings;

}

/*!
 * \brief PseudoTracker::randomX
 * \param d
 * \param m
 * \param s
 * \return
 *
 *  This method generates a random number
 *  depending on the specified distribution.
 *
 *  d = method of distribution:
 *  0 : uniform distribution (m-s,m+s)
 *  1 : normal distributed around m (s=sigma)
 *  2 : triangular distribution around m (m-s,m+s)
 */
double PseudoTracker::randomX(int d, double m, double s)
{
    double rv = 0.0;


      switch(d)
      {
        case 0 :
          rv = 2.0*(double)rand()/(double)RAND_MAX-1.0;
          rv = m+s*rv;
          break;
        case 1 :
          rv = randomNorm();
          rv = m+s*rv;
          break;
        case 2 :
          rv = randomTriangular(m,m-s,m+s);
          break;
        default :
          rv = -2;
          break;
        }

      return(rv);
}

/*!
 * \brief PseudoTracker::randomNorm
 * \return
 *
 * This method generates a normally distributed random number.
 */
double PseudoTracker::randomNorm()
{
    static int   iset=0;
    static double gset;
    double rnum;
    double fac;
    double rsq;
    double v1,v2;

    if ( iset == 0 )
      {
        do {
      v1  = 2.0*(double)rand()/(double)RAND_MAX-1.0;
      v2  = 2.0*(double)rand()/(double)RAND_MAX-1.0;
      rsq = v1*v1+v2*v2;
        } while ( rsq >= 1.0 || rsq == 0.0);

        fac = sqrt(-2.0*log(rsq)/rsq);
        rnum = v1*fac;
        gset = v2*fac;
        iset = 1;
      }
    else
      {
        rnum = gset;
        iset = 0;
      }
    return rnum;
}

/*!
 * \brief PseudoTracker::randomTriangular
 * \param c
 * \param a
 * \param b
 * \return
 *
 *  This method generates a triangular distributed random number.
 */
double PseudoTracker::randomTriangular(double c, double a, double b)
{
    double U = (double) rand() / (double) RAND_MAX;
       double F = (c - a) / (b - a);
       if (U <= F)
          return a + sqrt(U * (b - a) * (c - a));
       else
           return b - sqrt((1 - U) * (b - a) * (b - c));
}

/*!
 * \brief PseudoTracker::noisyPolarReading
 * \param r
 *
 * This method noisy a given polar Reading using randomly generated numbers.
 * It is used the error model described by  Hughes B, Sun W, Forbes A, Lewis A
 * 2010 Determining laser tracker alignment errors using
 * a network measurement CMSC Journal Autumn 2010, 26-32
 *
 */
void PseudoTracker::noisyPolarReading(ReadingPolar &r){

    double lambda = this->sensorConfiguration.getDoubleParameter().value("lambda [mm]")/1000;
    double mu = this->sensorConfiguration.getDoubleParameter().value("mu");
    double ex = this->sensorConfiguration.getDoubleParameter().value("ex [mm]")/1000;
    double by = this->sensorConfiguration.getDoubleParameter().value("by [mm]")/1000;
    double bz = this->sensorConfiguration.getDoubleParameter().value("bz [mm]")/1000;
    double alpha = this->sensorConfiguration.getDoubleParameter().value("alpha [arc sec]")*(M_PI/648000.0);
    double gamma = this->sensorConfiguration.getDoubleParameter().value("gamma [arc sec]")*(M_PI/648000.0);
    double Aa1 = this->sensorConfiguration.getDoubleParameter().value("Aa1 [arc sec]")*(M_PI/648000.0);
    double Ba1 = this->sensorConfiguration.getDoubleParameter().value("Ba1 [arc sec]")*(M_PI/648000.0);
    double Aa2 = this->sensorConfiguration.getDoubleParameter().value("Aa2 [arc sec]")*(M_PI/648000.0);
    double Ba2 = this->sensorConfiguration.getDoubleParameter().value("Ba2 [arc sec]")*(M_PI/648000.0);
    double Ae0 = this->sensorConfiguration.getDoubleParameter().value("Ae0 [arc sec]")*(M_PI/648000.0);
    double Ae1 = this->sensorConfiguration.getDoubleParameter().value("Ae1 [arc sec]")*(M_PI/648000.0);
    double Be1 = this->sensorConfiguration.getDoubleParameter().value("Be1 [arc sec]")*(M_PI/648000.0);
    double Ae2 = this->sensorConfiguration.getDoubleParameter().value("Ae2 [arc sec]")*(M_PI/648000.0);
    double Be2 = this->sensorConfiguration.getDoubleParameter().value("Be2 [arc sec]")*(M_PI/648000.0);

    lambda = randomX(1,0,lambda);
    mu = randomX(1,0,mu);
    ex = randomX(1,0,ex);
    by = randomX(1,0,by);
    bz = randomX(1,0,bz);
    alpha = randomX(1,0,alpha);
    gamma = randomX(1,0,gamma);
    Aa1 = randomX(1,0,Aa1);
    Ba1 = randomX(1,0,Ba1);
    Aa2 = randomX(1,0,Aa2);
    Ba2 = randomX(1,0,Ba2);
    Ae0 = randomX(1,0,Ae0);
    Ae1 = randomX(1,0,Ae1);
    Be1 = randomX(1,0,Be1);
    Ae2 = randomX(1,0,Ae2);
    Be2 = randomX(1,0,Be2);

    double az = r.azimuth;
    double ze = r.zenith;
    double d = r.distance;

    d = (1+mu)*d+lambda;

    double azF1 = Aa1*cos(az) + Ba1*sin(az);
    double azF2 = Aa2*cos(2*az) + Ba2*sin(2*az);

    az = az+azF1+azF2;

    double zeF1 = Ae1*cos(ze) + Be1*sin(ze);
    double zeF2 = Ae2*cos(2*ze) + Be2*sin(2*ze);

    ze = ze+Ae0+zeF1+zeF2;

    OiVec ebb;
    ebb.add(-ex);
    ebb.add(by);
    ebb.add(bz);

    OiVec e00;
    e00.add(ex);
    e00.add(0.0);
    e00.add(0.0);

    OiVec xAxis(3);
    xAxis.setAt(0, 1.0);

    OiVec yAxis(3);
    yAxis.setAt(1, 1.0);

    OiVec zAxis(3);
    zAxis.setAt(2, 1.0);


    OiMat Rz_Azimuth = OiMat::getRotationMatrix(az, zAxis);
    OiMat Rx_alpha = OiMat::getRotationMatrix(alpha, xAxis);
    OiMat Ry_zenith = OiMat::getRotationMatrix(ze-(M_PI/2.0), yAxis);
    OiMat Rx_minusAlpha = OiMat::getRotationMatrix(-1.0*alpha, xAxis);
    OiMat Rz_gamma = OiMat::getRotationMatrix(gamma, zAxis);


    OiVec b(3);
    b = Rz_Azimuth*e00 + Rz_Azimuth*Rx_alpha*Ry_zenith*Rx_minusAlpha*ebb;;

    OiVec n(3);
    n = Rz_Azimuth*Rx_alpha*Ry_zenith*Rx_minusAlpha*Rz_gamma*xAxis;


    OiVec p(3);
    p = b+d*n;

    r.azimuth = qAtan2(p.getAt(1),p.getAt(0));
    r.distance = qSqrt(p.getAt(0)*p.getAt(0)+p.getAt(1)*p.getAt(1)+p.getAt(2)*p.getAt(2));
    r.zenith = acos(p.getAt(2)/r.distance);

}

bool PseudoTracker::search() {
    emit this->sensorMessage("search", eInformationMessage, eConsoleMessage);
    QThread::msleep(1000);
    return true;
}
