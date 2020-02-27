#include "p_intersectlineline.h"

/*!
 * \brief IntersectLineLine::init
 */
void IntersectLineLine::init(){

    //set plugin meta data
    this->metaData.name = "IntersectLineLine";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "esc";
    this->metaData.description = QString("%1")
            .arg("This function calculates the intersection of a line with a line.");
    this->metaData.iid = ConstructFunction_iidd;

    //set needed elements
    NeededElement param1;
    param1.description = "Select a line to calculate the intersection.";
    param1.infinite = false;
    param1.typeOfElement = eLineElement;
    this->neededElements.append(param1);

    NeededElement param2;
    param2.description = "Select a line to calculate the intersection.";
    param2.infinite = false;
    param2.typeOfElement = eLineElement;
    this->neededElements.append(param2);

    //set spplicable for
    this->applicableFor.append(ePointFeature);

}

/*!
 * \brief IntersectLineLine::exec
 * \param point
 * \return
 */
bool IntersectLineLine::exec(Point &point){
    return this->setUpResult(point);
}


/*!
 * \brief IntersectLineLine::setUpResult
 * \param point
 * \return
 */
bool IntersectLineLine::setUpResult(Point &point){

    //get and check line and plane that shall be intersected
    if(!this->inputElements.contains(0) || this->inputElements[0].size() != 1
            || !this->inputElements.contains(1) || this->inputElements[1].size() != 1){
        return false;
    }
    QPointer<Line> line1 = this->inputElements[0].at(0).line;
    QPointer<Line> line2 = this->inputElements[1].at(0).line;
    if(line1.isNull() || line2.isNull() || !line1->getIsSolved() || !line2->getIsSolved()){
        return false;
    }

    //check if line and plane intersect
    if(!this->isIntersection(line2, line2)){
        emit this->sendMessage(QString("No intersection between line %1 and line %2")
                               .arg(line1->getFeatureName())
                               .arg(line2->getFeatureName()), eWarningMessage);
        return false;
    }

    //perform intersection

    // TODO
    OiVec s;


    //set result
    Position section;
    section.setVector(s);
    point.setPoint(section);

    return true;

}

/*!
 * \brief IntersectLineLine::isIntersection
 * \param line
 * \param plane
 * \return
 */
bool IntersectLineLine::isIntersection(const QPointer<Line> &line1, const QPointer<Line> &line2){

    //scalar product of the lines's direction vector and the line's normal vector
    double dot = 0.0;
    OiVec::dot(dot, line1->getDirection().getVector(), line2->getDirection().getVector());
    return dot > 0.0001 || dot < -0.0001;

}
