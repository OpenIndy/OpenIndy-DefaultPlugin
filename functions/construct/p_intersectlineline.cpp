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

    if(this->isParallel(line1, line2)){
        emit this->sendMessage(QString("line %1 and line %2 are parallel")
                               .arg(line1->getFeatureName())
                               .arg(line2->getFeatureName()), eWarningMessage);
        return false;
    }

    //perform intersection
    OiVec a = line1->getPosition().getVector();
    OiVec b = line2->getPosition().getVector();
    OiVec v = line1->getDirection().getVector();
    OiVec w = line2->getDirection().getVector();

    OiVec cvw, n1, n2;
    OiVec::cross(cvw, v, w);
    OiVec::cross(n1, v, cvw);
    OiVec::cross(n2, w, cvw);

    double dan1;
    double dbn1;
    double dwn1;
    OiVec::dot(dan1, a, n1);
    OiVec::dot(dbn1, b, n1);
    OiVec::dot(dwn1, w, n1);

    double dbn2;
    double dan2;
    double dvn2;
    OiVec::dot(dbn2, b, n2);
    OiVec::dot(dan2, a, n2);
    OiVec::dot(dvn2, v, n2);

    OiVec fline1 = (dbn2 - dan2) / dvn2 * v + a;
    OiVec fline2 = (dan1 - dbn1) / dwn1 * w + b;

    // TODO
    OiVec s;
    if(true) { // at line1
        s = fline1;
    } else { // at line2
        s = fline2;
    }
    // TODO at between fg & fh

    // TODO show / log distance between fg & fh
    // TODO show exact intersection: distance -> 0


    //set result
    Position section;
    section.setVector(s);
    point.setPoint(section);

    return true;

}

/*!
 * \brief IntersectLineLine::isParallel
 * \param line
 * \param plane
 * \return
 */
bool IntersectLineLine::isParallel(const QPointer<Line> &line1, const QPointer<Line> &line2){

    //scalar product of the lines's direction vector
    double dot = 0.0;
    OiVec::dot(dot, line1->getDirection().getVector(), line2->getDirection().getVector());
    return dot > 0.999;

}
