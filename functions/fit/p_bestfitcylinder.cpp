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

}

/*!
 * \brief BestFitCylinder::exec
 * \param cylinder
 * \return
 */
bool BestFitCylinder::exec(Cylinder &cylinder){
    this->statistic.reset();
    return this->setUpResult(cylinder);
}

/*!
 * \brief BestFitCylinder::setUpResult
 * \param cylinder
 * \return
 */
bool BestFitCylinder::setUpResult(Cylinder &cylinder){

    //get and check input observations
    if(!this->inputElements.contains(0) || this->inputElements[0].size() < 5){
        emit this->sendMessage(QString("Not enough valid observations to fit the cylinder %1").arg(cylinder.getFeatureName()), eWarningMessage);
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
    if(inputObservations.size() < 5){
        emit this->sendMessage(QString("Not enough valid observations to fit the cylinder %1").arg(cylinder.getFeatureName()), eWarningMessage);
        return false;
    }

    //calculate centroid of all observations
    OiVec centroid(4);
    foreach(const QPointer<Observation> &obs, inputObservations){
        centroid = centroid + obs->getXYZ();
    }
    centroid = centroid / (double)inputObservations.size();

    //calculate centroid reduced observations
    QList<QPointer<Observation> > reducedInputObservations;
    foreach(const QPointer<Observation> &obs, inputObservations){
        Observation *rObs = new Observation(*obs.data());
        rObs->setXYZ(obs->getXYZ() - centroid);
        reducedInputObservations.append(rObs);
    }

    //approximate cylinder by 2D circle fit
    if(!this->approximateCylinder(cylinder, reducedInputObservations)){
        emit this->sendMessage(QString("Error while generating approximations for cylinder parameters of cylinder %1").arg(cylinder.getFeatureName()), eErrorMessage);
        return false;
    }

    //fit the cylinder using the previously generated approximations
    int bestSolution = -1;
    double stdev = numeric_limits<double>::max();
    for(int i = 0; i < this->approximations.size(); i++){

        if(approximations[i].stdev < stdev){
            bestSolution = i;
            stdev = stdev;
        }

        bool success = this->fitCylinder(cylinder, reducedInputObservations, this->approximations[i]);
        if(success && cylinder.getStatistic().getStdev() < stdev){
            bestSolution = i;
            stdev = cylinder.getStatistic().getStdev();
        }
    }
    if(bestSolution < 0){
        emit this->sendMessage(QString("Error while fitting cylinder %1").arg(cylinder.getFeatureName()), eErrorMessage);
        return false;
    }



    OiMat Ralpha(4,4);
    OiMat Rbeta(4,4);
    OiMat Rall(4,4);

    Ralpha.setAt(0, 0, 1.0);
    Ralpha.setAt(1, 1, qCos(approximations[bestSolution].approxAlpha));
    Ralpha.setAt(1, 2, -qSin(approximations[bestSolution].approxAlpha));
    Ralpha.setAt(2, 1, qSin(approximations[bestSolution].approxAlpha));
    Ralpha.setAt(2, 2, qCos(approximations[bestSolution].approxAlpha));
    Rbeta.setAt(0, 0, qCos(approximations[bestSolution].approxBeta));
    Rbeta.setAt(0, 2, qSin(approximations[bestSolution].approxBeta));
    Rbeta.setAt(1, 1, 1.0);
    Rbeta.setAt(2, 0, -qSin(approximations[bestSolution].approxBeta));
    Rbeta.setAt(2, 2, qCos(approximations[bestSolution].approxBeta));

    Rall = Rbeta * Ralpha;
    Rall.setAt(3,3,1.0);

    OiVec tmp;
    foreach(const QPointer<Observation> &obs, reducedInputObservations){
        tmp = Rall * obs->getXYZ();
        tmp.setAt(3, 1.0);
        obs->setXYZ(tmp);
    }

/*
    //approximate cylinder by 2D circle fit
    if(!this->approximateCylinder(cylinder, reducedInputObservations)){
        emit this->sendMessage(QString("Error while generating approximations for cylinder parameters of cylinder %1").arg(cylinder.getFeatureName()), eErrorMessage);
        return false;
    }*/

    approximations[bestSolution].approxAlpha = 0.0;
    approximations[bestSolution].approxBeta = 0.0;




    if(!this->fitCylinder(cylinder, reducedInputObservations, this->approximations[bestSolution])){
        return false;
    }


    /*if(!this->fitCylinder(cylinder, inputObservations)){
        emit this->sendMessage(QString("Error while fitting cylinder %1").arg(cylinder.getFeatureName()), eErrorMessage);
        return false;
    }*/

    //transform cylinder position (centroid + rotation)
    Position cylinderPosition = cylinder.getPosition();
    OiVec pos = (Rall.inv() * cylinderPosition.getVectorH()) + centroid;
    pos.setAt(3, 1.0);
    cylinderPosition.setVector(pos);

    //transform cylinder axis (centroid + rotation)
    Direction cylinderAxis = cylinder.getDirection();
    OiVec axis = (Rall.inv() * cylinderAxis.getVectorH());
    axis.removeLast();
    axis.normalize();
    cylinderAxis.setVector(axis);

    //check that the direction vector is defined by the first two observations
    OiVec pos1 = inputObservations.at(0)->getXYZ();
    pos1.removeLast();
    OiVec pos2 = inputObservations.at(1)->getXYZ();
    pos2.removeLast();
    OiVec direction = pos2 - pos1;
    direction.normalize();
    double angle = 0.0; //angle between r and direction
    OiVec::dot(angle, axis, direction);
    angle = qAbs(qAcos(angle));
    if(angle > (PI/2.0)){
        Direction cylinderDirection = cylinder.getDirection();
        cylinderDirection.setVector(axis * -1.0);
        cylinder.setCylinder(cylinderPosition, cylinderDirection, cylinder.getRadius());
    }else{
        Direction cylinderDirection = cylinder.getDirection();
        cylinderDirection.setVector(axis);
        cylinder.setCylinder(cylinderPosition, cylinderDirection, cylinder.getRadius());
    }

    return true;

}

/*!
 * \brief BestFitCylinder::approximateCylinder
 * \param cylinder
 * \param inputObservations
 * \return
 */
bool BestFitCylinder::approximateCylinder(Cylinder &cylinder, const QList<QPointer<Observation> > &inputObservations){

    //clear current approximations
    this->approximations.clear();

    //get the number of observations
    int numPoints = inputObservations.size();

    //calculate centroid of all observations
    OiVec centroid3D(4);
    foreach(const QPointer<Observation> &obs, inputObservations){
        centroid3D = centroid3D + obs->getXYZ();
    }
    centroid3D.removeLast();
    centroid3D = centroid3D / numPoints;

    //set up covariance matrix of all observations
    OiMat H(3, 3);
    for (int k = 0; k < numPoints; k++) {
        QPointer<Observation> obs = inputObservations.at(k);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                double a = 0.0, b = 0.0;
                if (i == 0)
                    a = obs->getXYZ().getAt(0) - centroid3D.getAt(0);
                else if (i == 1)
                    a = obs->getXYZ().getAt(1) - centroid3D.getAt(1);
                else
                    a = obs->getXYZ().getAt(2) - centroid3D.getAt(2);
                if (j == 0)
                    b = obs->getXYZ().getAt(0) - centroid3D.getAt(0);
                else if (j == 1)
                    b = obs->getXYZ().getAt(1) - centroid3D.getAt(1);
                else
                    b = obs->getXYZ().getAt(2) - centroid3D.getAt(2);
                H.setAt(i,j, H.getAt(i,j) + a * b);
            }
        }
    }

    //singular value decomposition of H to get the major axis direction of the cylinder observations
    OiMat U(3, 3);
    OiVec d(3);
    OiMat V(3, 3);
    try{
        H.svd(U, d, V);
    }catch(const exception &e){
        emit this->sendMessage(QString("SVD error cylinder minimum solution: %1").arg(e.what()), eErrorMessage);
        return false;
    }

    //init helper variables
    OiVec centroid2D(2); //centroid of 2D circle
    double alpha_n = 0.0, beta_n = 0.0, radius_n = 0.0, x_m_n = 0.0, y_m_n = 0.0; //final solutions of cylinder approximation
    double vv_ref = numeric_limits<double>::max(); //smallest vv that has been found
    OiVec pn; //possible normal vector
    double a = 0.0, b = 0.0; //sin + cos of rotation angles
    double a_alpha = 0.0, b_alpha = 0.0, a_beta = 0.0, b_beta = 0.0; //possible rotation angles (check acos + asin)
    double alpha = 0.0, beta = 0.0; //rotation angles
    double _y = 0.0, _z = 0.0;
    double tx = 0.0, ty = 0.0; //transformed 2D coordinates
    double x_m = 0.0, y_m = 0.0, radius = 0.0, sum_vv = 0.0; //result parameters in circle fit
    OiVec v;

    //rotation matrices
    OiMat Ralpha(3, 3);
    OiMat Rbeta(3, 3);
    OiMat Rall(3, 3);

    //a matrices for circle adjustment
    OiVec A1(numPoints);
    OiMat A2(numPoints,3);

    //result vector for circle fit
    OiVec s(3);

    //rotated 2D points
    QList<float> points2D_x;
    QList<float> points2D_y;

    //rotated centroid reduced 2D points
    QList<float> centroidreduced2D_x;
    QList<float> centroidreduced2D_y;

    //one of the eigen-vectors is the approximate cylinder axis
    for(int i = 0; i < 3; i++){

        U.getCol(pn, i); //get eigenvector i

        //calculate rotation angles
        a = qSqrt(1.0 / (1.0 + (pn.getAt(2)/pn.getAt(1))*(pn.getAt(2)/pn.getAt(1))));
        b = a * pn.getAt(2) / pn.getAt(1);

        b_alpha = qAcos(b);
        a_alpha = qAsin(a);

        alpha = 0.0;

        //cos and sin are ambiguous, therefor check the right angle here
        if(this->compareAngles(b_alpha, a_alpha)){
            alpha = a_alpha;
        }else if(this->compareAngles(this->getCorrespondingCos(b_alpha), a_alpha)){
            alpha = a_alpha;
        }else if(this->compareAngles(b_alpha, this->getCorrespondingSin(a_alpha))){
            alpha = b_alpha;
        }else{
            alpha = this->getCorrespondingCos(b_alpha);
        }

        //transform eigenvector --> y-component should be 0 now
        _y = pn.getAt(1);
        _z = pn.getAt(2);
        pn.setAt(1, _y * qCos(alpha) - _z * qSin(alpha));
        pn.setAt(2, _y * qSin(alpha) + _z * qCos(alpha));

        a = 0.0, b = 0.0;
        a = qSqrt(1.0 / (1.0 + (-1.0 * pn.getAt(2) / pn.getAt(0))*(-1.0 * pn.getAt(2) / pn.getAt(0))));
        b = -1.0 * pn.getAt(2) * a / pn.getAt(0);

        b_beta = qAcos(b);
        a_beta = qAsin(a);

        beta = 0.0;

        //cos and sin are ambiguous, therefor check the right angle here
        if(this->compareAngles(b_beta, a_beta)){
            beta = b_beta;
        }else if(this->compareAngles(this->getCorrespondingCos(b_beta), a_beta)){
            beta = a_beta;
        }else if(this->compareAngles(b_beta, this->getCorrespondingSin(a_beta))){
            beta = b_beta;
        }else{
            beta = this->getCorrespondingCos(b_beta);
        }

        //set up rotation matrices
        Ralpha.setAt(0, 0, 1.0);
        Ralpha.setAt(1, 1, qCos(alpha));
        Ralpha.setAt(1, 2, -qSin(alpha));
        Ralpha.setAt(2, 1, qSin(alpha));
        Ralpha.setAt(2, 2, qCos(alpha));
        Rbeta.setAt(0, 0, qCos(beta));
        Rbeta.setAt(0, 2, qSin(beta));
        Rbeta.setAt(1, 1, 1.0);
        Rbeta.setAt(2, 0, -qSin(beta));
        Rbeta.setAt(2, 2, qCos(beta));

        Rall = Rbeta * Ralpha;

        //###########################################
        //circle fit to determine midpoint and radius
        //###########################################

        //clear transformed point lists
        points2D_x.clear();
        points2D_y.clear();
        centroidreduced2D_x.clear();
        centroidreduced2D_y.clear();

        //reset 2D centroid and recalculate with new rotation
        centroid2D = OiVec(2);

        //rotate observations in XY-plane and calculate 2D centroid
        for(int j = 0; j < numPoints; j++){

            QPointer<Observation> obs = inputObservations.at(j);

            tx = Rall.getAt(0,0)*obs->getXYZ().getAt(0)
                    + Rall.getAt(0,1)*obs->getXYZ().getAt(1)
                    + Rall.getAt(0,2)*obs->getXYZ().getAt(2);
            ty = Rall.getAt(1,0)*obs->getXYZ().getAt(0)
                    + Rall.getAt(1,1)*obs->getXYZ().getAt(1)
                    + Rall.getAt(1,2)*obs->getXYZ().getAt(2);

            points2D_x.append( tx );
            points2D_y.append( ty );

            centroid2D.setAt(0, centroid2D.getAt(0) + tx);
            centroid2D.setAt(1, centroid2D.getAt(1) + ty);

        }
        centroid2D.setAt(0, centroid2D.getAt(0) / (float)numPoints);
        centroid2D.setAt(1, centroid2D.getAt(1) / (float)numPoints);

        //calculate centroid reduced 2D coordinates
        for(int j = 0; j < numPoints; j++){
            centroidreduced2D_x.append( points2D_x.at(j) - centroid2D.getAt(0) );
            centroidreduced2D_y.append( points2D_y.at(j) - centroid2D.getAt(1) );
        }

        //fill a matrices
        for(int j = 0; j < numPoints; j++){
            tx = centroidreduced2D_x.at(j);
            ty = centroidreduced2D_y.at(j);

            A1.setAt(j, tx*tx + ty*ty);

            A2.setAt(j,0,tx);
            A2.setAt(j,1,ty);
            A2.setAt(j,2,1.0);
        }

        //solve equation system to get circle parameters
        try{
            if(!OiMat::solve(s, A2.t() * A2, -1.0 * A2.t() * A1)){
                return false;
            }
        }catch(const exception &e){
            emit this->sendMessage(QString("inv error cylinder minimum solution: %1").arg(e.what()), eErrorMessage);
            return false;
        }

        //calculate midpoint + radius
        x_m = (-1.0 * s.getAt(0) / 2.0) + centroid2D.getAt(0);
        y_m = (-1.0 * s.getAt(1) / 2.0) + centroid2D.getAt(1);
        radius = qSqrt(0.25 * (s.getAt(0) * s.getAt(0) + s.getAt(1) * s.getAt(1)) - s.getAt(2));

        //calculate statistic
        v = -1.0 * A1 - A2 * s;
        OiVec::dot(sum_vv, v,v);
        sum_vv = qSqrt(sum_vv / (numPoints-3.0));

        //add approximation
        CylinderApproximation approximation;
        approximation.approxAlpha = alpha;
        approximation.approxBeta = beta;
        approximation.approxXm = -1.0 * x_m;
        approximation.approxYm = -1.0 * y_m;
        approximation.approxRadius = radius;
        approximation.stdev = sum_vv;
        this->approximations.append(approximation);

    }

    //##############################################################
    //another approximation comes from the first two cylinder points
    //##############################################################

    OiVec diff = inputObservations.at(0)->getXYZ() - inputObservations.at(1)->getXYZ();
    diff.removeLast();
    pn = diff;

    //calculate rotation angles
    a = qSqrt(1.0 / (1.0 + (pn.getAt(2)/pn.getAt(1))*(pn.getAt(2)/pn.getAt(1))));
    b = a * pn.getAt(2) / pn.getAt(1);

    b_alpha = qAcos(b);
    a_alpha = qAsin(a);

    alpha = 0.0;

    //cos and sin are ambiguous, therefor check the right angle here
    if(this->compareAngles(b_alpha, a_alpha)){
        alpha = a_alpha;
    }else if(this->compareAngles(this->getCorrespondingCos(b_alpha), a_alpha)){
        alpha = a_alpha;
    }else if(this->compareAngles(b_alpha, this->getCorrespondingSin(a_alpha))){
        alpha = b_alpha;
    }else{
        alpha = this->getCorrespondingCos(b_alpha);
    }

    //transform eigenvector --> y-component should be 0 now
    _y = pn.getAt(1);
    _z = pn.getAt(2);
    pn.setAt(1, _y * qCos(alpha) - _z * qSin(alpha));
    pn.setAt(2, _y * qSin(alpha) + _z * qCos(alpha));

    a = 0.0, b = 0.0;
    a = qSqrt(1.0 / (1.0 + (-1.0 * pn.getAt(2) / pn.getAt(0))*(-1.0 * pn.getAt(2) / pn.getAt(0))));
    b = -1.0 * pn.getAt(2) * a / pn.getAt(0);

    b_beta = qAcos(b);
    a_beta = qAsin(a);

    beta = 0.0;

    //cos and sin are ambiguous, therefor check the right angle here
    if(this->compareAngles(b_beta, a_beta)){
        beta = b_beta;
    }else if(this->compareAngles(this->getCorrespondingCos(b_beta), a_beta)){
        beta = a_beta;
    }else if(this->compareAngles(b_beta, this->getCorrespondingSin(a_beta))){
        beta = b_beta;
    }else{
        beta = this->getCorrespondingCos(b_beta);
    }

    //set up rotation matrices
    Ralpha.setAt(0, 0, 1.0);
    Ralpha.setAt(1, 1, qCos(alpha));
    Ralpha.setAt(1, 2, -qSin(alpha));
    Ralpha.setAt(2, 1, qSin(alpha));
    Ralpha.setAt(2, 2, qCos(alpha));
    Rbeta.setAt(0, 0, qCos(beta));
    Rbeta.setAt(0, 2, qSin(beta));
    Rbeta.setAt(1, 1, 1.0);
    Rbeta.setAt(2, 0, -qSin(beta));
    Rbeta.setAt(2, 2, qCos(beta));

    Rall = Rbeta * Ralpha;

    //###########################################
    //circle fit to determine midpoint and radius
    //###########################################

    //clear transformed point lists
    points2D_x.clear();
    points2D_y.clear();
    centroidreduced2D_x.clear();
    centroidreduced2D_y.clear();

    //reset 2D centroid and recalculate with new rotation
    centroid2D = OiVec(2);

    //rotate observations in XY-plane and calculate 2D centroid
    for(int j = 0; j < numPoints; j++){

        QPointer<Observation> obs = inputObservations.at(j);

        tx = Rall.getAt(0,0)*obs->getXYZ().getAt(0)
                + Rall.getAt(0,1)*obs->getXYZ().getAt(1)
                + Rall.getAt(0,2)*obs->getXYZ().getAt(2);
        ty = Rall.getAt(1,0)*obs->getXYZ().getAt(0)
                + Rall.getAt(1,1)*obs->getXYZ().getAt(1)
                + Rall.getAt(1,2)*obs->getXYZ().getAt(2);

        points2D_x.append( tx );
        points2D_y.append( ty );

        centroid2D.setAt(0, centroid2D.getAt(0) + tx);
        centroid2D.setAt(1, centroid2D.getAt(1) + ty);

    }
    centroid2D.setAt(0, centroid2D.getAt(0) / (float)numPoints);
    centroid2D.setAt(1, centroid2D.getAt(1) / (float)numPoints);

    //calculate centroid reduced 2D coordinates
    for(int j = 0; j < numPoints; j++){
        centroidreduced2D_x.append( points2D_x.at(j) - centroid2D.getAt(0) );
        centroidreduced2D_y.append( points2D_y.at(j) - centroid2D.getAt(1) );
    }

    //fill a matrices
    for(int j = 0; j < numPoints; j++){
        tx = centroidreduced2D_x.at(j);
        ty = centroidreduced2D_y.at(j);

        A1.setAt(j, tx*tx + ty*ty);

        A2.setAt(j,0,tx);
        A2.setAt(j,1,ty);
        A2.setAt(j,2,1.0);
    }

    //solve equation system to get circle parameters
    try{
        if(!OiMat::solve(s, A2.t() * A2, -1.0 * A2.t() * A1)){
            return false;
        }
    }catch(const exception &e){
        emit this->sendMessage(QString("inv error cylinder minimum solution: %1").arg(e.what()), eErrorMessage);
        return false;
    }

    //calculate midpoint + radius
    x_m = (-1.0 * s.getAt(0) / 2.0) + centroid2D.getAt(0);
    y_m = (-1.0 * s.getAt(1) / 2.0) + centroid2D.getAt(1);
    radius = qSqrt(0.25 * (s.getAt(0) * s.getAt(0) + s.getAt(1) * s.getAt(1)) - s.getAt(2));

    //calculate statistic
    v = -1.0 * A1 - A2 * s;
    OiVec::dot(sum_vv, v,v);
    sum_vv = qSqrt(sum_vv / (numPoints-3.0));

    //add approximation
    CylinderApproximation approximation;
    approximation.approxAlpha = alpha;
    approximation.approxBeta = beta;
    approximation.approxXm = -1.0 * x_m;
    approximation.approxYm = -1.0 * y_m;
    approximation.approxRadius = radius;
    approximation.stdev = sum_vv;
    this->approximations.append(approximation);

    return true;

}

/*!
 * \brief BestFitCylinder::fitCylinder
 * \param cylinder
 * \param inputObservations
 * \param approximation
 * \return
 */
bool BestFitCylinder::fitCylinder(Cylinder &cylinder, const QList<QPointer<Observation> > &inputObservations, const CylinderApproximation &approximation){

    //get the number of observations
    int numPoints = inputObservations.size();

    //centroids of all points
    OiVec centroid(4);

    //initialize variables
    OiVec v(numPoints*3); //approximation of corrections
    OiVec L0(numPoints*3); //L + v
    OiVec c(numPoints+5); //right side
    OiMat N(numPoints+5, numPoints+5); //normal equation matrix
    OiVec res(numPoints+5); //adjustment result
    double _r = 0.0, _X0 = 0.0, _Y0 = 0.0, _alpha = 0.0, _beta = 0.0;
    double _r_armijo = 0.0, _X0_armijo = 0.0, _Y0_armijo = 0.0, _alpha_armijo = 0.0, _beta_armijo = 0.0;
    //OiVec a(5);
    //OiVec b(5);
    OiVec a(numPoints);
    OiVec b(numPoints);
    double _x = 0.0, _y = 0.0, _z = 0.0;
    double _x_armijo = 0.0, _y_armijo = 0.0, _z_armijo = 0.0;
    double a1 = 0.0, a2 = 0.0;
    double diff = 0.0, _xr = 0.0, _yr = 0.0;
    double sigma = 2.0;
    OiMat Ralpha(3, 3);
    OiMat Rbeta(3, 3);
    OiMat Rall(3, 3);

    //set approximations of unknowns
    _r = approximation.approxRadius;
    _X0 = approximation.approxXm;
    _Y0 = approximation.approxYm;
    _alpha = approximation.approxAlpha;
    _beta = approximation.approxBeta;
    OiVec x(5); //vector of unknown corrections

    //fill L vector
    for(int i = 0; i < numPoints; i++){

        QPointer<Observation> obs = inputObservations.at(i);

        L0.setAt(i*3, obs->getXYZ().getAt(0));
        L0.setAt(i*3+1, obs->getXYZ().getAt(1));
        L0.setAt(i*3+2, obs->getXYZ().getAt(2));

        centroid = centroid + obs->getXYZ();

    }
    centroid.removeLast();
    centroid = centroid / (float)numPoints;

    int numIterations = 0;

    double stopAA = 0.0, stopBB = 0.0, stopXX = 0.0, stopVV = 0.0;
    do{

        //improve unknowns
        _r += x.getAt(0);
        _X0 += x.getAt(1);
        _Y0 += x.getAt(2);
        _alpha += x.getAt(3);
        _beta += x.getAt(4);

        //improve observations
        //L0 = L0 + v;

        //build rotation matrices
        Ralpha.setAt(0, 0, 1.0);
        Ralpha.setAt(1, 1, qCos(_alpha));
        Ralpha.setAt(1, 2, -qSin(_alpha));
        Ralpha.setAt(2, 1, qSin(_alpha));
        Ralpha.setAt(2, 2, qCos(_alpha));
        Rbeta.setAt(0, 0, qCos(_beta));
        Rbeta.setAt(0, 2, qSin(_beta));
        Rbeta.setAt(1, 1, 1.0);
        Rbeta.setAt(2, 0, -qSin(_beta));
        Rbeta.setAt(2, 2, qCos(_beta));

        Rall = Rbeta * Ralpha;

        //fill A and B matrix + w vector + right side
        for(int i = 0; i < numPoints; i++){

            _x = L0.getAt(i*3);
            _y = L0.getAt(i*3+1);
            _z = L0.getAt(i*3+2);

            a1 = _X0 + _x * qCos(_beta) + _y * qSin(_alpha) * qSin(_beta) + _z * qCos(_alpha) * qSin(_beta);
            a2 = _Y0 + _y * qCos(_alpha) - _z * qSin(_alpha);

            //A
            N.setAt(i, numPoints, 1.0);
            N.setAt(i, numPoints+1, -1.0 * a1 / qSqrt(a1*a1 + a2*a2));
            N.setAt(i, numPoints+2, -1.0 * a2 / qSqrt(a1*a1 + a2*a2));
            N.setAt(i, numPoints+3, -1.0 * ((_y * qSin(_beta) * qCos(_alpha) - _z * qSin(_beta) * qSin(_alpha)) * a1 - (_y * qSin(_alpha) + _z * qCos(_alpha)) * a2) / qSqrt(a1*a1 + a2*a2));
            N.setAt(i, numPoints+4, -1.0 * (_y * qSin(_alpha) * qCos(_beta) - _x * qSin(_beta) + _z * qCos(_alpha) * qCos(_beta)) * a1 / qSqrt(a1*a1 + a2*a2));

            //AT
            N.setAt(numPoints, i, 1.0);
            N.setAt(numPoints+1, i, -1.0 * a1 / qSqrt(a1*a1 + a2*a2));
            N.setAt(numPoints+2, i, -1.0 * a2 / qSqrt(a1*a1 + a2*a2));
            N.setAt(numPoints+3, i, -1.0 * ((_y * qSin(_beta) * qCos(_alpha) - _z * qSin(_beta) * qSin(_alpha)) * a1 - (_y * qSin(_alpha) + _z * qCos(_alpha)) * a2) / qSqrt(a1*a1 + a2*a2));
            N.setAt(numPoints+4, i, -1.0 * (_y * qSin(_alpha) * qCos(_beta) - _x * qSin(_beta) + _z * qCos(_alpha) * qCos(_beta)) * a1 / qSqrt(a1*a1 + a2*a2));

            //BBT
            N.setAt(i, i, (-1.0 * qCos(_beta) * a1 / qSqrt(a1*a1 + a2*a2))*(-1.0 * qCos(_beta) * a1 / qSqrt(a1*a1 + a2*a2))
                        + (-1.0 * (qSin(_alpha) * qSin(_beta) * a1 + qCos(_alpha) * a2) / qSqrt(a1*a1 + a2*a2))*(-1.0 * (qSin(_alpha) * qSin(_beta) * a1 + qCos(_alpha) * a2) / qSqrt(a1*a1 + a2*a2))
                        + (-1.0 * (qCos(_alpha) * qSin(_beta) * a1 - qSin(_alpha) * a2) / qSqrt(a1*a1 + a2*a2))*(-1.0 * (qCos(_alpha) * qSin(_beta) * a1 - qSin(_alpha) * a2) / qSqrt(a1*a1 + a2*a2)));

            _xr = Rall.getAt(0,0) * _x + Rall.getAt(0,1) * _y + Rall.getAt(0,2) * _z + _X0;
            _yr = Rall.getAt(1,0) * _x + Rall.getAt(1,1) * _y + Rall.getAt(1,2) * _z + _Y0;

            //approximate radius of the cylinder minus distance of point i to the z-axis is the contradiction
            diff = _r - qSqrt(_xr*_xr + _yr*_yr);

            c.setAt(i, diff);

        }

        //solve the normal equation system
        try{
            if(!OiMat::solve(res, N, -1.0*c)){
                return false;
            }
        }catch(const exception &e){
            emit this->sendMessage(QString("solve error cylinder fit: %1").arg(e.what()), eErrorMessage);
            return false;
        }

        //get new corrections for unknowns
        for(int i = numPoints; i < numPoints+5; i++){
            x.setAt(i-numPoints, res.getAt(i));
        }

        //calculate improvements
        for(int i = 0; i < numPoints; ++i){

            QPointer<Observation> obs = inputObservations.at(i);

            _x = obs->getXYZ().getAt(0);
            _y = obs->getXYZ().getAt(1);
            _z = obs->getXYZ().getAt(2);

            a1 = _X0 + _x * qCos(_beta) + _y * qSin(_alpha) * qSin(_beta) + _z * qCos(_alpha) * qSin(_beta);
            a2 = _Y0 + _y * qCos(_alpha) - _z * qSin(_alpha);

            v.setAt(i*3, (-1.0 * qCos(_beta) * a1 / qSqrt(a1*a1 + a2*a2)) * res.getAt(i));
            v.setAt(i*3+1, (-1.0 * (qSin(_alpha) * qSin(_beta) * a1 + qCos(_alpha) * a2) / qSqrt(a1*a1 + a2*a2)) * res.getAt(i));
            v.setAt(i*3+2, (-1.0 * (qCos(_alpha) * qSin(_beta) * a1 - qSin(_alpha) * a2) / qSqrt(a1*a1 + a2*a2)) * res.getAt(i));

        }

        //apply Armijo rule which is useful in case of bad approximations
        /*do{

            sigma = sigma / 2.0;

            _r_armijo = _r + sigma * x.getAt(0);
            _X0_armijo = _X0 + sigma * x.getAt(1);
            _Y0_armijo = _Y0 + sigma * x.getAt(2);
            _alpha_armijo = _alpha + sigma * x.getAt(3);
            _beta_armijo = _beta + sigma * x.getAt(4);

            for(int i = 0; i < 5; i++){
                _x = L0.getAt(i*3);
                _y = L0.getAt(i*3+1);
                _z = L0.getAt(i*3+2);

                a.setAt(i, _r_armijo - qSqrt( (_X0_armijo + _x*qCos(_beta_armijo) + _y*qSin(_alpha_armijo)*qSin(_beta_armijo) + _z*qCos(_alpha_armijo)*qSin(_beta_armijo))*(_X0_armijo + _x*qCos(_beta_armijo) + _y*qSin(_alpha_armijo)*qSin(_beta_armijo) + _z*qCos(_alpha_armijo)*qSin(_beta_armijo))
                                       + (_Y0_armijo + _y*qCos(_alpha_armijo) - _z*qSin(_alpha_armijo))*(_Y0_armijo + _y*qCos(_alpha_armijo) - _z*qSin(_alpha_armijo)) ));

                b.setAt(i, _r - qSqrt( (_X0 + _x*qCos(_beta) + _y*qSin(_alpha)*qSin(_beta) + _z*qCos(_alpha)*qSin(_beta))*(_X0 + _x*qCos(_beta) + _y*qSin(_alpha)*qSin(_beta) + _z*qCos(_alpha)*qSin(_beta))
                                               + (_Y0 + _y*qCos(_alpha) - _z*qSin(_alpha))*(_Y0 + _y*qCos(_alpha) - _z*qSin(_alpha)) ));
            }

            OiVec::dot(stopAA, a, a);
            OiVec::dot(stopBB, b, b);

        }while( stopAA > ( stopBB - 2.0 * 0.001 * sigma * stopBB ) );

        x = sigma * x;*/




        do{

            sigma = sigma / 2.0;

            _r_armijo = _r + sigma * x.getAt(0);
            _X0_armijo = _X0 + sigma * x.getAt(1);
            _Y0_armijo = _Y0 + sigma * x.getAt(2);
            _alpha_armijo = _alpha + sigma * x.getAt(3);
            _beta_armijo = _beta + sigma * x.getAt(4);

            for(int i = 0; i < numPoints; i++){
                _x = L0.getAt(i*3);
                _y = L0.getAt(i*3+1);
                _z = L0.getAt(i*3+2);

                _x_armijo = L0.getAt(i*3) + sigma * v.getAt(i*3);
                _y_armijo = L0.getAt(i*3+1) + sigma * v.getAt(i*3+1);
                _z_armijo = L0.getAt(i*3+2) + sigma * v.getAt(i*3+2);

                a.setAt(i, _r_armijo - qSqrt( (_X0_armijo + _x_armijo*qCos(_beta_armijo) + _y_armijo*qSin(_alpha_armijo)*qSin(_beta_armijo) + _z_armijo*qCos(_alpha_armijo)*qSin(_beta_armijo))*(_X0_armijo + _x_armijo*qCos(_beta_armijo) + _y_armijo*qSin(_alpha_armijo)*qSin(_beta_armijo) + _z_armijo*qCos(_alpha_armijo)*qSin(_beta_armijo))
                                       + (_Y0_armijo + _y_armijo*qCos(_alpha_armijo) - _z_armijo*qSin(_alpha_armijo))*(_Y0_armijo + _y_armijo*qCos(_alpha_armijo) - _z_armijo*qSin(_alpha_armijo)) ));

                b.setAt(i, _r - qSqrt( (_X0 + _x*qCos(_beta) + _y*qSin(_alpha)*qSin(_beta) + _z*qCos(_alpha)*qSin(_beta))*(_X0 + _x*qCos(_beta) + _y*qSin(_alpha)*qSin(_beta) + _z*qCos(_alpha)*qSin(_beta))
                                               + (_Y0 + _y*qCos(_alpha) - _z*qSin(_alpha))*(_Y0 + _y*qCos(_alpha) - _z*qSin(_alpha)) ));
            }

            OiVec::dot(stopAA, a, a);
            OiVec::dot(stopBB, b, b);

        }while( stopAA > stopBB );

        OiVec::dot(stopXX, x, x);
        OiVec::dot(stopVV, v, v);

        x = sigma * x;
        v = sigma * v;





        //nur testweise
        if(sigma < 0.1){
            std::default_random_engine generator;


            /*_r = approximation.approxRadius;
            _X0 = approximation.approxXm;
            _Y0 = approximation.approxYm;
            _alpha = approximation.approxAlpha;
            _beta = approximation.approxBeta;

            OiVec X_temp(5);
            X_temp.setAt(0, _r);
            X_temp.setAt(1, _X0);
            X_temp.setAt(2, _Y0);
            X_temp.setAt(3, _alpha);
            X_temp.setAt(4, _beta);*/

            for(int k = 0; k < 5; k++){
                std::uniform_real_distribution<double> distribution(-3.0 * qAbs(x.getAt(k)), 3.0 * qAbs(x.getAt(k)));
                //std::uniform_real_distribution<double> distribution(-0.001 * qAbs(X_temp.getAt(k)), 0.001 * qAbs(X_temp.getAt(k)));
                //std::uniform_real_distribution<double> distribution(-(double)(1.0/sigma) * qAbs(X_temp.getAt(k)), (double)(1.0/sigma) * qAbs(X_temp.getAt(k)));
                double dice_roll = distribution(generator);
                x.setAt(k, x.getAt(k) + dice_roll);
            }
        }



        sigma = 2.0;

        numIterations++;

    //}while( (stopXX > 0.000000001) && (stopVV > 0.000000001) && numIterations < 1000 );
    //}while( (stopXX > 0.00000000001) && (stopVV > 0.00000000001) && numIterations < 1000 );
    //}while( ((stopXX > 0.00000000001) || (stopVV > 0.00000000001)) && numIterations < 1000 );
    //}while( ((stopXX > 0.0000000000001) || (stopVV > 0.00000000001)) && numIterations < 1000 );
    }while( stopXX > 0.0000000000001 && numIterations < 1000 );

    if(numIterations >= 1000){
        return false;
    }

    //apply corrections to unknowns
    _r += x.getAt(0);
    _X0 += x.getAt(1);
    _Y0 += x.getAt(2);
    _alpha += x.getAt(3);
    _beta += x.getAt(4);

    //calculate rotation matrices
    Ralpha.setAt(0, 0, 1.0);
    Ralpha.setAt(1, 1, qCos(_alpha));
    Ralpha.setAt(1, 2, -qSin(_alpha));
    Ralpha.setAt(2, 1, qSin(_alpha));
    Ralpha.setAt(2, 2, qCos(_alpha));
    Rbeta.setAt(0, 0, qCos(_beta));
    Rbeta.setAt(0, 2, qSin(_beta));
    Rbeta.setAt(1, 1, 1.0);
    Rbeta.setAt(2, 0, -qSin(_beta));
    Rbeta.setAt(2, 2, qCos(_beta));

    Rall = Rbeta * Ralpha;

    //calculate point on cylinder axis
    OiVec xyz(3);
    xyz.setAt(0, _X0);
    xyz.setAt(1, _Y0);
    xyz.setAt(2, 0.0);
    OiMat::solve(xyz, Rall, -1.0 * xyz);

    //calculate cylinder axis
    OiVec axis(3);
    axis.setAt(2, 1.0);
    OiMat::solve(axis, Rall, axis);

    //calculate sum vv
    double sumVV = 0.0;
    OiVec v_obs(3);
    for(int i = 0; i < numPoints; i++){

        QPointer<Observation> obs = inputObservations.at(i);

        _x = obs->getXYZ().getAt(0);
        _y = obs->getXYZ().getAt(1);
        _z = obs->getXYZ().getAt(2);

        float b[3]; //vector between point on cylinder axis and point p which is probably on cylinder
        b[0] = _x - xyz.getAt(0);
        b[1] = _y - xyz.getAt(1);
        b[2] = _z - xyz.getAt(2);

        float n0CrossB[3]; //cross product of cylinder axis (length 1) and b
        n0CrossB[0] = axis.getAt(1) * b[2] - axis.getAt(2) * b[1];
        n0CrossB[1] = axis.getAt(2) * b[0] - axis.getAt(0) * b[2];
        n0CrossB[2] = axis.getAt(0) * b[1] - axis.getAt(1) * b[0];

        float radiusActual = 0.0f; //smallest distance of point p to cylinder axis

        radiusActual = qSqrt( n0CrossB[0]*n0CrossB[0] + n0CrossB[1]*n0CrossB[1] + n0CrossB[2]*n0CrossB[2] );

        float distance = 0.0f;

        distance = radiusActual - _r; //distance error

        //calculate residual vector
        v_obs.setAt(0, b[0]);
        v_obs.setAt(1, b[1]);
        v_obs.setAt(2, b[2]);
        v_obs.normalize();
        v_obs = distance * v_obs;

        //set up display residuals
        Residual residual;
        residual.elementId = obs->getId();
        residual.dimension = eMetric;
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayVX), v_obs.getAt(0));
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayVY), v_obs.getAt(1));
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayVZ), v_obs.getAt(2));
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayV), qSqrt(v_obs.getAt(0) * v_obs.getAt(0)
                                                                                                + v_obs.getAt(2) * v_obs.getAt(2)));
        this->statistic.addDisplayResidual(residual);

        sumVV += distance * distance;

    }

    //set up result
    Radius cylinderRadius(_r);
    Direction cylinderDirection(axis);
    Position cylinderPosition(xyz);
    cylinder.setCylinder(cylinderPosition, cylinderDirection, cylinderRadius);
    this->statistic.setIsValid(true);
    this->statistic.setStdev(qSqrt(sumVV / ((double)numPoints - 5.0)));
    cylinder.setStatistic(this->statistic);

    return true;

}

/*!
 * \brief BestFitCylinder::getCorrespondingCos
 * \param a
 * \return
 */
double BestFitCylinder::getCorrespondingCos(double a){
    while(a < 0.0){
        a += 2.0*PI;
    }
    while(a > 2.0*PI){
        a -= 2.0*PI;
    }
    return 2.0 * PI - a;
}

/*!
 * \brief BestFitCylinder::getCorrespondingSin
 * \param a
 * \return
 */
double BestFitCylinder::getCorrespondingSin(double a){
    while(a < 0.0){
        a += 2.0*PI;
    }
    while(a > 2.0*PI){
        a -= 2.0*PI;
    }
    if(a <= PI){
        return PI - a;
    }else{
        return 3.0*PI - a;
    }
}

/*!
 * \brief BestFitCylinder::compareAngles
 * \param a
 * \param b
 * \return
 */
bool BestFitCylinder::compareAngles(double a, double b){

    double t = 2.0*PI;

    //a und b between 0 und 2*PI
    while(a < 0.0){
        a += t;
    }
    while(a > t){
        a -= t;
    }
    while(b < 0.0){
        b += t;
    }
    while(b > t){
        b -= t;
    }

    double diff = a - b;
    if(diff < 0.001 && diff > -0.001){
        return true;
    }
    return false;

}

bool BestFitCylinder::test(Cylinder &cylinder){

    //get and check input observations
    if(!this->inputElements.contains(0) || this->inputElements[0].size() < 5){
        emit this->sendMessage(QString("Not enough valid observations to fit the cylinder %1").arg(cylinder.getFeatureName()), eWarningMessage);
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
    if(inputObservations.size() < 5){
        emit this->sendMessage(QString("Not enough valid observations to fit the cylinder %1").arg(cylinder.getFeatureName()), eWarningMessage);
        return false;
    }

    //approximate cylinder by 2D circle fit
    if(!this->approximateCylinder(cylinder, inputObservations)){
        emit this->sendMessage(QString("Error while generating approximations for cylinder parameters of cylinder %1").arg(cylinder.getFeatureName()), eErrorMessage);
        return false;
    }

    //fit the cylinder using the previously generated approximations
    int bestSolution = -1;
    double stdev = numeric_limits<double>::max();
    for(int i = 0; i < this->approximations.size(); i++){
        bool success = this->fitCylinder(cylinder, inputObservations, this->approximations[i]);
        if(success && cylinder.getStatistic().getStdev() < stdev){
            bestSolution = i;
            stdev = cylinder.getStatistic().getStdev();
        }
    }
    if(bestSolution < 0){
        emit this->sendMessage(QString("Error while fitting cylinder %1").arg(cylinder.getFeatureName()), eErrorMessage);
        return false;
    }
    this->fitTest(cylinder, inputObservations, this->approximations[bestSolution]);

    //check that the direction vector is defined by the first two observations
    OiVec pos1 = inputObservations.at(0)->getXYZ();
    pos1.removeLast();
    OiVec pos2 = inputObservations.at(1)->getXYZ();
    pos2.removeLast();
    OiVec direction = pos2 - pos1;
    direction.normalize();
    double angle = 0.0; //angle between r and direction
    OiVec::dot(angle, cylinder.getDirection().getVector(), direction);
    angle = qAbs(qAcos(angle));
    if(angle > (PI/2.0)){
        Direction cylinderDirection = cylinder.getDirection();
        cylinderDirection.setVector(cylinder.getDirection().getVector() * -1.0);
        cylinder.setCylinder(cylinder.getPosition(), cylinderDirection, cylinder.getRadius());
    }

    return true;

}

bool BestFitCylinder::approxTest(Cylinder &cylinder, const QList<QPointer<Observation> > &inputObservations){

    //clear current approximations
    this->approximations.clear();

    //get the number of observations
    int numPoints = inputObservations.size();

    //calculate centroid of all observations
    OiVec centroid3D(4);
    foreach(const QPointer<Observation> &obs, inputObservations){
        centroid3D = centroid3D + obs->getXYZ();
    }
    centroid3D.removeLast();
    centroid3D = centroid3D / numPoints;

    //set up covariance matrix of all observations
    OiMat H(3, 3);
    for (int k = 0; k < numPoints; k++) {
        QPointer<Observation> obs = inputObservations.at(k);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                double a = 0.0, b = 0.0;
                if (i == 0)
                    a = obs->getXYZ().getAt(0) - centroid3D.getAt(0);
                else if (i == 1)
                    a = obs->getXYZ().getAt(1) - centroid3D.getAt(1);
                else
                    a = obs->getXYZ().getAt(2) - centroid3D.getAt(2);
                if (j == 0)
                    b = obs->getXYZ().getAt(0) - centroid3D.getAt(0);
                else if (j == 1)
                    b = obs->getXYZ().getAt(1) - centroid3D.getAt(1);
                else
                    b = obs->getXYZ().getAt(2) - centroid3D.getAt(2);
                H.setAt(i,j, H.getAt(i,j) + a * b);
            }
        }
    }

    //singular value decomposition of H to get the major axis direction of the cylinder observations
    OiMat U(3, 3);
    OiVec d(3);
    OiMat V(3, 3);
    try{
        H.svd(U, d, V);
    }catch(const exception &e){
        emit this->sendMessage(QString("SVD error cylinder minimum solution: %1").arg(e.what()), eErrorMessage);
        return false;
    }

    //init helper variables
    OiVec centroid2D(2); //centroid of 2D circle
    double alpha_n = 0.0, beta_n = 0.0, radius_n = 0.0, x_m_n = 0.0, y_m_n = 0.0; //final solutions of cylinder approximation
    double vv_ref = numeric_limits<double>::max(); //smallest vv that has been found
    OiVec pn; //possible normal vector
    double a = 0.0, b = 0.0; //sin + cos of rotation angles
    double a_alpha = 0.0, b_alpha = 0.0, a_beta = 0.0, b_beta = 0.0; //possible rotation angles (check acos + asin)
    double alpha = 0.0, beta = 0.0; //rotation angles
    double _y = 0.0, _z = 0.0;
    double tx = 0.0, ty = 0.0; //transformed 2D coordinates
    double x_m = 0.0, y_m = 0.0, radius = 0.0, sum_vv = 0.0; //result parameters in circle fit
    OiVec v;

    //rotation matrices
    OiMat Ralpha(3, 3);
    OiMat Rbeta(3, 3);
    OiMat Rall(3, 3);

    //a matrices for circle adjustment
    OiVec A1(numPoints);
    OiMat A2(numPoints,3);

    //result vector for circle fit
    OiVec s(3);

    //rotated 2D points
    QList<float> points2D_x;
    QList<float> points2D_y;

    //rotated centroid reduced 2D points
    QList<float> centroidreduced2D_x;
    QList<float> centroidreduced2D_y;

    pn = cylinder.getDirection().getVector();

    //calculate rotation angles
    a = qSqrt(1.0 / (1.0 + (pn.getAt(2)/pn.getAt(1))*(pn.getAt(2)/pn.getAt(1))));
    b = a * pn.getAt(2) / pn.getAt(1);

    b_alpha = qAcos(b);
    a_alpha = qAsin(a);

    alpha = 0.0;

    //cos and sin are ambiguous, therefor check the right angle here
    if(this->compareAngles(b_alpha, a_alpha)){
        alpha = a_alpha;
    }else if(this->compareAngles(this->getCorrespondingCos(b_alpha), a_alpha)){
        alpha = a_alpha;
    }else if(this->compareAngles(b_alpha, this->getCorrespondingSin(a_alpha))){
        alpha = b_alpha;
    }else{
        alpha = this->getCorrespondingCos(b_alpha);
    }

    //transform eigenvector --> y-component should be 0 now
    _y = pn.getAt(1);
    _z = pn.getAt(2);
    pn.setAt(1, _y * qCos(alpha) - _z * qSin(alpha));
    pn.setAt(2, _y * qSin(alpha) + _z * qCos(alpha));

    a = 0.0, b = 0.0;
    a = qSqrt(1.0 / (1.0 + (-1.0 * pn.getAt(2) / pn.getAt(0))*(-1.0 * pn.getAt(2) / pn.getAt(0))));
    b = -1.0 * pn.getAt(2) * a / pn.getAt(0);

    b_beta = qAcos(b);
    a_beta = qAsin(a);

    beta = 0.0;

    //cos and sin are ambiguous, therefor check the right angle here
    if(this->compareAngles(b_beta, a_beta)){
        beta = b_beta;
    }else if(this->compareAngles(this->getCorrespondingCos(b_beta), a_beta)){
        beta = a_beta;
    }else if(this->compareAngles(b_beta, this->getCorrespondingSin(a_beta))){
        beta = b_beta;
    }else{
        beta = this->getCorrespondingCos(b_beta);
    }

    //set up rotation matrices
    Ralpha.setAt(0, 0, 1.0);
    Ralpha.setAt(1, 1, qCos(alpha));
    Ralpha.setAt(1, 2, -qSin(alpha));
    Ralpha.setAt(2, 1, qSin(alpha));
    Ralpha.setAt(2, 2, qCos(alpha));
    Rbeta.setAt(0, 0, qCos(beta));
    Rbeta.setAt(0, 2, qSin(beta));
    Rbeta.setAt(1, 1, 1.0);
    Rbeta.setAt(2, 0, -qSin(beta));
    Rbeta.setAt(2, 2, qCos(beta));

    Rall = Rbeta * Ralpha;

    //###########################################
    //circle fit to determine midpoint and radius
    //###########################################

    //clear transformed point lists
    points2D_x.clear();
    points2D_y.clear();
    centroidreduced2D_x.clear();
    centroidreduced2D_y.clear();

    //reset 2D centroid and recalculate with new rotation
    centroid2D = OiVec(2);

    //rotate observations in XY-plane and calculate 2D centroid
    for(int j = 0; j < numPoints; j++){

        QPointer<Observation> obs = inputObservations.at(j);

        tx = Rall.getAt(0,0)*obs->getXYZ().getAt(0)
                + Rall.getAt(0,1)*obs->getXYZ().getAt(1)
                + Rall.getAt(0,2)*obs->getXYZ().getAt(2);
        ty = Rall.getAt(1,0)*obs->getXYZ().getAt(0)
                + Rall.getAt(1,1)*obs->getXYZ().getAt(1)
                + Rall.getAt(1,2)*obs->getXYZ().getAt(2);

        points2D_x.append( tx );
        points2D_y.append( ty );

        centroid2D.setAt(0, centroid2D.getAt(0) + tx);
        centroid2D.setAt(1, centroid2D.getAt(1) + ty);

    }
    centroid2D.setAt(0, centroid2D.getAt(0) / (float)numPoints);
    centroid2D.setAt(1, centroid2D.getAt(1) / (float)numPoints);

    //calculate centroid reduced 2D coordinates
    for(int j = 0; j < numPoints; j++){
        centroidreduced2D_x.append( points2D_x.at(j) - centroid2D.getAt(0) );
        centroidreduced2D_y.append( points2D_y.at(j) - centroid2D.getAt(1) );
    }

    //fill a matrices
    for(int j = 0; j < numPoints; j++){
        tx = centroidreduced2D_x.at(j);
        ty = centroidreduced2D_y.at(j);

        A1.setAt(j, tx*tx + ty*ty);

        A2.setAt(j,0,tx);
        A2.setAt(j,1,ty);
        A2.setAt(j,2,1.0);
    }

    //solve equation system to get circle parameters
    try{
        if(!OiMat::solve(s, A2.t() * A2, -1.0 * A2.t() * A1)){
            return false;
        }
    }catch(const exception &e){
        emit this->sendMessage(QString("inv error cylinder minimum solution: %1").arg(e.what()), eErrorMessage);
        return false;
    }

    //calculate midpoint + radius
    x_m = (-1.0 * s.getAt(0) / 2.0) + centroid2D.getAt(0);
    y_m = (-1.0 * s.getAt(1) / 2.0) + centroid2D.getAt(1);
    radius = qSqrt(0.25 * (s.getAt(0) * s.getAt(0) + s.getAt(1) * s.getAt(1)) - s.getAt(2));

    //calculate statistic
    v = -1.0 * A1 - A2 * s;
    OiVec::dot(sum_vv, v,v);
    sum_vv = qSqrt(sum_vv / (numPoints-3.0));

    //add approximation
    CylinderApproximation approximation;
    approximation.approxAlpha = alpha;
    approximation.approxBeta = beta;
    approximation.approxXm = -1.0 * x_m;
    approximation.approxYm = -1.0 * y_m;
    approximation.approxRadius = radius;
    this->approximations.append(approximation);

    return true;

}

bool BestFitCylinder::fitTest(Cylinder &cylinder, const QList<QPointer<Observation> > &inputObservations, const CylinderApproximation &approximation){

    //get the number of observations
    int numPoints = inputObservations.size();

    //centroids of all points
    OiVec centroid(4);

    //initialize variables
    OiVec v(numPoints*3); //approximation of corrections
    OiVec L0(numPoints*3); //L + v
    OiVec c(numPoints+5); //right side
    OiMat N(numPoints+5, numPoints+5); //normal equation matrix
    OiVec res(numPoints+5); //adjustment result
    double _r = 0.0, _X0 = 0.0, _Y0 = 0.0, _alpha = 0.0, _beta = 0.0;
    double _r_armijo = 0.0, _X0_armijo = 0.0, _Y0_armijo = 0.0, _alpha_armijo = 0.0, _beta_armijo = 0.0;
    //OiVec a(5);
    //OiVec b(5);
    OiVec a(numPoints);
    OiVec b(numPoints);
    double _x = 0.0, _y = 0.0, _z = 0.0;
    double _x_armijo = 0.0, _y_armijo = 0.0, _z_armijo = 0.0;
    double a1 = 0.0, a2 = 0.0;
    double diff = 0.0, _xr = 0.0, _yr = 0.0;
    double sigma = 2.0;
    OiMat Ralpha(3, 3);
    OiMat Rbeta(3, 3);
    OiMat Rall(3, 3);

    //set approximations of unknowns
    _r = approximation.approxRadius;
    _X0 = approximation.approxXm;
    _Y0 = approximation.approxYm;
    _alpha = approximation.approxAlpha;
    _beta = approximation.approxBeta;
    OiVec x(5); //vector of unknown corrections

    //fill L vector
    for(int i = 0; i < numPoints; i++){

        QPointer<Observation> obs = inputObservations.at(i);

        L0.setAt(i*3, obs->getXYZ().getAt(0));
        L0.setAt(i*3+1, obs->getXYZ().getAt(1));
        L0.setAt(i*3+2, obs->getXYZ().getAt(2));

        centroid = centroid + obs->getXYZ();

    }
    centroid.removeLast();
    centroid = centroid / (float)numPoints;

    int numIterations = 0;

    double stopAA = 0.0, stopBB = 0.0, stopXX = 0.0, stopVV = 0.0;
    do{

        //improve unknowns
        _r += x.getAt(0);
        _X0 += x.getAt(1);
        _Y0 += x.getAt(2);
        _alpha += x.getAt(3);
        _beta += x.getAt(4);

        //improve observations
        //L0 = L0 + v;

        //build rotation matrices
        Ralpha.setAt(0, 0, 1.0);
        Ralpha.setAt(1, 1, qCos(_alpha));
        Ralpha.setAt(1, 2, -qSin(_alpha));
        Ralpha.setAt(2, 1, qSin(_alpha));
        Ralpha.setAt(2, 2, qCos(_alpha));
        Rbeta.setAt(0, 0, qCos(_beta));
        Rbeta.setAt(0, 2, qSin(_beta));
        Rbeta.setAt(1, 1, 1.0);
        Rbeta.setAt(2, 0, -qSin(_beta));
        Rbeta.setAt(2, 2, qCos(_beta));

        Rall = Rbeta * Ralpha;

        //fill A and B matrix + w vector + right side
        for(int i = 0; i < numPoints; i++){

            _x = L0.getAt(i*3);
            _y = L0.getAt(i*3+1);
            _z = L0.getAt(i*3+2);

            a1 = _X0 + _x * qCos(_beta) + _y * qSin(_alpha) * qSin(_beta) + _z * qCos(_alpha) * qSin(_beta);
            a2 = _Y0 + _y * qCos(_alpha) - _z * qSin(_alpha);

            //A
            N.setAt(i, numPoints, 1.0);
            N.setAt(i, numPoints+1, -1.0 * a1 / qSqrt(a1*a1 + a2*a2));
            N.setAt(i, numPoints+2, -1.0 * a2 / qSqrt(a1*a1 + a2*a2));
            N.setAt(i, numPoints+3, -1.0 * ((_y * qSin(_beta) * qCos(_alpha) - _z * qSin(_beta) * qSin(_alpha)) * a1 - (_y * qSin(_alpha) + _z * qCos(_alpha)) * a2) / qSqrt(a1*a1 + a2*a2));
            N.setAt(i, numPoints+4, -1.0 * (_y * qSin(_alpha) * qCos(_beta) - _x * qSin(_beta) + _z * qCos(_alpha) * qCos(_beta)) * a1 / qSqrt(a1*a1 + a2*a2));

            //AT
            N.setAt(numPoints, i, 1.0);
            N.setAt(numPoints+1, i, -1.0 * a1 / qSqrt(a1*a1 + a2*a2));
            N.setAt(numPoints+2, i, -1.0 * a2 / qSqrt(a1*a1 + a2*a2));
            N.setAt(numPoints+3, i, -1.0 * ((_y * qSin(_beta) * qCos(_alpha) - _z * qSin(_beta) * qSin(_alpha)) * a1 - (_y * qSin(_alpha) + _z * qCos(_alpha)) * a2) / qSqrt(a1*a1 + a2*a2));
            N.setAt(numPoints+4, i, -1.0 * (_y * qSin(_alpha) * qCos(_beta) - _x * qSin(_beta) + _z * qCos(_alpha) * qCos(_beta)) * a1 / qSqrt(a1*a1 + a2*a2));

            //BBT
            N.setAt(i, i, (-1.0 * qCos(_beta) * a1 / qSqrt(a1*a1 + a2*a2))*(-1.0 * qCos(_beta) * a1 / qSqrt(a1*a1 + a2*a2))
                        + (-1.0 * (qSin(_alpha) * qSin(_beta) * a1 + qCos(_alpha) * a2) / qSqrt(a1*a1 + a2*a2))*(-1.0 * (qSin(_alpha) * qSin(_beta) * a1 + qCos(_alpha) * a2) / qSqrt(a1*a1 + a2*a2))
                        + (-1.0 * (qCos(_alpha) * qSin(_beta) * a1 - qSin(_alpha) * a2) / qSqrt(a1*a1 + a2*a2))*(-1.0 * (qCos(_alpha) * qSin(_beta) * a1 - qSin(_alpha) * a2) / qSqrt(a1*a1 + a2*a2)));

            _xr = Rall.getAt(0,0) * _x + Rall.getAt(0,1) * _y + Rall.getAt(0,2) * _z + _X0;
            _yr = Rall.getAt(1,0) * _x + Rall.getAt(1,1) * _y + Rall.getAt(1,2) * _z + _Y0;

            //approximate radius of the cylinder minus distance of point i to the z-axis is the contradiction
            diff = _r - qSqrt(_xr*_xr + _yr*_yr);

            c.setAt(i, diff);

        }

        //solve the normal equation system
        try{
            if(!OiMat::solve(res, N, -1.0*c)){
                return false;
            }
        }catch(const exception &e){
            emit this->sendMessage(QString("solve error cylinder fit: %1").arg(e.what()), eErrorMessage);
            return false;
        }

        //get new corrections for unknowns
        for(int i = numPoints; i < numPoints+5; i++){
            x.setAt(i-numPoints, res.getAt(i));
        }

        //calculate improvements
        for(int i = 0; i < numPoints; ++i){

            QPointer<Observation> obs = inputObservations.at(i);

            _x = obs->getXYZ().getAt(0);
            _y = obs->getXYZ().getAt(1);
            _z = obs->getXYZ().getAt(2);

            a1 = _X0 + _x * qCos(_beta) + _y * qSin(_alpha) * qSin(_beta) + _z * qCos(_alpha) * qSin(_beta);
            a2 = _Y0 + _y * qCos(_alpha) - _z * qSin(_alpha);

            v.setAt(i*3, (-1.0 * qCos(_beta) * a1 / qSqrt(a1*a1 + a2*a2)) * res.getAt(i));
            v.setAt(i*3+1, (-1.0 * (qSin(_alpha) * qSin(_beta) * a1 + qCos(_alpha) * a2) / qSqrt(a1*a1 + a2*a2)) * res.getAt(i));
            v.setAt(i*3+2, (-1.0 * (qCos(_alpha) * qSin(_beta) * a1 - qSin(_alpha) * a2) / qSqrt(a1*a1 + a2*a2)) * res.getAt(i));

        }

        //apply Armijo rule which is useful in case of bad approximations
        /*do{

            sigma = sigma / 2.0;

            _r_armijo = _r + sigma * x.getAt(0);
            _X0_armijo = _X0 + sigma * x.getAt(1);
            _Y0_armijo = _Y0 + sigma * x.getAt(2);
            _alpha_armijo = _alpha + sigma * x.getAt(3);
            _beta_armijo = _beta + sigma * x.getAt(4);

            for(int i = 0; i < 5; i++){
                _x = L0.getAt(i*3);
                _y = L0.getAt(i*3+1);
                _z = L0.getAt(i*3+2);

                a.setAt(i, _r_armijo - qSqrt( (_X0_armijo + _x*qCos(_beta_armijo) + _y*qSin(_alpha_armijo)*qSin(_beta_armijo) + _z*qCos(_alpha_armijo)*qSin(_beta_armijo))*(_X0_armijo + _x*qCos(_beta_armijo) + _y*qSin(_alpha_armijo)*qSin(_beta_armijo) + _z*qCos(_alpha_armijo)*qSin(_beta_armijo))
                                       + (_Y0_armijo + _y*qCos(_alpha_armijo) - _z*qSin(_alpha_armijo))*(_Y0_armijo + _y*qCos(_alpha_armijo) - _z*qSin(_alpha_armijo)) ));

                b.setAt(i, _r - qSqrt( (_X0 + _x*qCos(_beta) + _y*qSin(_alpha)*qSin(_beta) + _z*qCos(_alpha)*qSin(_beta))*(_X0 + _x*qCos(_beta) + _y*qSin(_alpha)*qSin(_beta) + _z*qCos(_alpha)*qSin(_beta))
                                               + (_Y0 + _y*qCos(_alpha) - _z*qSin(_alpha))*(_Y0 + _y*qCos(_alpha) - _z*qSin(_alpha)) ));
            }

            OiVec::dot(stopAA, a, a);
            OiVec::dot(stopBB, b, b);

        }while( stopAA > ( stopBB - 2.0 * 0.001 * sigma * stopBB ) );

        x = sigma * x;*/




        do{

            sigma = sigma / 2.0;

            _r_armijo = _r + sigma * x.getAt(0);
            _X0_armijo = _X0 + sigma * x.getAt(1);
            _Y0_armijo = _Y0 + sigma * x.getAt(2);
            _alpha_armijo = _alpha + sigma * x.getAt(3);
            _beta_armijo = _beta + sigma * x.getAt(4);

            for(int i = 0; i < numPoints; i++){
                _x = L0.getAt(i*3);
                _y = L0.getAt(i*3+1);
                _z = L0.getAt(i*3+2);

                _x_armijo = L0.getAt(i*3) + sigma * v.getAt(i*3);
                _y_armijo = L0.getAt(i*3+1) + sigma * v.getAt(i*3+1);
                _z_armijo = L0.getAt(i*3+2) + sigma * v.getAt(i*3+2);

                a.setAt(i, _r_armijo - qSqrt( (_X0_armijo + _x_armijo*qCos(_beta_armijo) + _y_armijo*qSin(_alpha_armijo)*qSin(_beta_armijo) + _z_armijo*qCos(_alpha_armijo)*qSin(_beta_armijo))*(_X0_armijo + _x_armijo*qCos(_beta_armijo) + _y_armijo*qSin(_alpha_armijo)*qSin(_beta_armijo) + _z_armijo*qCos(_alpha_armijo)*qSin(_beta_armijo))
                                       + (_Y0_armijo + _y_armijo*qCos(_alpha_armijo) - _z_armijo*qSin(_alpha_armijo))*(_Y0_armijo + _y_armijo*qCos(_alpha_armijo) - _z_armijo*qSin(_alpha_armijo)) ));

                b.setAt(i, _r - qSqrt( (_X0 + _x*qCos(_beta) + _y*qSin(_alpha)*qSin(_beta) + _z*qCos(_alpha)*qSin(_beta))*(_X0 + _x*qCos(_beta) + _y*qSin(_alpha)*qSin(_beta) + _z*qCos(_alpha)*qSin(_beta))
                                               + (_Y0 + _y*qCos(_alpha) - _z*qSin(_alpha))*(_Y0 + _y*qCos(_alpha) - _z*qSin(_alpha)) ));
            }

            OiVec::dot(stopAA, a, a);
            OiVec::dot(stopBB, b, b);

        }while( stopAA > stopBB );

        OiVec::dot(stopXX, x, x);
        OiVec::dot(stopVV, v, v);

        x = sigma * x;
        v = sigma * v;

        sigma = 2.0;

        numIterations++;

    //}while( (stopXX > 0.000000001) && (stopVV > 0.000000001) && numIterations < 1000 );
    //}while( (stopXX > 0.00000000001) && (stopVV > 0.00000000001) && numIterations < 1000 );
    //}while( ((stopXX > 0.00000000001) || (stopVV > 0.00000000001)) && numIterations < 1000 );
    //}while( ((stopXX > 0.0000000000001) || (stopVV > 0.00000000001)) && numIterations < 1000 );
    //}while(numIterations < 99);
    }while( stopXX > 0.0000000000001 && numIterations < 1000 );

    if(numIterations >= 1000){
        return false;
    }

    //apply corrections to unknowns
    _r += x.getAt(0);
    _X0 += x.getAt(1);
    _Y0 += x.getAt(2);
    _alpha += x.getAt(3);
    _beta += x.getAt(4);

    //calculate rotation matrices
    Ralpha.setAt(0, 0, 1.0);
    Ralpha.setAt(1, 1, qCos(_alpha));
    Ralpha.setAt(1, 2, -qSin(_alpha));
    Ralpha.setAt(2, 1, qSin(_alpha));
    Ralpha.setAt(2, 2, qCos(_alpha));
    Rbeta.setAt(0, 0, qCos(_beta));
    Rbeta.setAt(0, 2, qSin(_beta));
    Rbeta.setAt(1, 1, 1.0);
    Rbeta.setAt(2, 0, -qSin(_beta));
    Rbeta.setAt(2, 2, qCos(_beta));

    Rall = Rbeta * Ralpha;

    //calculate point on cylinder axis
    OiVec xyz(3);
    xyz.setAt(0, _X0);
    xyz.setAt(1, _Y0);
    xyz.setAt(2, 0.0);
    OiMat::solve(xyz, Rall, -1.0 * xyz);

    //calculate cylinder axis
    OiVec axis(3);
    axis.setAt(2, 1.0);
    OiMat::solve(axis, Rall, axis);

    //calculate sum vv
    double sumVV = 0.0;
    OiVec v_obs(3);
    for(int i = 0; i < numPoints; i++){

        QPointer<Observation> obs = inputObservations.at(i);

        _x = obs->getXYZ().getAt(0);
        _y = obs->getXYZ().getAt(1);
        _z = obs->getXYZ().getAt(2);

        float b[3]; //vector between point on cylinder axis and point p which is probably on cylinder
        b[0] = _x - xyz.getAt(0);
        b[1] = _y - xyz.getAt(1);
        b[2] = _z - xyz.getAt(2);

        float n0CrossB[3]; //cross product of cylinder axis (length 1) and b
        n0CrossB[0] = axis.getAt(1) * b[2] - axis.getAt(2) * b[1];
        n0CrossB[1] = axis.getAt(2) * b[0] - axis.getAt(0) * b[2];
        n0CrossB[2] = axis.getAt(0) * b[1] - axis.getAt(1) * b[0];

        float radiusActual = 0.0f; //smallest distance of point p to cylinder axis

        radiusActual = qSqrt( n0CrossB[0]*n0CrossB[0] + n0CrossB[1]*n0CrossB[1] + n0CrossB[2]*n0CrossB[2] );

        float distance = 0.0f;

        distance = radiusActual - _r; //distance error

        //calculate residual vector
        v_obs.setAt(0, b[0]);
        v_obs.setAt(1, b[1]);
        v_obs.setAt(2, b[2]);
        v_obs.normalize();
        v_obs = distance * v_obs;

        //set up display residuals
        Residual residual;
        residual.elementId = obs->getId();
        residual.dimension = eMetric;
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayVX), v_obs.getAt(0));
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayVY), v_obs.getAt(1));
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayVZ), v_obs.getAt(2));
        residual.corrections.insert(getObservationDisplayAttributesName(eObservationDisplayV), qSqrt(v_obs.getAt(0) * v_obs.getAt(0)
                                                                                                + v_obs.getAt(2) * v_obs.getAt(2)));
        this->statistic.addDisplayResidual(residual);

        sumVV += distance * distance;

    }

    //set up result
    Radius cylinderRadius(_r);
    Direction cylinderDirection(axis);
    Position cylinderPosition(xyz);
    cylinder.setCylinder(cylinderPosition, cylinderDirection, cylinderRadius);
    this->statistic.setIsValid(true);
    this->statistic.setStdev(qSqrt(sumVV / ((double)numPoints - 5.0)));
    cylinder.setStatistic(this->statistic);

    return true;

}
