#include "p_spherefrompoints.h"

/*!
 * \brief SphereFromPoints::init
 */
void SphereFromPoints::init(){

    //set plugin meta data
    this->metaData.name = "SphereFromPoints";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "kern";
    this->metaData.description = QString("%1 %2")
            .arg("This function caclulates an adjusted sphere.")
            .arg("You can input as many points as you want which are then used to find the best fit sphere.");
    this->metaData.iid = ConstructFunction_iidd;

    //set needed elements
    NeededElement param1;
    param1.description = "Select at least four points to calculate the best fit sphere.";
    param1.infinite = true;
    param1.typeOfElement = ePointElement;
    this->neededElements.append(param1);

    //set spplicable for
    this->applicableFor.append(eSphereFeature);

}

/*!
 * \brief SphereFromPoints::exec
 * \param sphere
 * \return
 */
bool SphereFromPoints::exec(Sphere &sphere){
    return this->setUpResult(sphere);
}

/*!
 * \brief SphereFromPoints::setUpResult
 * \param sphere
 * \return
 */
bool SphereFromPoints::setUpResult(Sphere &sphere){

    //get and check input points
    if(!this->inputElements.contains(0) || this->inputElements[0].size() < 4){
        emit this->sendMessage(QString("Not enough valid points to fit the sphere %1").arg(sphere.getFeatureName()), eWarningMessage);
        return false;
    }
    QList<QPointer<Point> > inputPoints;
    foreach(const InputElement &element, this->inputElements[0]){
        if(!element.point.isNull() && element.point->getIsSolved()){
            inputPoints.append(element.point);
            this->setIsUsed(0, element.id, true);
        }
        this->setIsUsed(0, element.id, false);
    }
    if(inputPoints.size() < 4){
        emit this->sendMessage(QString("Not enough valid points to fit the sphere %1").arg(sphere.getFeatureName()), eWarningMessage);
        return false;
    }

    //calculate centroid coordinates
    OiVec centroid(3);
    for(int i = 0; i < inputPoints.size(); i++){
        centroid = centroid + inputPoints.at(i)->getPosition().getVector();
    }
    centroid = centroid / inputPoints.size();

    //Drixler
    OiMat N(4, 4);
    OiVec n(4);
    for(int i = 0; i < inputPoints.size(); i++){
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;

        x = inputPoints.at(i)->getPosition().getVector().getAt(0) - centroid.getAt(0);
        y = inputPoints.at(i)->getPosition().getVector().getAt(1) - centroid.getAt(1);
        z = inputPoints.at(i)->getPosition().getVector().getAt(2) - centroid.getAt(2);

        double xx = x*x;
        double yy = y*y;
        double zz = z*z;
        double xy = x*y;
        double xz = x*z;
        double yz = y*z;
        double xyz2 = x*x + y*y + z*z;

        N.setAt(0,0, N.getAt(0,0) + xx);
        N.setAt(0,1, N.getAt(0,1) + xy);
        N.setAt(0,2, N.getAt(0,2) + xz);
        N.setAt(0,3, N.getAt(0,3) + x);
        N.setAt(1,0, N.getAt(1,0) + xy);
        N.setAt(1,1, N.getAt(1,1) + yy);
        N.setAt(1,2, N.getAt(1,2) + yz);
        N.setAt(1,3, N.getAt(1,3) + y);
        N.setAt(2,0, N.getAt(2,0) + xz);
        N.setAt(2,1, N.getAt(2,1) + yz);
        N.setAt(2,2, N.getAt(2,2) + zz);
        N.setAt(2,3, N.getAt(2,3) + z);
        N.setAt(3,0, N.getAt(3,0) + x);
        N.setAt(3,1, N.getAt(3,1) + y);
        N.setAt(3,2, N.getAt(3,2) + z);
        N.setAt(3,3, N.getAt(3,3) + 1.0);

        n.setAt(0, n.getAt(0) + x*xyz2);
        n.setAt(1, n.getAt(1) + y*xyz2);
        n.setAt(2, n.getAt(2) + z*xyz2);
        n.setAt(3, n.getAt(3) + xyz2);
    }

    OiMat Q(4,4);
    try{
        Q = N.inv();
    }catch(exception &e){
        emit this->sendMessage(e.what(), eErrorMessage);
        return false;
    }

    OiVec a = -1.0 * Q * n;

    double r = 0.0;
    double xm = 0.0;
    double ym = 0.0;
    double zm = 0.0;

    r = qSqrt( qAbs( 0.25 * (a.getAt(0)*a.getAt(0) + a.getAt(1)*a.getAt(1) + a.getAt(2)*a.getAt(2)) - a.getAt(3) ) );
    xm = -0.5 * a.getAt(0) + centroid.getAt(0);
    ym = -0.5 * a.getAt(1) + centroid.getAt(1);
    zm = -0.5 * a.getAt(2) + centroid.getAt(2);

    //set approximate result
    Position position;
    position.setVector(xm, ym, zm);
    Radius radius;
    radius.setRadius(r);
    sphere.setSphere(position, radius);

    return true;

}
