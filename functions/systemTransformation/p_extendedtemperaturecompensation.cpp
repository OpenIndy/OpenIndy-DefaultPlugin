#include "p_extendedtemperaturecompensation.h"

/*!
 * \brief ExtendedTemperatureCompensation::init
 */
void ExtendedTemperatureCompensation::init(){

    //set plugin meta data
    this->metaData.name = "9ParameterHelmertTransformation";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "jwa";
    this->metaData.description = QString("%1 %2").arg("This is a 9 parameter helmert transformation. You can calculate the")
            .arg("parameters by the given points (at least 3).");
    this->metaData.iid = "de.openIndy.plugin.function.systemTransformation.v001";

    //set needed elements
    NeededElement param1;
    param1.description = "Select points for calculating the transformation.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(eTrafoParamFeature);

}

/*!
 * \brief ExtendedTemperatureCompensation::exec
 * \param trafoParam
 * \return
 */
bool ExtendedTemperatureCompensation::exec(TrafoParam &trafoParam){

    this->protocol.clear();
    this->svdError = false;

    this->initPoints();

    if(this->locSystem.count() == this->refSystem.count() && this->locSystem.count() > 2){

        //get translation and rotation from loc to ref system, so you can transform later
        this->rotation = this->approxRotation();
        this->scale = this->approxScale(this->rotation);
        this->translation = this->approxTranslation(this->rotation, this->scale);

        if(this->locSystem.count()>3){
            return this->calc(trafoParam);

        }else{
            OiMat rot = this->getRotationMatrix(this->rotation);
            OiMat s = this->getScaleMatrix(this->scale);
            OiMat t = this->getTranslationMatrix(this->translation);

            trafoParam.setTransformationParameters(rot, t, s);
            return true;
        }

    }else{
        emit this->sendMessage("Not enough points to calculate transformation parameters");
        return false;
    }

}

/*!
 * \brief ExtendedTemperatureCompensation::mergeInputElements
 * \return
 */
bool ExtendedTemperatureCompensation::mergeInputElements(){
    return true;
}

/*!
 * \brief calc the trafo param using iterations
 * \param tp
 * \return
 */
bool ExtendedTemperatureCompensation::calc(TrafoParam &tp)
{

    //transform loc (start system) to "pseudo"-loc system
    //transformation with previosly approximated translation, rotation and scale
    this->preliminaryTransformation(); 

    //get standard deviation
    double sumVV = 0.0;

    for (int i = 0;i<this->locSystem.size();i++) {
        OiVec diffVec = this->refSystem.at(i)-this->locSystem.at(i);
        sumVV += diffVec.getAt(0)*diffVec.getAt(0);
        sumVV += diffVec.getAt(1)*diffVec.getAt(1);
        sumVV += diffVec.getAt(2)*diffVec.getAt(2);

    }

    Statistic statistic;
    statistic.setStdev(sqrt(sumVV/(3.0*this->locSystem.size()-9.0)));
    tp.setStatistic(statistic);

    //get rotation between pseudo loc and ref system
    OiVec tmpRotation = this->approxRotation();

    //get scale between pseudo loc and ref system (using temperature or distance)
    OiVec tmpScale = this->approxScale(tmpRotation);

    //get translation between pseudo loc and ref system
    OiVec tmpTranslation = this->approxTranslation(tmpRotation,tmpScale);

    //approximation
    OiVec x0(9);

    x0.setAt(0,tmpRotation.getAt(0));
    x0.setAt(1,tmpRotation.getAt(1));
    x0.setAt(2,tmpRotation.getAt(2));
    x0.setAt(3,tmpScale.getAt(0));
    x0.setAt(4,tmpScale.getAt(1));
    x0.setAt(5,tmpScale.getAt(2));
    x0.setAt(6,tmpTranslation.getAt(0));
    x0.setAt(7,tmpTranslation.getAt(1));
    x0.setAt(8,tmpTranslation.getAt(2));

    //observations
    OiVec l = this->fillLVector();

    OiVec l_diff;
    OiMat a;
    OiVec x;
    OiMat qxx;

    //A matrix
    a = this->fillAMatrix(x0);
    //normal equation
    OiMat n = a.t()*a;

    try{
        qxx = n.inv(); // try to calc the inverse
    }catch(runtime_error e){
       return false;
    }catch(logic_error e){
        return false;
    }

    //differential l vector
    OiVec l0 = this->fillL0Vector(x0);
    l_diff = l -l0;

    //right side
    OiVec c = a.t()*l_diff;

    //calc x
    x = qxx*c;
    x0 = x0+x;

    OiVec v = a * x - l_diff;
    OiVec vtv = v.t() * v;
    double s0_post = sqrt(vtv.getAt(0) / (3 * this->locSystem.length() - 9));
    OiMat sxx = s0_post * s0_post * qxx;

    //tp.getStatistic()->stdev = s0_post;


    //set the trafo parameters with the previously calulated values and the additional values from adjustment

    this->translation.setAt(0,this->translation.getAt(0)+x0.getAt(6));
    this->translation.setAt(1,this->translation.getAt(1)+x0.getAt(7));
    this->translation.setAt(2,this->translation.getAt(2)+x0.getAt(8));

    this->rotation.setAt(0,this->rotation.getAt(0)+x0.getAt(0));
    this->rotation.setAt(1,this->rotation.getAt(1)+x0.getAt(1));
    this->rotation.setAt(2,this->rotation.getAt(2)+x0.getAt(2));

    this->scale.setAt(0,this->scale.getAt(0)*x0.getAt(3));
    this->scale.setAt(1,this->scale.getAt(1)*x0.getAt(4));
    this->scale.setAt(2,this->scale.getAt(2)*x0.getAt(5));

    OiMat s = this->getScaleMatrix(this->scale);
    OiMat r = this->getRotationMatrix(this->rotation);
    OiMat t = this->getTranslationMatrix(this->translation);

    tp.setTransformationParameters(r,t,s);

    return true;
}

/*!
 * \brief ExtendedTemperatureCompensation::init fills local system and reference system vectors.
 */
void ExtendedTemperatureCompensation::initPoints(){

    //clear lists
    this->locSystem.clear();
    this->refSystem.clear();

    //get and check input points
    /*if(!this->inputElements.contains(0) || this->inputElements[0].size() < 3
            || !this->inputElements.contains(1) || this->inputElements[1].size() != this->inputElements[0].size()){
        return;
    }
    for(int i = 0; i < this->inputElements[0].size(); i++){
        if(this->inputElements[0].at(i).point.isNull() || this->inputElements[1].at(i).point.isNull()
                || this->inputElements[0].at(i).point->getFeatureName().compare(this->inputElements[1].at(i).point->getFeatureName()) != 0){
            this->setUseState(0, this->inputElements[0].at(i).point->getId(), false);
            this->setUseState(1, this->inputElements[1].at(i).point->getId(), false);
            continue;
        }
        this->setUseState(0, this->inputElements[0].at(i).point->getId(), true);
        this->setUseState(1, this->inputElements[1].at(i).point->getId(), true);
        this->locSystem.append(this->inputElements[0].at(i).point->getPosition().getVectorH());
        this->refSystem.append(this->inputElements[1].at(i).point->getPosition().getVectorH());
    }*/

    if(this->inputPointsStartSystem.size() >= 3 &&
            this->inputPointsStartSystem.size() == this->inputPointsDestinationSystem.size()){

        for(int i = 0; i < this->inputPointsStartSystem.size(); i++){

            this->locSystem.append(this->inputPointsStartSystem.at(i).getPosition().getVector());
            this->refSystem.append(this->inputPointsDestinationSystem.at(i).getPosition().getVector());

        }

    }

}

/*!
 * \brief approxTranslation calculates approximated translation of given local and reference system.
 * \param points
 * \return
 */
OiVec ExtendedTemperatureCompensation::approxTranslation(OiVec rot, OiVec s)
{
    //get rotation matrix of approx values
    OiMat r = this->getRotationMatrix(rot);

    //get scale matrix of approx valies
    OiMat scaleMat = this->scaleMatrix(s);

    //centroid point of reference system
    OiVec centroidPref = this->calcCentroidPoint(this->refSystem);
    centroidPref.add(1.0);

    //get centroid point of loc system
    OiVec centroidPloc = this->calcCentroidPoint(this->locSystem);
    centroidPloc.add(1.0);

    //rotate and scale local centroid point to reference system
    OiVec tmp = centroidPloc;
    OiVec st = scaleMat*tmp;
    OiVec rst = r *st;
    OiVec trans = centroidPref - rst;
    trans.setAt(3,1.0);

    return trans;
}

/*!
 * \brief approxRotation calculates the rotation between the given local and reference system
 * \param Translation t
 * \return
 */
OiVec ExtendedTemperatureCompensation::approxRotation()
{
    //calculate approximated rotation angles with Drixler algorithm
    //=> good approximation of preliminary transformation, so a rotation matrix with small angles can be used later at
    //adjusting the parameters iterativ

    vector<OiVec> centroidCoords;
    //get centroid coords
    centroidCoords.push_back(this->calcCentroidPoint(this->locSystem));
    centroidCoords.push_back(this->calcCentroidPoint(this->refSystem));
    if(centroidCoords.size() == 2){
        //centroid reduced coords
        vector<OiVec> locC = this->centroidReducedCoord(locSystem, centroidCoords.at(0));
        vector<OiVec> refC = this->centroidReducedCoord(refSystem, centroidCoords.at(1));
        vector<OiMat> modelMatrices = this->modelMatrix(locC, refC);
        OiMat n = this->normalEquationMatrix(modelMatrices);
        OiVec q = this->quaternion(n);
        if(!svdError){
            OiMat r = this->rotationMatrix(q);
            OiVec result = this->getRotationAngles(r);
            if(result.getAt(0) == -0.0){
                result.setAt(0,0.0);
            }
            if(result.getAt(1) == -0.0){
                result.setAt(1,0.0);
            }
            if(result.getAt(2) == -0.0){
                result.setAt(2,0.0);
            }
            return result;
        }
    }

}

/*!
 * \brief approxScale calculates the scale depending on temperature
 */
OiVec ExtendedTemperatureCompensation::approxScale(OiVec rot)
{
    OiVec s(4);

    //get scale from differces in distance components (x, y, z)
    //between two points in each system
    double sx = 0.0;
    double sy = 0.0;
    double sz = 0.0;

    //get the current rotation matrix
    OiMat rotMat;

    try{
       rotMat = this->getRotationMatrix(rot).inv(); // try to calc the inverse
    }catch(runtime_error e){
       return false;
    }catch(logic_error e){
        return false;
    }

    QList<OiVec> tmpRefList;

    //rotate the reference system to local system
    for(int i=0; i<this->refSystem.size(); i++){
        OiVec tmpRef = rotMat*this->refSystem.at(i);
        tmpRefList.append(tmpRef);
    }

    int countX = 0;
    int countY = 0;
    int countZ = 0;

    for(int i=1; i<this->locSystem.size(); i++){

        //get x y and z component of vector from point 0 to i in loc system
        double sxLoc = qFabs(locSystem.at(0).getAt(0)-locSystem.at(i).getAt(0));
        double syLoc = qFabs(locSystem.at(0).getAt(1)-locSystem.at(i).getAt(1));
        double szLoc = qFabs(locSystem.at(0).getAt(2)-locSystem.at(i).getAt(2));

        //get x y and z component of vector from point 0 to i in ref system
        double sxRef = qFabs(tmpRefList.at(0).getAt(0)-tmpRefList.at(i).getAt(0));
        double syRef = qFabs(tmpRefList.at(0).getAt(1)-tmpRefList.at(i).getAt(1));
        double szRef = qFabs(tmpRefList.at(0).getAt(2)-tmpRefList.at(i).getAt(2));

        //0.050m is criteria, because of noisy measurements
        //and no representative distances.
        if(sxLoc <= 0.050 || sxRef <= 0.050){

        }else{
            sx += (sxRef/sxLoc);
            countX ++;
        }

        if(syLoc <= 0.050 || syRef <=0.050){

        }else{
            sy += (syRef/syLoc);
            countY++;
        }

        if(szLoc <= 0.050 || szRef <= 0.050){

        }else{
            sz += (szRef/szLoc);
            countZ++;
        }
    }

    //if no scale could be calculated (noisy measurements),
    //set this scale component to 1.000000
    if(sx == 0.0){
        sx = 1.0;
    }else{
        sx = sx/(double)countX;
    }

    if(sy == 0.0){
        sy = 1.0;
    }else{
        sy = sy/(double)countY;
    }

    if(sz == 0.0){
        sz = 1.0;
    }else{
        sz = sz/(double)countZ;
    }

    s.setAt(0,sx);
    s.setAt(1,sy);
    s.setAt(2,sz);
    s.setAt(3,1.0);

    return s;
}

/*!
 * \brief calcCentroidPoint of given points
 * \param points
 * \return
 */
OiVec ExtendedTemperatureCompensation::calcCentroidPoint(QList<OiVec> points)
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    for(int i=0;i<points.size();i++){
        x += points.at(i).getAt(0);
        y += points.at(i).getAt(1);
        z += points.at(i).getAt(2);
    }
    x = x/points.size();
    y = y/points.size();
    z = z/points.size();

    OiVec centroidPoint(3);
    centroidPoint.setAt(0,x);
    centroidPoint.setAt(1,y);
    centroidPoint.setAt(2,z);

    return centroidPoint;
}

/*!
 * \brief fillLVector with observations
 * \return
 */
OiVec ExtendedTemperatureCompensation::fillLVector()
{
    OiVec l(this->locSystem.length()*3);
    for(int row=0;row<this->locSystem.length()*3;row++){
        if((row+1)%3 == 1){ //x observation
            l.setAt(row,this->locSystem.at(row/3).getAt(0));
        }
        if((row+1)%3 == 2){ // y observation
            l.setAt(row,this->locSystem.at(row/3).getAt(1));
        }
        if((row+1)%3 == 0){ // z observation
            l.setAt(row,this->locSystem.at(row/3).getAt(2));
        }
    }
    return l;
}

/*!
 * \brief fillAMatrix fills the A matrix
 * rotation x, rotation y, rotation z, scale x, scale y, scale z, tx, ty, tz
 * \param x0
 * \return
 */
OiMat ExtendedTemperatureCompensation::fillAMatrix(OiVec x0)
{
    OiMat a(locSystem.length()*3,9);

    //loc system is transformed with approx values of translation and rotation. Scale is always near 1.00000, so
    //this should be approxed enough for the pre transformation.
    //That´s why I can use the rotation matrix for small angles.
    for(int row=0; row<locSystem.length()*3; row++){
        if((row+1) % 3 == 1){ // x
            a.setAt(row,0,0.0);
            a.setAt(row,1,-locSystem.at(row/3).getAt(2)*(x0.getAt(3)));
            a.setAt(row,2,locSystem.at(row/3).getAt(1)*(x0.getAt(3)));
            a.setAt(row,3,(locSystem.at(row/3).getAt(0)+x0.getAt(2)*locSystem.at(row/3).getAt(1)-x0.getAt(1)*locSystem.at(row/3).getAt(2)));
            a.setAt(row,4,0.0);
            a.setAt(row,5,0.0);
            a.setAt(row,6,1.0);
            a.setAt(row,7,0.0);
            a.setAt(row,8,0.0);
        }else if((row+1) % 3 == 2){ //y
            a.setAt(row,0,locSystem.at(row/3).getAt(2)*(x0.getAt(4)));
            a.setAt(row,1,0.0);
            a.setAt(row,2,-locSystem.at(row/3).getAt(0)*(x0.getAt(4)));
            a.setAt(row,3,0.0);
            a.setAt(row,4,(-x0.getAt(2)*locSystem.at(row/3).getAt(0)+locSystem.at(row/3).getAt(1)+x0.getAt(0)*locSystem.at(row/3).getAt(2)));
            a.setAt(row,5,0.0);
            a.setAt(row,6,0.0);
            a.setAt(row,7,1.0);
            a.setAt(row,8,0.0);
        }else if((row+1) % 3 == 0){ //z
            a.setAt(row,0,-locSystem.at(row/3).getAt(1)*(x0.getAt(5)));
            a.setAt(row,1,locSystem.at(row/3).getAt(0)*(x0.getAt(5)));
            a.setAt(row,2,0.0);
            a.setAt(row,3,0.0);
            a.setAt(row,4,0.0);
            a.setAt(row,5,(x0.getAt(1)*locSystem.at(row/3).getAt(0)-x0.getAt(0)*locSystem.at(row/3).getAt(1)+locSystem.at(row/3).getAt(2)));
            a.setAt(row,6,0.0);
            a.setAt(row,7,0.0);
            a.setAt(row,8,1.0);
        }
    }
    return a;
}

/*!
 * \brief rotationMatrix sets up the rotation matrix
 * \return
 */
OiMat ExtendedTemperatureCompensation::getRotationMatrix(OiVec rot)
{
    OiMat result(4,4);
    double alpha = rot.getAt(0);
    double beta = rot.getAt(1);
    double gamma = rot.getAt(2);
    result.setAt(0,0,qCos(beta)*qCos(gamma));
    result.setAt(0,1,qCos(alpha)*qSin(gamma)+qSin(alpha)*qSin(beta)*qCos(gamma));
    result.setAt(0,2,qSin(alpha)*qSin(gamma)-qCos(alpha)*qSin(beta)*qCos(gamma));
    result.setAt(0,3,0.0);
    result.setAt(1,0,-qCos(beta)*qSin(gamma));
    result.setAt(1,1,qCos(alpha)*qCos(gamma)-qSin(alpha)*qSin(beta)*qSin(gamma));
    result.setAt(1,2,qSin(alpha)*qCos(gamma)+qCos(alpha)*qSin(beta)*qSin(gamma));
    result.setAt(1,3,0.0);
    result.setAt(2,0,qSin(beta));
    result.setAt(2,1,-qSin(alpha)*qCos(beta));
    result.setAt(2,2,qCos(alpha)*qCos(beta));
    result.setAt(2,3,0.0);
    result.setAt(3,0,0.0);
    result.setAt(3,1,0.0);
    result.setAt(3,2,0.0);
    result.setAt(3,3,1.0);

    return result;
}

/*!
 * \brief preliminaryTransformation calculates the first approxiamted transformation
 */
void ExtendedTemperatureCompensation::preliminaryTransformation()
{
    //get rotation matrix of current rotation angles
    OiMat rot = this->getRotationMatrix(this->rotation);

    //get scale matrix of current scales
    OiMat s = this->scaleMatrix(this->scale);

    QList<OiVec> tmpLoc;
    for(int i=0; i<this->locSystem.size();i++){
        //get vector point i
        OiVec tmp = this->locSystem.at(i);
        tmp.setAt(3,1.0);

        //scale the point
        OiVec st = s*tmp;
        //rotate the point
        OiVec rst = rot*st;

        //add translation
        OiVec tmptrafo = this->translation + rst;
        tmptrafo.setAt(3,1.0);
        tmpLoc.append(tmptrafo);
    }
    this->locSystem = tmpLoc;
}

/*!
 * \brief scaleMatrix generates a matrix with the 3 scales on the main diagonal
 * \return
 */
OiMat ExtendedTemperatureCompensation::scaleMatrix(OiVec s)
{
    OiMat scaleMat(4,4);
    scaleMat.setAt(0,0,s.getAt(0));
    scaleMat.setAt(0,1,0.0);
    scaleMat.setAt(0,2,0.0);
    scaleMat.setAt(0,3,0.0);
    scaleMat.setAt(1,0,0.0);
    scaleMat.setAt(1,1,s.getAt(1));
    scaleMat.setAt(1,2,0.0);
    scaleMat.setAt(1,3,0.0);
    scaleMat.setAt(2,0,0.0);
    scaleMat.setAt(2,1,0.0);
    scaleMat.setAt(2,2,s.getAt(2));
    scaleMat.setAt(2,3,0.0);
    scaleMat.setAt(3,0,0.0);
    scaleMat.setAt(3,1,0.0);
    scaleMat.setAt(3,2,0.0);
    scaleMat.setAt(3,3,1.0);

    return scaleMat;
}

/*!
 * \brief fillL0Vector fills the l0 vector
 * \param x0
 * \return
 */
OiVec ExtendedTemperatureCompensation::fillL0Vector(OiVec x0)
{
    OiVec l0(this->locSystem.length() * 3);

    double sx, sy, sz;

    sx = x0.getAt(3);
    sy = x0.getAt(4);
    sz = x0.getAt(5);

    for(int row = 0; row < this->locSystem.length() * 3; row++){
        if( (row+1) % 3 == 1 ){ //observation of x
            l0.setAt(row, sx * this->locSystem.at(row/3).getAt(0) + sx * x0.getAt(2) * this->locSystem.at(row/3).getAt(1) - sx * x0.getAt(1) * this->locSystem.at(row/3).getAt(2) + x0.getAt(6));
        }else if( (row+1) % 3 == 2 ){ //observation of y
            l0.setAt(row, sy * this->locSystem.at(row/3).getAt(1) - sy * x0.getAt(2) * this->locSystem.at(row/3).getAt(0) + sy * x0.getAt(0) * this->locSystem.at(row/3).getAt(2) + x0.getAt(7));
        }else if( (row+1) % 3 == 0 ){ //observation of z
            l0.setAt(row, sz * this->locSystem.at(row/3).getAt(2) + sz * x0.getAt(1) * this->locSystem.at(row/3).getAt(0) - sz * x0.getAt(0) * this->locSystem.at(row/3).getAt(1) + x0.getAt(8));
        }
    }
    return l0;
}

/*!
 * \brief centroidReducedCoord calculates the centroid reduced coordinates
 * \param input
 * \param centroid
 * \return
 */
vector<OiVec> ExtendedTemperatureCompensation::centroidReducedCoord(QList<OiVec> input, OiVec centroid)
{
    vector<OiVec> result;
    foreach (OiVec coord, input) {
       OiVec c;
       c.add(coord.getAt(0));
       c.add(coord.getAt(1));
       c.add(coord.getAt(2));
       OiVec rc;
       rc = c-centroid;
       result.push_back(rc);
    }

    return result;
}

/*!
 * \brief modelMatrix
 * \param locC
 * \param refC
 * \return
 */
vector<OiMat> ExtendedTemperatureCompensation::modelMatrix(vector<OiVec> locC, vector<OiVec> refC)
{
    vector<OiMat> result;
    if(locC.size() == refC.size()){
        for(int i = 0; i < locC.size(); i++){
            OiMat a(4, 4);
            a.setAt(0, 1, -( locC.at(i).getAt(0) - refC.at(i).getAt(0) ));
            a.setAt(0, 2, -( locC.at(i).getAt(1) - refC.at(i).getAt(1) ));
            a.setAt(0, 3, -( locC.at(i).getAt(2) - refC.at(i).getAt(2) ));
            a.setAt(1, 0, ( locC.at(i).getAt(0) - refC.at(i).getAt(0) ));
            a.setAt(1, 2, ( locC.at(i).getAt(2) + refC.at(i).getAt(2) ));
            a.setAt(1, 3, -( locC.at(i).getAt(1) + refC.at(i).getAt(1) ));
            a.setAt(2, 0, ( locC.at(i).getAt(1) - refC.at(i).getAt(1) ));
            a.setAt(2, 1, -( locC.at(i).getAt(2) + refC.at(i).getAt(2) ));
            a.setAt(2, 3, ( locC.at(i).getAt(0) + refC.at(i).getAt(0) ));
            a.setAt(3, 0, ( locC.at(i).getAt(2) - refC.at(i).getAt(2) ));
            a.setAt(3, 1, ( locC.at(i).getAt(1) + refC.at(i).getAt(1) ));
            a.setAt(3, 2, -( locC.at(i).getAt(0) + refC.at(i).getAt(0) ));
            result.push_back(a);
        }
    }
    return result;
}

/*!
 * \brief normalEquationMatrix
 * \param vecA
 * \return
 */
OiMat ExtendedTemperatureCompensation::normalEquationMatrix(vector<OiMat> vecA)
{
    OiMat result(4, 4);
    foreach(OiMat a, vecA){
        result +=  a.t() * a;
    }
    return result;
}

/*!
 * \brief quaternion; SVD to get the transformation vector (quaternion)
 * \param n
 * \return
 */
OiVec ExtendedTemperatureCompensation::quaternion(OiMat n)
{
    OiVec q(4);
    try{
        OiMat u(4,4);
        OiMat v(4,4);
        OiVec s(4);
        n.svd(u, s, v);
        u.getCol(q, 3);
    }catch(runtime_error& e){
        svdError = true;
    }
    return q;
}

/*!
 * \brief rotationMatrix
 * \param q
 * \return
 */
OiMat ExtendedTemperatureCompensation::rotationMatrix(OiVec q)
{
    OiMat result(3, 3);
    result.setAt(0, 0, q.getAt(0)*q.getAt(0) + q.getAt(1)*q.getAt(1) - q.getAt(2)*q.getAt(2) - q.getAt(3)*q.getAt(3));
    result.setAt(0, 1, 2*(q.getAt(1)*q.getAt(2) - q.getAt(0)*q.getAt(3)));
    result.setAt(0, 2, 2*(q.getAt(1)*q.getAt(3) + q.getAt(0)*q.getAt(2)));
    result.setAt(1, 0, 2*(q.getAt(1)*q.getAt(2) + q.getAt(0)*q.getAt(3)));
    result.setAt(1, 1, q.getAt(0)*q.getAt(0) - q.getAt(1)*q.getAt(1) + q.getAt(2)*q.getAt(2) - q.getAt(3)*q.getAt(3));
    result.setAt(1, 2, 2*(q.getAt(2)*q.getAt(3) - q.getAt(0)*q.getAt(1)));
    result.setAt(2, 0, 2*(q.getAt(3)*q.getAt(1) - q.getAt(0)*q.getAt(2)));
    result.setAt(2, 1, 2*(q.getAt(3)*q.getAt(2) + q.getAt(0)*q.getAt(1)));
    result.setAt(2, 2, q.getAt(0)*q.getAt(0) - q.getAt(1)*q.getAt(1) - q.getAt(2)*q.getAt(2) + q.getAt(3)*q.getAt(3));

    return result;
}

/*!
 * \brief getRotationAngles
 * \param r
 * \return
 */
OiVec ExtendedTemperatureCompensation::getRotationAngles(OiMat r)
{
    OiVec rot(3);
    rot.setAt(0, qAtan2(-r.getAt(2,1), r.getAt(2,2))); //alpha
    rot.setAt(1, qAsin(r.getAt(2,0))); //beta
    rot.setAt(2, qAtan2(-r.getAt(1,0), r.getAt(0,0))); //gamma
    if( qFabs(qCos(rot.getAt(1)) * qCos(rot.getAt(2))) - qFabs(r.getAt(0,0)) > 0.01 ){
        rot.setAt(1, PI - rot.getAt(1));
    }
    rot.add(1.0);

    return rot;
}

/*!
 * \brief getTranslationMatrix generates the homogeneous translation matrix
 * \param trans
 * \return
 */
OiMat ExtendedTemperatureCompensation::getTranslationMatrix(OiVec trans)
{
    OiMat tmpTranslation(4,4);

    tmpTranslation.setAt(0,0,1.0);
    tmpTranslation.setAt(0,1,0.0);
    tmpTranslation.setAt(0,2,0.0);
    tmpTranslation.setAt(0,3,trans.getAt(0));
    tmpTranslation.setAt(1,0,0.0);
    tmpTranslation.setAt(1,1,1.0);
    tmpTranslation.setAt(1,2,0.0);
    tmpTranslation.setAt(1,3,trans.getAt(1));
    tmpTranslation.setAt(2,0,0.0);
    tmpTranslation.setAt(2,1,0.0);
    tmpTranslation.setAt(2,2,1.0);
    tmpTranslation.setAt(2,3,trans.getAt(2));
    tmpTranslation.setAt(3,0,0.0);
    tmpTranslation.setAt(3,1,0.0);
    tmpTranslation.setAt(3,2,0.0);
    tmpTranslation.setAt(3,3,1.0);

    return tmpTranslation;
}

/*!
 * \brief getScaleMatrix generates the homogeneous scale matrix
 * \param s
 * \return
 */
OiMat ExtendedTemperatureCompensation::getScaleMatrix(OiVec s)
{
    OiMat tmpScale(4,4);

    tmpScale.setAt(0,0,s.getAt(0));
    tmpScale.setAt(0,1,0.0);
    tmpScale.setAt(0,2,0.0);
    tmpScale.setAt(0,3,0.0);
    tmpScale.setAt(1,0,0.0);
    tmpScale.setAt(1,1,s.getAt(1));
    tmpScale.setAt(1,2,0.0);
    tmpScale.setAt(1,3,0.0);
    tmpScale.setAt(2,0,0.0);
    tmpScale.setAt(2,1,0.0);
    tmpScale.setAt(2,2,s.getAt(2));
    tmpScale.setAt(2,3,0.0);
    tmpScale.setAt(3,0,0.0);
    tmpScale.setAt(3,1,0.0);
    tmpScale.setAt(3,2,0.0);
    tmpScale.setAt(3,3,1.0);

    return tmpScale;
}


