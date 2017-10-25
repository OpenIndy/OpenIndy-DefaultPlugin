#include "p_helmert7Param.h"

/*!
 * \brief Helmert7Param::init
 */
void Helmert7Param::init(){

    //set plugin meta data
    this->metaData.name = "HelmertTransformation";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra, jwa";
    this->metaData.description = QString("%1 %2")
            .arg("This function calculates a helmert transformation with or without scale.")
            .arg("That transformation is based on identical points in start and target system.");
    this->metaData.iid = SystemTransformation_iidd;

    //set needed elements
    NeededElement param1;
    param1.description = "Select points for calculating the transformation.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(eTrafoParamFeature);

    //scale type
    this->stringParameters.insert("calculate scale", "no");
    this->stringParameters.insert("calculate scale", "yes");

    this->stringParameters.insert("use temperature", "yes");
    this->stringParameters.insert("use temperature", "no");

    //temperatures
    this->doubleParameters.insert("reference", 20.0);
    this->doubleParameters.insert("actual", 20.0);

    //materials
    QList<QString> materials = getMaterials();

    for(int i = 0; i < materials.size(); i++){
        this->stringParameters.insert("material", materials.at(i));
    }
}

/*!
 * \brief Helmert7Param::exec
 * \param trafoParam
 * \return
 */
bool Helmert7Param::exec(TrafoParam &trafoParam){
    this->svdError = false;

    this->initPoints(); //fills the locSystem and refSystem vectors based on the given common points.

    this->getScaleType();

    if(locSystem.count() == refSystem.count() && locSystem.count() > 2){ //if enough common points available
        if(this->scaleType == pointScale){
            if(this->calc_7p(trafoParam)){
                if(locSystem.count() > 3){
                    return this->adjust_7p(trafoParam);
                }else if(locSystem.count() == 3){
                    return true;
                }
            }
        }else{
            return this->calc_6p(trafoParam);
        }
    }else{
        emit this->sendMessage("Not enough common points!", eWarningMessage);
    }
    return false;
}

/*!
 * \brief Helmert7Param::getScaleType
 * set scale type depending on settings
 */
void Helmert7Param::getScaleType()
{
    if(this->scalarInputParams.stringParameter.contains("calculate scale")){
        if(this->scalarInputParams.stringParameter.value("calculate scale").compare("yes") == 0){
            if(this->scalarInputParams.stringParameter.contains("use temperature")){
                if(this->scalarInputParams.stringParameter.value("use temperature").compare("no") == 0){
                    this->scaleType = pointScale;
                    return;
                }else{
                    this->scaleType = tempScale;
                    return;
                }
            }
        }else{
            this->scaleType = noScale;
            return;
        }
    }
}

/*!
 * \brief Helmert7Param::initPoints
 */
void Helmert7Param::initPoints(){

    //clear lists
    this->locSystem.clear();
    this->refSystem.clear();

    if(this->inputPointsStartSystem.size() >= 3 &&
            this->inputPointsStartSystem.size() == this->inputPointsDestinationSystem.size()){

        for(int i = 0; i < this->inputPointsStartSystem.size(); i++){

            this->locSystem.append(this->inputPointsStartSystem.at(i).getPosition().getVector());
            this->refSystem.append(this->inputPointsDestinationSystem.at(i).getPosition().getVector());
        }
    }
}

/*!
 * \brief Helmert7Param::calc
 * Calculate transformation parameter
 * \param tp
 * \return
 */
bool Helmert7Param::calc_7p(TrafoParam &tp){
    vector<OiVec> centroidCoords = this->calcCentroidCoord(); //centroid coordinates
    if(centroidCoords.size() == 2){
        vector<OiVec> locC = this->centroidReducedCoord(locSystem, centroidCoords.at(0)); //centroid reduced destination coordinates
        vector<OiVec> refC = this->centroidReducedCoord(refSystem, centroidCoords.at(1)); //centroid reduced target coordinates
        vector<OiMat> modelMatrices = this->modelMatrix(locC, refC); //vector of model matrices - one for each common point
        OiMat n = this->normalEquationMatrix(modelMatrices); //calculate the normal equation matrix
        OiVec q = this->quaternion(n);
        if( !svdError ){
            OiMat r = this->rotationMatrix(q); //fill rotation matrix
            this->fillTrafoParam(r, locC, refC, centroidCoords, tp); //fill trafo param object
            return true;
        }
    }
    return false;
}

/*!
 * \brief Helmert7Param::calcCentroidCoord
 * Calculate centroid coordinates for start and target system
 * \return
 */
vector<OiVec> Helmert7Param::calcCentroidCoord(){
    vector<OiVec> result;
    //centroid for destination system
    OiVec centroidLoc(3);
    foreach(OiVec loc, this->locSystem){
        OiVec coord;
        coord.add(loc.getAt(0));
        coord.add(loc.getAt(1));
        coord.add(loc.getAt(2));
        centroidLoc = centroidLoc + coord;
    }
    if(this->locSystem.count() > 0){ result.push_back( centroidLoc / this->locSystem.count() ); }
    //centroid for target system
    OiVec centroidRef(3);
    foreach(OiVec ref, this->refSystem){
        OiVec coord;
        coord.add(ref.getAt(0));
        coord.add(ref.getAt(1));
        coord.add(ref.getAt(2));
        centroidRef = centroidRef + coord;
    }
    if(refSystem.count() > 0){ result.push_back( centroidRef / this->refSystem.count() ); }
    return result;
}

/*!
 * \brief Helmert7Param::centroidReducedCoord
 * Calculates centroid reduced coordinates
 * \param input
 * \param centroid
 * \return
 */
vector<OiVec> Helmert7Param::centroidReducedCoord(QList<OiVec> input, OiVec centroid){
    vector<OiVec> result;
    foreach(OiVec coord, input){
        OiVec c;
        c.add(coord.getAt(0));
        c.add(coord.getAt(1));
        c.add(coord.getAt(2));
        OiVec rc = c - centroid;
        result.push_back( rc );
    }
    return result;
}

/*!
 * \brief Helmert7Param::modelMatrix
 * Returns a vector of model matrices. One model matrix for each common point.
 * \param locC
 * \param refC
 * \return
 */
vector<OiMat> Helmert7Param::modelMatrix(vector<OiVec> locC, vector<OiVec> refC){
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
 * \brief Helmert7Param::normalEquationMatrix
 * Calculate normal equation matrix
 * \param vecA
 * \return
 */
OiMat Helmert7Param::normalEquationMatrix(vector<OiMat> vecA){
    OiMat result(4, 4);
    foreach(OiMat a, vecA){
        result +=  a.t() * a;
    }
    return result;
}

/*!
 * \brief Helmert7Param::quaternion
 * SVD to get the transformation vector (quaternion)
 * \param n
 * \return
 */
OiVec Helmert7Param::quaternion(OiMat n){
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
 * \brief Helmert7Param::rotationMatrix
 * Set up the rotation matrix by using the quaternion q
 * \param q
 * \return
 */
OiMat Helmert7Param::rotationMatrix(OiVec q){
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
 * \brief Helmert7Param::fillTrafoParam
 * Fills trafoParam object
 * \param r
 * \param tp
 */
void Helmert7Param::fillTrafoParam(OiMat r, vector<OiVec> locC, vector<OiVec> refC, vector<OiVec> centroidCoords, TrafoParam &tp){
    //calc scale
    double o = 0.0;
    double u = 0.0;
    for(int i = 0; i < locC.size(); i++){
        OiVec vecO = refC.at(i).t() * r * locC.at(i);
        OiVec vecU = locC.at(i).t() * locC.at(i);
        o += vecO.getAt(0);
        u += vecU.getAt(0);
    }
    double m = 1.0;
    if(u > 0){ m = o / u; }
    //tp.setScale(m, m, m);
    //calc translation
    OiVec t = centroidCoords.at(1) - m * r * centroidCoords.at(0);
    //tp.setTranslation(t.getAt(0), t.getAt(1), t.getAt(2));
    //calc rotation
    OiVec rot(3);
    rot.setAt(0, qAtan2(-r.getAt(2,1), r.getAt(2,2))); //alpha
    rot.setAt(1, qAsin(r.getAt(2,0))); //beta
    rot.setAt(2, qAtan2(-r.getAt(1,0), r.getAt(0,0))); //gamma
    if( qFabs(qCos(rot.getAt(1)) * qCos(rot.getAt(2))) - qFabs(r.getAt(0,0)) > 0.01 ){
        rot.setAt(1, PI - rot.getAt(1));
    }
    //tp.setRotation(rot.getAt(0), rot.getAt(1), rot.getAt(2));
    //fill transformation matrix
    OiMat translation(4, 4);
    translation.setAt(0, 0, 1.0);
    translation.setAt(1, 1, 1.0);
    translation.setAt(2, 2, 1.0);
    translation.setAt(3, 3, 1.0);
    translation.setAt(0, 3, t.getAt(0));
    translation.setAt(1, 3, t.getAt(1));
    translation.setAt(2, 3, t.getAt(2));
    OiMat scale(4, 4);
    scale.setAt(0, 0, m);
    scale.setAt(1, 1, m);
    scale.setAt(2, 2, m);
    scale.setAt(3, 3, 1.0);
    OiMat rotation(4, 4);
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            rotation.setAt(i, j, r.getAt(i, j));
        }
    }
    rotation.setAt(3, 3, 1.0);
    tp.setTransformationParameters(rotation, translation, scale);

    double sumVV = 0.0;

    OiVec start(4);
    OiVec dest(4);
    for (int i = 0;i<this->locSystem.size();i++) {
        start.setAt(0, this->refSystem.at(i).getAt(0));
        start.setAt(1, this->refSystem.at(i).getAt(1));
        start.setAt(2, this->refSystem.at(i).getAt(2));
        start.setAt(3, 1.0);
        dest.setAt(0, this->locSystem.at(i).getAt(0));
        dest.setAt(1, this->locSystem.at(i).getAt(1));
        dest.setAt(2, this->locSystem.at(i).getAt(2));
        dest.setAt(3, 1.0);
        OiVec diffVec = start-(tp.getHomogenMatrix()*dest);
        sumVV += diffVec.getAt(0)*diffVec.getAt(0);
        sumVV += diffVec.getAt(1)*diffVec.getAt(1);
        sumVV += diffVec.getAt(2)*diffVec.getAt(2);

    }

    Statistic stats;
    stats.setStdev(sqrt(sumVV/(3.0*this->locSystem.size()-7.0)));
    tp.setStatistic(stats);
    //tp.generateHomogenMatrix();
}

/*!
 * \brief Helmert7Param::adjust
 * Transformation with small angles
 * \param tp
 * \return
 */
bool Helmert7Param::adjust_7p(TrafoParam &tp){
    bool result = false;

    //approximation
    OiVec x0(7);
    x0.setAt(0, tp.getRotation().getAt(0));
    x0.setAt(1, tp.getRotation().getAt(1));
    x0.setAt(2, tp.getRotation().getAt(2));
    x0.setAt(3, tp.getScale().getAt(0));
    x0.setAt(4, tp.getTranslation().getAt(0));
    x0.setAt(5, tp.getTranslation().getAt(1));
    x0.setAt(6, tp.getTranslation().getAt(2));

    //observations
    OiVec l = this->fillLVector();

    OiVec l_diff;
    OiMat a;
    OiVec x;
    OiMat qxx;

    //iterate till solution is ok
    int iterations = 0;
    OiVec stop;
    do{

        //A matrix
        a = this->fillAMatrix(x0);

        //normalequation --> trying to invert it
        OiMat n = a.t() * a;

        try{
            qxx = n.inv(); //try to calc the inverse
        }catch(runtime_error e){
            return false;
        }catch(logic_error e){
            return false;
        }

        //differential l vector
        OiVec l0 = this->fillL0Vector(x0);
        l_diff = l - l0;

        //right side
        OiVec c = a.t() * l_diff;

        //calc x
        x = qxx * c;
        x0 = x0 + x;

        stop = x.t() * x;
        iterations++;

    }while( stop.getAt(0) > (0.000001) || iterations < 100 ); //termination criterion

    //if(iterations < 100){ //adjustment successful
        OiVec v = a * x - l_diff;
        OiVec vtv = v.t() * v;
        double s0_post = sqrt(vtv.getAt(0) / (3.0 * this->locSystem.length() - 7.0));
        OiMat sxx = s0_post * s0_post * qxx;
        //tp.getStatistic()->stdev = s0_post;
        result = true;
    //}

    return result;
}

/*!
 * \brief Helmert7Param::fillAMatrix
 * Fills the A Matrix
 * x^T = alpha beta gamma m X0 Y0 Z0
 * \param x0
 * \return
 */
OiMat Helmert7Param::fillAMatrix(OiVec x0){
    OiMat a(locSystem.length() * 3, 7);
    for(int row = 0; row < locSystem.length() * 3; row++){
        if( (row+1) % 3 == 1 ){ //observation of x
            a.setAt(row, 0, 0);
            a.setAt(row, 1, -locSystem.at(row/3).getAt(2) * x0.getAt(3));
            a.setAt(row, 2, locSystem.at(row/3).getAt(1) * x0.getAt(3));
            a.setAt(row, 3, locSystem.at(row/3).getAt(0) + locSystem.at(row/3).getAt(1) * x0.getAt(2) - locSystem.at(row/3).getAt(2) * x0.getAt(1));
            a.setAt(row, 4, 1);
            a.setAt(row, 5, 0);
            a.setAt(row, 6, 0);
        }else if( (row+1) % 3 == 2 ){ //observation of y
            a.setAt(row, 0, locSystem.at(row/3).getAt(2) * x0.getAt(3));
            a.setAt(row, 1, 0);
            a.setAt(row, 2, -locSystem.at(row/3).getAt(0) * x0.getAt(3));
            a.setAt(row, 3, locSystem.at(row/3).getAt(1) - locSystem.at(row/3).getAt(0) * x0.getAt(2) + locSystem.at(row/3).getAt(2) * x0.getAt(0));
            a.setAt(row, 4, 0);
            a.setAt(row, 5, 1);
            a.setAt(row, 6, 0);
        }else if( (row+1) % 3 == 0 ){ //observation of z
            a.setAt(row, 0, -locSystem.at(row/3).getAt(1) * x0.getAt(3));
            a.setAt(row, 1, locSystem.at(row/3).getAt(0) * x0.getAt(3));
            a.setAt(row, 2, 0);
            a.setAt(row, 3, locSystem.at(row/3).getAt(2) + locSystem.at(row/3).getAt(0) * x0.getAt(1) - locSystem.at(row/3).getAt(1) * x0.getAt(0));
            a.setAt(row, 4, 0);
            a.setAt(row, 5, 0);
            a.setAt(row, 6, 1);
        }
    }
    return a;
}

/*!
 * \brief Helmert7Param::fillLVector
 * Fills the L Vector
 * \return
 */
OiVec Helmert7Param::fillLVector(){
    OiVec l(this->locSystem.length() * 3);
    for(int row = 0; row < this->locSystem.length() * 3; row++){
        if( (row+1) % 3 == 1 ){ //observation of x
            l.setAt(row, this->locSystem.at(row/3).getAt(0));
        }else if( (row+1) % 3 == 2 ){ //observation of y
            l.setAt(row, this->locSystem.at(row/3).getAt(1));
        }else if( (row+1) % 3 == 0 ){ //observation of z
            l.setAt(row, this->locSystem.at(row/3).getAt(2));
        }
    }
    return l;
}

/*!
 * \brief Helmert7Param::fillL0Vector
 * Fills the L0 Vector
 * \param x0
 * \return
 */
OiVec Helmert7Param::fillL0Vector(OiVec x0){
    OiVec l0(this->locSystem.length() * 3);
    for(int row = 0; row < this->locSystem.length() * 3; row++){
        if( (row+1) % 3 == 1 ){ //observation of x
            l0.setAt(row, x0.getAt(3) * this->locSystem.at(row/3).getAt(0) + x0.getAt(3) * x0.getAt(2) * this->locSystem.at(row/3).getAt(1) - x0.getAt(3) * x0.getAt(1) * this->locSystem.at(row/3).getAt(2) + x0.getAt(4));
        }else if( (row+1) % 3 == 2 ){ //observation of y
            l0.setAt(row, x0.getAt(3) * this->locSystem.at(row/3).getAt(1) - x0.getAt(3) * x0.getAt(2) * this->locSystem.at(row/3).getAt(0) + x0.getAt(3) * x0.getAt(0) * this->locSystem.at(row/3).getAt(2) + x0.getAt(5));
        }else if( (row+1) % 3 == 0 ){ //observation of z
            l0.setAt(row, x0.getAt(3) * this->locSystem.at(row/3).getAt(2) + x0.getAt(3) * x0.getAt(1) * this->locSystem.at(row/3).getAt(0) - x0.getAt(3) * x0.getAt(0) * this->locSystem.at(row/3).getAt(1) + x0.getAt(6));
        }
    }
    return l0;
}

/*!
 * \brief Helmert7Param::p6_adjust
 * \param tp
 * \return
 */
bool Helmert7Param::calc_6p(TrafoParam &tp)
{
    //adjust trafo param if enough points are given
    bool result = false;

    this->p6_rotation = this->p6_approxRotation();
    this->p6_translation = this->p6_approxTranslation(this->p6_rotation);

    //transform loc (start system) to "pseudo"-loc system
    //transformation with previosly approximated translation and rotation
    this->p6_preliminaryTransformation();

    //get standard deviation
    double sumVV = 0.0;

    for (int i = 0;i<this->locSystem.size();i++) {
        OiVec diffVec = this->refSystem.at(i)-this->locSystem.at(i);
        sumVV += diffVec.getAt(0)*diffVec.getAt(0);
        sumVV += diffVec.getAt(1)*diffVec.getAt(1);
        sumVV += diffVec.getAt(2)*diffVec.getAt(2);

    }

    Statistic statistic;
    statistic.setStdev(sqrt(sumVV/(3.0*this->locSystem.size()-6.0)));
    tp.setStatistic(statistic);

    //get new rotation and translation between pseudo-loc system and ref system
    OiVec tmpRotation = this->p6_approxRotation();
    OiVec tmpTranslation = this->p6_approxTranslation(tmpRotation);

    //approximation
    OiVec x0(6);
    x0.setAt(0, tmpRotation.getAt(0));
    x0.setAt(1, tmpRotation.getAt(1));
    x0.setAt(2, tmpRotation.getAt(2));
    x0.setAt(3, tmpTranslation.getAt(0));
    x0.setAt(4, tmpTranslation.getAt(1));
    x0.setAt(5, tmpTranslation.getAt(2));

    //observations
    OiVec l = this->p6_fillLVector();

    OiVec l_diff;
    OiMat a;
    OiVec x;
    OiMat qxx;

    //A matrix
    a = this->p6_fillAMatrix(x0);

    //normalequation --> trying to invert it
    OiMat n = a.t() * a;

    try{
        qxx = n.inv(); //try to calc the inverse
    }catch(runtime_error e){
        return false;
    }catch(logic_error e){
        return false;
    }

    //differential l vector
    OiVec l0 = this->p6_fillL0Vector(x0);
    l_diff = l - l0;

    //right side
    OiVec c = a.t() * l_diff;

    //calc x
    x = qxx * c;
    x0 = x0 + x;

    OiVec v = a * x - l_diff;
    OiVec vtv = v.t() * v;
    double s0_post = sqrt(vtv.getAt(0) / (3 * this->locSystem.length() - 6));
    OiMat sxx = s0_post * s0_post * qxx;

    //tp.getStatistic()->stdev = s0_post;

    //set the trafo parameters with the previously calulated values and the additional values from adjustment

    //get scale from temperature or set scale to 1.0
    double scaleValue = this->setScaleValue();
    OiVec scale(4);
    scale.setAt(0,scaleValue);
    scale.setAt(1,scaleValue);
    scale.setAt(2,scaleValue);
    scale.setAt(3,1.0);

    this->p6_translation.setAt(0,this->p6_translation.getAt(0)+x0.getAt(3));
    this->p6_translation.setAt(1,this->p6_translation.getAt(1)+x0.getAt(4));
    this->p6_translation.setAt(2,this->p6_translation.getAt(2)+x0.getAt(5));

    this->p6_rotation.setAt(0,this->p6_rotation.getAt(0)+x0.getAt(0));
    this->p6_rotation.setAt(1,this->p6_rotation.getAt(1)+x0.getAt(1));
    this->p6_rotation.setAt(2,this->p6_rotation.getAt(2)+x0.getAt(2));

    OiMat r = this->p6_getRotationMatrix(this->p6_rotation);
    OiMat t = this->p6_getTranslationMatrix(this->p6_translation);
    OiMat s = this->p6_getScaleMatrix(scale);

    tp.setTransformationParameters(r,t,s);

    result = true;

    return result;
}

/*!
 * \brief Helmert7Param::p6_fillAMatrix
 * \param x0
 * \return
 */
OiMat Helmert7Param::p6_fillAMatrix(OiVec x0)
{
    OiMat a(locSystem.length()*3,6);

    //loc system is transformed with approx values of translation and rotation, so
    //this should be approxed enough for the pre transformation.
    //That´s why I can use the rotation matrix for small angles.
    for(int row=0; row<locSystem.length()*3; row++){
        if((row+1) % 3 == 1){ // x
            a.setAt(row,0,0.0);
            a.setAt(row,1,-locSystem.at(row/3).getAt(2));
            a.setAt(row,2,locSystem.at(row/3).getAt(1));
            a.setAt(row,3,1.0);
            a.setAt(row,4,0.0);
            a.setAt(row,5,0.0);
        }else if((row+1) % 3 == 2){ //y
            a.setAt(row,0,locSystem.at(row/3).getAt(2));
            a.setAt(row,1,0.0);
            a.setAt(row,2,-locSystem.at(row/3).getAt(0));
            a.setAt(row,3,0.0);
            a.setAt(row,4,1.0);
            a.setAt(row,5,0.0);
        }else if((row+1) % 3 == 0){ //z
            a.setAt(row,0,-locSystem.at(row/3).getAt(1));
            a.setAt(row,1,locSystem.at(row/3).getAt(0));
            a.setAt(row,2,0.0);
            a.setAt(row,3,0.0);
            a.setAt(row,4,0.0);
            a.setAt(row,5,1.0);
        }
    }
    return a;
}

/*!
 * \brief Helmert7Param::p6_fillLVector
 * \return
 */
OiVec Helmert7Param::p6_fillLVector()
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
 * \brief Helmert7Param::p6_fillL0Vector
 * \param x0
 * \return
 */
OiVec Helmert7Param::p6_fillL0Vector(OiVec x0)
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
 * \brief Helmert7Param::p6_preliminaryTransformation
 */
void Helmert7Param::p6_preliminaryTransformation()
{
    //get rotation matrix of current rotation angles
    OiMat rot = this->p6_getRotationMatrix(this->p6_rotation);

    this->locSystem = this->extendVector(this->locSystem);
    this->refSystem = this->extendVector(this->refSystem);

    QList<OiVec> tmpLoc;
    for(int i=0; i<this->locSystem.size();i++){
        //get vector point i
        OiVec tmp = this->locSystem.at(i);

        //rotate the point
        OiVec rt = rot*tmp;

        //add translation
        OiVec tmptrafo = this->p6_translation + rt;
        tmptrafo.setAt(3,1.0);
        tmpLoc.append(tmptrafo);
    }
    this->locSystem = tmpLoc;
}

/*!
 * \brief Helmert7Param::p6_approxRotation
 * \return
 */
OiVec Helmert7Param::p6_approxRotation()
{
    vector<OiVec> centroidCoords = this->calcCentroidCoord(); //centroid coordinates

    if(centroidCoords.size() == 2){
        vector<OiVec> locC = this->centroidReducedCoord(locSystem, centroidCoords.at(0)); //centroid reduced destination coordinates
        vector<OiVec> refC = this->centroidReducedCoord(refSystem, centroidCoords.at(1)); //centroid reduced target coordinates
        vector<OiMat> modelMatrices = this->modelMatrix(locC, refC); //vector of model matrices - one for each common point
        OiMat n = this->normalEquationMatrix(modelMatrices); //calculate the normal equation matrix
        OiVec q = this->quaternion(n);
        if( !svdError ){
            OiMat r = this->rotationMatrix(q); //fill rotation matrix
            OiVec result = this->p6_getRotationAngles(r);
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
 * \brief Helmert7Param::p6_approxTranslation
 * \param rot
 * \return
 */
OiVec Helmert7Param::p6_approxTranslation(OiVec rot)
{
    //get rotation matrix of approx values
    OiMat r = this->p6_getRotationMatrix(rot);

    vector<OiVec> centroidCoords = this->calcCentroidCoord(); //centroid coordinates

    //centroid point of reference system
    OiVec centroidPref = centroidCoords.at(1);
    centroidPref.add(1.0);

    //get centroid point of loc system
    OiVec centroidPloc = centroidCoords.at(0);
    centroidPloc.add(1.0);

    //rotate local centroid point to reference system
    OiVec tmp = centroidPloc;
    OiVec rt = r *tmp;
    OiVec trans = centroidPref - rt;
    trans.setAt(3,1.0);

    return trans;
}

/*!
 * \brief Helmert7Param::p6_getRotationMatrix
 * \param rot
 * \return
 */
OiMat Helmert7Param::p6_getRotationMatrix(OiVec rot)
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
 * \brief Helmert7Param::p6_getRotationAngles
 * \param r
 * \return
 */
OiVec Helmert7Param::p6_getRotationAngles(OiMat r)
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
 * \brief Helmert7Param::p6_getTranslationMatrix
 * \param trans
 * \return
 */
OiMat Helmert7Param::p6_getTranslationMatrix(OiVec trans)
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
 * \brief Helmert7Param::p6_getScaleMatrix
 * \param s
 * \return
 */
OiMat Helmert7Param::p6_getScaleMatrix(OiVec s)
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

/*!
 * \brief Helmert7Param::setScaleValue
 * \return
 */
double Helmert7Param::setScaleValue()
{
    double scale = 1.0;

    if(this->scaleType == noScale){
        scale = 1.0;
    }else{
        double act = this->scalarInputParams.doubleParameter.value("actual");
        double ref = this->scalarInputParams.doubleParameter.value("reference");
        QString material = this->scalarInputParams.stringParameter.value("material");

        return oi::getTemperatureExpansion(material, act, ref);
    }
    return scale;
}

/*!
 * \brief Helmert7Param::extendVector
 * \param vec
 */
QList<OiVec> Helmert7Param::extendVector(QList<OiVec> vec)
{
    QList<OiVec> result;

    for(int i=0; i<vec.size();i++){
        OiVec v = vec.at(i);
        v.add(1.0);
        result.append(v);
    }
    return result;
}
