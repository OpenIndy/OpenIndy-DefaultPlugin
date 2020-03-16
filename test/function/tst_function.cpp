#include <QString>
#include <QtTest>
#include <QPointer>
#include <QList>

#include "p_register.h"
#include "p_bestfitcylinder.h"
#include "p_bestfitcylinderfrompoints.h"
#include "p_intersectlineline.h"
#include "p_rectifytopoint.h"
#include "p_rectifytovector.h"
#include "featurewrapper.h"
#include "types.h"
#include "chooselalib.h"
#include "p_bestfitplane.h"
#include "p_bestfitpoint.h"
#include "p_bestfitcircleinplane.h"
#include "p_bestfitcircleinplanefrompoints.h"
#include "p_bestfitline.h"
#include "p_bestfitsphere.h"
#include "p_pointfrompoints.h"
#include "p_register.h"
#include "p_factory.h"

#define COMPARE_DOUBLE(actual, expected, threshold) QVERIFY2(std::abs(actual-expected)< threshold, QString("actual: %1, expected: %2").arg(actual).arg(expected).toLatin1().data());
#define _OI_VEC(v) v.getAt(0) << "," << v.getAt(1) << "," << v.getAt(2)
#define DEBUG_CYLINDER(feature) qDebug() << qSetRealNumberPrecision(10) << "position=" << _OI_VEC(feature->getPosition().getVector()) << ", direction=" << _OI_VEC(feature->getDirection().getVector()) << ", radius=" << feature->getRadius().getRadius() << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_CIRCLE(feature)   qDebug() << qSetRealNumberPrecision(10) << "position=" << _OI_VEC(feature->getPosition().getVector()) << ", direction=" << _OI_VEC(feature->getDirection().getVector()) << ", radius=" << feature->getRadius().getRadius() << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_SPHERE(feature)   qDebug() << qSetRealNumberPrecision(10) << "position=" << _OI_VEC(feature->getPosition().getVector()) << ", direction=" << _OI_VEC(feature->getDirection().getVector()) << ", radius=" << feature->getRadius().getRadius() << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_PLANE(feature)    qDebug() << qSetRealNumberPrecision(10) << "position=" << _OI_VEC(feature->getPosition().getVector()) << ", direction=" << _OI_VEC(feature->getDirection().getVector()) << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_POINT(feature)    qDebug() << qSetRealNumberPrecision(10) << "position=" << _OI_VEC(feature->getPosition().getVector()) << ", direction=" << _OI_VEC(feature->getDirection().getVector()) << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_LINE(feature)     qDebug() << qSetRealNumberPrecision(10) << "position=" << _OI_VEC(feature->getPosition().getVector()) << ", direction=" << _OI_VEC(feature->getDirection().getVector()) << ", stdev=" << feature->getStatistic().getStdev();

using namespace oi;

class FunctionTest : public QObject
{
    Q_OBJECT

public:
    FunctionTest();

private Q_SLOTS:
    void testPointFromPoints_Register2();
    void testPointFromPoints_Register();

    void testPointFromPoints_point();
    void testPointFromPoints_circle();

    void testBestFitSphere_residuals();
    void testBestFitCircleInPlane_residuals2();
    void testBestFitCircleInPlane_residuals();
    void testBestFitLine_residuals();
    void testBestFitPlane_residuals();
    void testBestFitPoint_residuals();

    void testVRadial();
    void testVRadial2();
    void testVRadial3();

    void testCircleInPlaneFromPoints();

    void testRectifyToVector_PlaneToCoodinateSystem_yAxis();
    void testRectifyToVector_PlaneToCoodinateSystem_xAxis();
    void testRectifyToVector_PlaneToCoodinateSystem_zAxis();

    void testRectifyToVector_CircleToStation();
    void testRectifyToVector_LineToStation();
    void testRectifyToVector_CylinderToStation();
    void testRectifyToVector_PlaneToStation();
    void testRectifyToVector_PlaneToPlane_negative();

    void testRectifyToPoint_Circle_negative();
    void testRectifyToPoint_Line_negative();
    void testRectifyToPoint_Cylinder_negative();
    void testRectifyToPoint_Plane_Station();
    void testRectifyToPoint_Plane_negative();
    void testRectifyToPoint_Plane_positive();

    void testIntersectLineLine_intersect2_atfirstline();
    void testIntersectLineLine_intersect2_atsecondline();
    void testIntersectLineLine_intersect2_midpoint();
    void testIntersectLineLine_intersect1_atfirstline();
    void testIntersectLineLine_intersect1_atsecondline();
    void testIntersectLineLine_intersect1_midpoint();
    void testIntersectLineLine_parallel();

    void testDisableAllObservationsButLastOne_2side_yes();

    void testBestFitCylinder1__DummyPoint1();
    void testBestFitCylinder1__DummyPoint2();

    void testBestFitCircleInPlane_DummyPoint1();
    void testBestFitCircleInPlane_DummyPoint2();
    void testBestFitCircleInPlane();

    void testBestFitPlane_DummyPoint1();
    void testBestFitPlane_DummyPoint2();

    void testBestFitCylinderAproximationDirection1();

    void testRegisterPoint();
    void testRegisterSphere();
    void testRegisterCircle();

    // OI-566
    void testBestFitCylinder1();
    void testBestFitCylinder1_modifyX();
    void testBestFitCylinder1_modifyZ();
    void testBestFitCylinder1_modifyY();

    // OI-298
    void testBestFitCylinder2_trafo_guess_axis_3();
    void testBestFitCylinder2_trafo_1st_2_pts();
    void testBestFitCylinder2_trafo_guess_axis_2();
    void testBestFitCylinder2_trafo_guess_axis_1();
    void testBestFitCylinder2_guess_axis();
    void testBestFitCylinder2_1st_2_pts();


    void testDisableAllObservationsButLastOne_no();
    void testDisableAllObservationsButLastOne_yes();

    void testBestFitPlane();

    void printMessage(const QString &msg, const MessageTypes &msgType, const MessageDestinations &msgDest = eConsoleMessage);

private:
    QPointer<Function> createFunction(QString name);

    void addInputObservations(QString data, QPointer<Function> function, int id, int inputElementKey, bool shouldBeUsed);

    void addInputLine(double x, double y, double z, double i, double j, double k, QPointer<Function> function, int id, int inputElementKey);
    void addInputStation(double x, double y, double z, double i, double j, double k, QPointer<Function> function, int id, int inputElementKey);
    void addInputCoordinateSystem(double x, double y, double z, double i, double j, double k, QPointer<Function> function, int id, int inputElementKey);
    void addInputCoordinateSystem(OiMat trafo, QPointer<Function> function, int id, int inputElementKey);
    void addInputPoint(double x, double y, double z, QPointer<Function> function, int id, int inputElementKey);
    void addInputPoints(QString data, QPointer<Function> function, int id, int inputElementKey);
    void addInputPlane(double x, double y, double z, double i, double j, double k, QPointer<Function> function, int id, int inputElementKey);
    void addInputCircle(double x, double y, double z, double i, double j, double k, double radius, QPointer<Function> function, int id, int inputElementKey);

    QPointer<Plane> createPlane(double x, double y, double z, double i, double j, double k);
    QPointer<Cylinder> createCylinder(double x, double y, double z, double i, double j, double k, double r);
    QPointer<Circle> createCircle(double x, double y, double z, double i, double j, double k, double r);
    QPointer<Line> createLine(double x, double y, double z, double i, double j, double k);

};

FunctionTest::FunctionTest()
{
}

void FunctionTest::printMessage(const QString &msg, const MessageTypes &msgType, const MessageDestinations &msgDest) {
    qDebug() << msg;
}

void FunctionTest::addInputObservations(QString data, QPointer<Function> function, int id = 1000, int inputElementKey = InputElementKey::eDefault, bool shouldBeUsed = true) {
    QTextStream stream(data.toUtf8());
    while(!stream.atEnd()) {
        QStringList xyz = stream.readLine().split(" ");
        if(xyz.size() == 0) {
            continue;
        }

        OiVec * vec = new OiVec(4);
        vec->setAt(0, xyz.at(0).toDouble());
        vec->setAt(1, xyz.at(1).toDouble());
        vec->setAt(2, xyz.at(2).toDouble());
        vec->setAt(3, 1.0);

        ReadingCartesian * readingCartesian = new ReadingCartesian();
//        readingCartesian->xyz.setAt(0, xyz.at(0).toDouble());
//        readingCartesian->xyz.setAt(1, xyz.at(1).toDouble());
//        readingCartesian->xyz.setAt(2, xyz.at(2).toDouble());
        readingCartesian->isValid = true;

        Observation * observation = new Observation(*vec, id, true);
        observation->setIsSolved(true);

        Reading * reading = new Reading(*readingCartesian);
        if(xyz.size() > 3) {
            reading->setSensorFace(xyz.at(3).compare("FS") == 0 ? SensorFaces::eFrontSide : SensorFaces::eBackSide);
        }
        reading->setObservation(observation);

        InputElement * element = new InputElement(id);
        element->typeOfElement = eObservationElement;
        element->observation = observation;
        element->shouldBeUsed = shouldBeUsed;


        function->addInputElement(*element, inputElementKey);

        id++;
     }
}

void FunctionTest::addInputPoints(QString data, QPointer<Function> function, int id = 1000, int inputElementKey = InputElementKey::eDefault) {
    QTextStream stream(data.toUtf8());
    while(!stream.atEnd()) {
        QStringList xyz = stream.readLine().split(" ");
        if(xyz.size() == 0) {
            continue;
        }

        QPointer<Point> feature = new Point(id, false, Position( xyz.at(0).toDouble(),  xyz.at(1).toDouble(),  xyz.at(2).toDouble()));
        feature->setIsSolved(true);
        feature->setFeatureName(QString("point_%1").arg(id));

        InputElement * element = new InputElement(id);
        element->typeOfElement = ePointElement;
        element->point = feature;
        element->geometry = feature;

        function->addInputElement(*element, inputElementKey);

        id++;
     }
}

void FunctionTest::addInputLine(double x, double y, double z, double i, double j, double k, QPointer<Function> function, int id = 2000, int inputElementKey = InputElementKey::eDefault){
    OiVec * p = new OiVec(4);
    p->setAt(0, x);
    p->setAt(1, y);
    p->setAt(2, z);
    p->setAt(3, 1.0);
    Position * xyz = new Position(*p);

    OiVec * a = new OiVec(4);
    a->setAt(0, i);
    a->setAt(1, j);
    a->setAt(2, k);
    a->setAt(3, 1.0);
    Direction * ijk = new Direction(*a);

    Line * line = new Line(false, *xyz, *ijk);
    line->setIsSolved(true);
    line->setFeatureName(QString("line_%1").arg(id));

    InputElement * element = new InputElement(id);
    element->typeOfElement = eLineElement;
    element->line = line;
    element->geometry = line;

    function->addInputElement(*element, inputElementKey);
}

void FunctionTest::addInputStation(double x, double y, double z, double i, double j, double k, QPointer<Function> function, int id = 2000, int inputElementKey = InputElementKey::eDefault){
    OiVec * p = new OiVec(4);
    p->setAt(0, x);
    p->setAt(1, y);
    p->setAt(2, z);
    p->setAt(3, 1.0);
    Position * xyz = new Position(*p);

    OiVec * a = new OiVec(4);
    a->setAt(0, i);
    a->setAt(1, j);
    a->setAt(2, k);
    a->setAt(3, 1.0);
    Direction * ijk = new Direction(*a);

    QPointer<Station> station = new Station(QString("station_%1").arg(id));
    station->setCoordinateSystem(*xyz, Direction(0,0,0), Direction(0,0,0), *ijk);
    station->setIsSolved(true);

    InputElement * element = new InputElement(id);
    element->typeOfElement = eStationElement;
    element->station = station;

    function->addInputElement(*element, inputElementKey);
}

void FunctionTest::addInputCoordinateSystem(double x, double y, double z, double i, double j, double k, QPointer<Function> function, int id = 2000, int inputElementKey = InputElementKey::eDefault){
    OiVec * p = new OiVec(4);
    p->setAt(0, x);
    p->setAt(1, y);
    p->setAt(2, z);
    p->setAt(3, 1.0);
    Position * xyz = new Position(*p);

    OiVec * a = new OiVec(4);
    a->setAt(0, i);
    a->setAt(1, j);
    a->setAt(2, k);
    a->setAt(3, 1.0);
    Direction * ijk = new Direction(*a);

    QPointer<CoordinateSystem> coordianteSystem = new CoordinateSystem();
    coordianteSystem->setFeatureName(QString("system_%1").arg(id));
    coordianteSystem->setCoordinateSystem(*xyz, Direction(0,0,0), Direction(0,0,0), *ijk);
    coordianteSystem->setIsSolved(true);

    InputElement * element = new InputElement(id);
    element->typeOfElement = eCoordinateSystemElement;
    element->coordSystem = coordianteSystem;

    function->addInputElement(*element, inputElementKey);
}

void FunctionTest::addInputCoordinateSystem(OiMat trafo, QPointer<Function> function, int id = 2000, int inputElementKey = InputElementKey::eDefault){
    QPointer<CoordinateSystem> coordianteSystem = new CoordinateSystem();
    coordianteSystem->setFeatureName(QString("system_%1").arg(id));
    coordianteSystem->transformOriginAndAxis(trafo);
    coordianteSystem->setIsSolved(true);

    InputElement * element = new InputElement(id);
    element->typeOfElement = eCoordinateSystemElement;
    element->coordSystem = coordianteSystem;

    function->addInputElement(*element, inputElementKey);
}

void FunctionTest::addInputPoint(double x, double y, double z, QPointer<Function> function, int id = 2000, int inputElementKey = InputElementKey::eDefault) {
    OiVec pointPos = OiVec(3);
    pointPos.setAt(0, x);
    pointPos.setAt(1, y);
    pointPos.setAt(2, z);
    QPointer<Point> feature = new Point(false, Position(pointPos));
    feature->setIsSolved(true);
    feature->setFeatureName(QString("point_%1").arg(id));

    InputElement * element = new InputElement(id);
    element->typeOfElement = ePointElement;
    element->point = feature;
    element->geometry = feature;

    function->addInputElement(*element, inputElementKey);
}

void FunctionTest::addInputCircle(double x, double y, double z, double i, double j, double k, double radius, QPointer<Function> function, int id = 2000, int inputElementKey = InputElementKey::eDefault){
    OiVec * p = new OiVec(4);
    p->setAt(0, x);
    p->setAt(1, y);
    p->setAt(2, z);
    p->setAt(3, 1.0);
    Position * xyz = new Position(*p);

    OiVec * a = new OiVec(4);
    a->setAt(0, i);
    a->setAt(1, j);
    a->setAt(2, k);
    a->setAt(3, 1.0);
    Direction * ijk = new Direction(*a);

    Radius *r = new Radius();
    r->setRadius(radius);

    Circle * feature = new Circle(false, *xyz, *ijk, *r);
    feature->setIsSolved(true);
    feature->setFeatureName(QString("circle_%1").arg(id));

    InputElement * element = new InputElement(id);
    element->typeOfElement = eCircleElement;
    element->circle = feature;
    element->geometry = feature;

    function->addInputElement(*element, inputElementKey);
}

void FunctionTest::addInputPlane(double x, double y, double z, double i, double j, double k, QPointer<Function> function, int id = 2000, int inputElementKey = InputElementKey::eDefault){
    OiVec * p = new OiVec(4);
    p->setAt(0, x);
    p->setAt(1, y);
    p->setAt(2, z);
    p->setAt(3, 1.0);
    Position * xyz = new Position(*p);

    OiVec * a = new OiVec(4);
    a->setAt(0, i);
    a->setAt(1, j);
    a->setAt(2, k);
    a->setAt(3, 1.0);
    Direction * ijk = new Direction(*a);

    Plane * feature = new Plane(false, *xyz, *ijk);
    feature->setIsSolved(true);
    feature->setFeatureName(QString("plane_%1").arg(id));

    InputElement * element = new InputElement(id);
    element->typeOfElement = eLineElement;
    element->plane = feature;
    element->geometry = feature;

    function->addInputElement(*element, inputElementKey);
}

QPointer<Plane> FunctionTest::createPlane(double x, double y, double z, double i, double j, double k) {
    OiVec * p = new OiVec(4);
    p->setAt(0, x);
    p->setAt(1, y);
    p->setAt(2, z);
    p->setAt(3, 1.0);
    Position * xyz = new Position(*p);

    OiVec * d = new OiVec(4);
    d->setAt(0, i);
    d->setAt(1, j);
    d->setAt(2, k);
    d->setAt(3, 1.0);
    Direction * ijk = new Direction(*d);
    QPointer<Plane> feature = new Plane(false, *xyz, *ijk);
    return feature;
}

QPointer<Cylinder> FunctionTest::createCylinder(double x, double y, double z, double i, double j, double k, double r) {
    OiVec * p = new OiVec(4);
    p->setAt(0, x);
    p->setAt(1, y);
    p->setAt(2, z);
    p->setAt(3, 1.0);
    Position * xyz = new Position(*p);

    OiVec * d = new OiVec(4);
    d->setAt(0, i);
    d->setAt(1, j);
    d->setAt(2, k);
    d->setAt(3, 1.0);
    Direction * ijk = new Direction(*d);
    QPointer<Cylinder> feature = new Cylinder(false, *xyz, *ijk, Radius(r));
    return feature;
}

QPointer<Circle> FunctionTest::createCircle(double x, double y, double z, double i, double j, double k, double r) {
    OiVec * p = new OiVec(4);
    p->setAt(0, x);
    p->setAt(1, y);
    p->setAt(2, z);
    p->setAt(3, 1.0);
    Position * xyz = new Position(*p);

    OiVec * d = new OiVec(4);
    d->setAt(0, i);
    d->setAt(1, j);
    d->setAt(2, k);
    d->setAt(3, 1.0);
    Direction * ijk = new Direction(*d);
    QPointer<Circle> feature = new Circle(false, *xyz, *ijk, Radius(r));
    return feature;
}

QPointer<Line> FunctionTest::createLine(double x, double y, double z, double i, double j, double k) {
    OiVec * p = new OiVec(4);
    p->setAt(0, x);
    p->setAt(1, y);
    p->setAt(2, z);
    p->setAt(3, 1.0);
    Position * xyz = new Position(*p);

    OiVec * d = new OiVec(4);
    d->setAt(0, i);
    d->setAt(1, j);
    d->setAt(2, k);
    d->setAt(3, 1.0);
    Direction * ijk = new Direction(*d);
    QPointer<Line> feature = new Line(false, *xyz, *ijk);
    return feature;
}

void FunctionTest::testRegisterPoint()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new Register();
    function->init();

    OiVec pointPos = OiVec(3);
    pointPos.setAt(0, 999.9927);
    pointPos.setAt(1, 1999.9898);
    pointPos.setAt(2, 2999.9817);
    QPointer<Point> point = new Point(false, Position(pointPos));
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);

    addInputPlane(1374.9964, 1624.9982, 1024.7504, 0.100818, -0.097854, 0.990081, function);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    OiVec xyz = point->getPosition().getVector();
    QCOMPARE(xyz.getAt(0), 810.34019818328215);
    QCOMPARE(xyz.getAt(1), 2184.0666108152623);
    QCOMPARE(xyz.getAt(2), 1137.5033860957583);

    delete point.data();
    delete pointFeature.data();
    delete function.data();
}

void FunctionTest::testRegisterSphere()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new Register();
    function->init();

    OiVec spherePos = OiVec(3);
    spherePos.setAt(0, -25.0146);
    spherePos.setAt(1, -25.0191);
    spherePos.setAt(2, -25.0159);
    Radius radius(1025.6284);
    QPointer<Sphere> sphere = new Sphere(false, Position(spherePos), radius);
    QPointer<FeatureWrapper> sphereFeature = new FeatureWrapper();
    sphereFeature->setSphere(sphere);

    addInputPlane(1374.9964, 1624.9982, 1024.7504, 0.100818, -0.097854, 0.990081, function);

    bool res = function->exec(sphereFeature);
    QVERIFY2(res, "exec");

    OiVec xyz = sphere->getPosition().getVector();
    QCOMPARE(xyz.getAt(0), 77.722886204991397);
    QCOMPARE(xyz.getAt(1), -124.73615424728945);
    QCOMPARE(xyz.getAt(2), 983.91537298026219);

    delete sphere.data();
    delete sphereFeature.data();
    delete function.data();
}

void FunctionTest::testRegisterCircle()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new Register();
    function->init();

    QPointer<Circle> circle = createCircle(600.0108, 499.9982, 999.9979, 0.000010, 0.000007, -1.000000, 781.0312);

    QPointer<FeatureWrapper> circleFeature = new FeatureWrapper();
    circleFeature->setCircle(circle);

    addInputPlane(1374.9964, 1624.9982, 1024.7504, 0.100818, -0.097854, 0.990081, function);

    bool res = function->exec(circleFeature);
    QVERIFY2(res, "exec");

    OiVec xyz = circle->getPosition().getVector();
    QCOMPARE(xyz.getAt(0), 599.2600818127703);
    QCOMPARE(xyz.getAt(1), 500.72684743888169);
    QCOMPARE(xyz.getAt(2), 992.62548819327355);

    delete circle.data();
    delete circleFeature.data();
    delete function.data();
}

// OI-494
void FunctionTest::testVRadial()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
-3283.654 -79.927 194.917\n\
-3271.578 -84.991 203.643\n\
-3292.599 -64.647 196.517\n\
-3308.806 -73.417 213.805\n\
-3301.555 -87.819 210.971\n\
-3289.428 -79.973 198.027\n\
-3292.505 -57.849 200.595\n\
-3303.342 -63.362 213.794\n\
");


    addInputObservations(data, function);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "guess axis");
    function->setScalarInputParams(scalarInputParams);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1000).corrections.value("vr", -1),   0.026, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("vr", -1), (-0.007), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("vr", -1), (-0.037), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1003).corrections.value("vr", -1),   0.010, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1004).corrections.value("vr", -1), (-0.003), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1005).corrections.value("vr", -1), (-0.007), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1006).corrections.value("vr", -1),   0.030, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1007).corrections.value("vr", -1), (-0.012), 0.001);

    COMPARE_DOUBLE(function->getStatistic().getFormError(), 0.067, 0.001);

    delete function.data();
}

// OI-637
void FunctionTest::testVRadial2()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinderFromPoints();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
-3283.654 -79.927 194.917\n\
-3271.578 -84.991 203.643\n\
-3292.599 -64.647 196.517\n\
-3308.806 -73.417 213.805\n\
-3301.555 -87.819 210.971\n\
-3289.428 -79.973 198.027\n\
-3292.505 -57.849 200.595\n\
-3303.342 -63.362 213.794\n\
");


    addInputPoints(data, function);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "guess axis");
    function->setScalarInputParams(scalarInputParams);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1000).corrections.value("vr", -1),   0.026, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("vr", -1), (-0.007), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("vr", -1), (-0.037), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1003).corrections.value("vr", -1),   0.010, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1004).corrections.value("vr", -1), (-0.003), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1005).corrections.value("vr", -1), (-0.007), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1006).corrections.value("vr", -1),   0.030, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1007).corrections.value("vr", -1), (-0.012), 0.001);

    COMPARE_DOUBLE(function->getStatistic().getFormError(), 0.067, 0.001);

    delete function.data();
}
void FunctionTest::testVRadial3() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
-3283.654 -79.927 194.917\n\
-3271.578 -84.991 203.643\n\
-3292.599 -64.647 196.517\n\
-3308.806 -73.417 213.805\n\
-3301.555 -87.819 210.971\n\
-3289.428 -79.973 198.027\n\
-3292.505 -57.849 200.595\n\
-3303.342 -63.362 213.794\n\
");


    addInputObservations(data, function);
    addInputObservations("-3305.0 -65.0 15.", function, 5000, InputElementKey::eDefault, false);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "guess axis");
    function->setScalarInputParams(scalarInputParams);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1000).corrections.value("vr", -1),   0.026, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("vr", -1), (-0.007), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("vr", -1), (-0.037), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1003).corrections.value("vr", -1),   0.010, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1004).corrections.value("vr", -1), (-0.003), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1005).corrections.value("vr", -1), (-0.007), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1006).corrections.value("vr", -1),   0.030, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1007).corrections.value("vr", -1), (-0.012), 0.001);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(5000).corrections.value("vr", -1), (140.563), 0.001);

    COMPARE_DOUBLE(function->getStatistic().getFormError(), 0.067, 0.001);

    delete function.data();
}

// OI-566
void FunctionTest::testBestFitCylinder1()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);


    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);


    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
900 19.05 -1205\n\
900 19.05 0\n\
900 19.05 1205\n\
700 12.34 -1205\n\
700 12.34 0\n\
700 12.34 1205\n\
200 12.34 -1205\n\
200 12.34 0\n\
200 12.34 1205\n\
0 19.05 -1205\n\
0 19.05 0\n\
0 19.05 1205\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(2), 1.0, 0.00001);
    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 10438.54, 0.01);

    delete function.data();
}

// OI-566
void FunctionTest::testBestFitCylinder1_modifyX()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
901 19.05 -1205\n\
900 19.05 0\n\
900 19.05 1205\n\
700 12.34 -1205\n\
700 12.34 0\n\
700 12.34 1205\n\
200 12.34 -1205\n\
200 12.34 0\n\
200 12.34 1205\n\
0 19.05 -1205\n\
0 19.05 0\n\
0 19.05 1205\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(2), 1.0, 0.00001);
    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 10449.77, 0.01);

    delete function.data();
}

// OI-566
void FunctionTest::testBestFitCylinder1_modifyZ()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
900 19.05 -1206\n\
900 19.05 0\n\
900 19.05 1205\n\
700 12.34 -1205\n\
700 12.34 0\n\
700 12.34 1205\n\
200 12.34 -1205\n\
200 12.34 0\n\
200 12.34 1205\n\
0 19.05 -1205\n\
0 19.05 0\n\
0 19.05 1205\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(2), 1.0, 0.00001);
    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 10438.54, 0.01);

    delete function.data();
}

// OI-566
void FunctionTest::testBestFitCylinder1_modifyY()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
900 20.05 -1205\n\
900 19.05 0\n\
900 19.05 1205\n\
700 12.34 -1205\n\
700 12.34 0\n\
700 12.34 1205\n\
200 12.34 -1205\n\
200 12.34 0\n\
200 12.34 1205\n\
0 19.05 -1205\n\
0 19.05 0\n\
0 19.05 1205\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(2), 1.0, 0.00001);
    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 10185.71, 0.01);

    delete function.data();
}

// OI-298
void FunctionTest::testBestFitCylinder2_trafo_guess_axis_3()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    // Station 4
    QString data = "\
256.39 1460.15 1326.66\n\
264.47 1449.69 1335.19\n\
256.13 1477.83 1328.50\n\
237.81 1477.93 1345.91\n\
237.02 1461.86 1342.85\n\
251.36 1462.91 1329.84\n\
259.58 1483.65 1332.64\n\
247.51 1484.00 1345.92\n\
";
    addInputObservations(data, function);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "guess axis");
    function->setScalarInputParams(scalarInputParams);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-298
void FunctionTest::testBestFitCylinder2_trafo_1st_2_pts()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
340.00 1467.40 1096.57\n\
352.00 1466.29 1083.28\n\
342.47 1446.13 1080.47\n\
328.11 1445.99 1093.50\n\
329.93 1461.97 1096.57\n\
348.18 1460.71 1079.14\n\
354.71 1432.09 1085.80\n\
347.31 1443.06 1077.28\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-298
void FunctionTest::testBestFitCylinder2_trafo_guess_axis_2()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
347.31 1443.06 1077.28\n\
354.71 1432.09 1085.80\n\
348.18 1460.71 1079.14\n\
329.93 1461.97 1096.57\n\
328.11 1445.99 1093.50\n\
342.47 1446.13 1080.47\n\
352.00 1466.29 1083.28\n\
340.00 1467.40 1096.57\n\
");


    addInputObservations(data, function);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "guess axis");
    function->setScalarInputParams(scalarInputParams);


    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-298
void FunctionTest::testBestFitCylinder2_trafo_guess_axis_1()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
-3283.64 -79.92 194.91\n\
-3271.56 -84.98 203.64\n\
-3292.58 -64.64 196.51\n\
-3308.79 -73.41 213.80\n\
-3301.54 -87.81 210.96\n\
-3289.41 -79.96 198.02\n\
-3292.49 -57.84 200.59\n\
-3303.32 -63.35 213.79\n\
");


    addInputObservations(data, function);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "guess axis");
    function->setScalarInputParams(scalarInputParams);


    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-298
void FunctionTest::testBestFitCylinder2_guess_axis()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
-59.57 -7.72 17.57\n\
-59.44 -18.23 5.86\n\
-58.84 10.00 16.30\n\
-33.61 11.09 15.63\n\
-34.43 -5.00 18.49\n\
-53.82 -4.73 18.56\n\
-58.87 15.80 10.89\n\
-41.04 16.85 9.11\n\
");


    addInputObservations(data, function);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "guess axis");
    function->setScalarInputParams(scalarInputParams);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-298 direction
void FunctionTest::testBestFitCylinder2_1st_2_pts()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
-59.57 -7.72 17.57\n\
-34.43 -5.00 18.49\n\
-59.44 -18.23 5.86\n\
-58.84 10.00 16.30\n\
-33.61 11.09 15.63\n\
-53.82 -4.73 18.56\n\
-58.87 15.80 10.89\n\
-41.04 16.85 9.11\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-627
void FunctionTest::testBestFitCylinderAproximationDirection1()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
-59.57 -7.72 17.57\n\
-34.43 -5.00 18.49\n\
-59.44 -18.23 5.86\n\
-58.84 10.00 16.30\n\
-33.61 11.09 15.63\n\
-53.82 -4.73 18.56\n\
-58.87 15.80 10.89\n\
-41.04 16.85 9.11\n\
");


    addInputObservations(data, function);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "direction");
    function->setScalarInputParams(scalarInputParams);

    // QDEBUG : FunctionTest::testBestFitCylinder10() position= -49.95613858 , 0.002538946657 , 0.003555186998 , direction= 0.9999999383 , 0.00019742765 , -0.0002907078214 , radius= 19.15680458 , stdev= 0.03371648532

    addInputLine(0,0,0, -0.9, 0, 0, function, 2000, 1);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    // QDEBUG : FunctionTest::testBestFitCylinderAproximationDirection1() position= -49.95613858 , 0.002538946657 , 0.003555186993 , direction= 0.9999999383 , 0.0001974276497 , -0.0002907078213 , radius= 19.15680458 , stdev= 0.03371845401
    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(0), (-0.9999999383), 0.000001);
    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(1), (-0.0001974276497), 0.000001);
    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(2), (0.0002907078213), 0.000001);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-573
void FunctionTest::testDisableAllObservationsButLastOne_no()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("disable all observations but last one", "no");
    function->setScalarInputParams(scalarInputParams);

    QPointer<Point> point = new Point(false);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
1.0 1.1 1.2 FS\n\
1.1 1.2 1.3 FS\n\
1.2 1.3 1.4 FS\n\
1.3 1.4 1.5 FS\n\
");

    addInputObservations(data, function);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    DEBUG_POINT(point);

    COMPARE_DOUBLE(point->getPosition().getVector().getAt(0), 1.15, 0.01);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(1), 1.25, 0.01);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(2), 1.35, 0.01);

    delete function.data();
}

// OI-573
void FunctionTest::testDisableAllObservationsButLastOne_yes()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("disable all observations but last one", "yes");
    function->setScalarInputParams(scalarInputParams);

    QPointer<Point> point = new Point(false);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
1.0 1.1 1.2 FS\n\
1.1 1.2 1.3 FS\n\
1.2 1.3 1.4 FS\n\
1.3 1.4 1.5 FS\n\
");

    addInputObservations(data, function);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    DEBUG_POINT(point);

    COMPARE_DOUBLE(point->getPosition().getVector().getAt(0), 1.3, 0.01);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(1), 1.4, 0.01);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(2), 1.5, 0.01);

    delete function.data();
}

// OI-573
void FunctionTest::testDisableAllObservationsButLastOne_2side_yes()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("disable all observations but last one", "yes");
    function->setScalarInputParams(scalarInputParams);

    QPointer<Point> point = new Point(false);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
1.0 1.1 1.2 FS\n\
1.02 1.1 1.2 BS\n\
1.1 1.2 1.3 FS\n\
1.12 1.2 1.3 BS\n\
1.2 1.3 1.4 FS\n\
1.22 1.32 1.42 BS\n\
");

    addInputObservations(data, function);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    DEBUG_POINT(point);

    COMPARE_DOUBLE(point->getPosition().getVector().getAt(0), 1.21, 0.01);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(1), 1.31, 0.01);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(2), 1.41, 0.01);

    delete function.data();
}

// OI-577: old behavior
void FunctionTest::testBestFitPlane()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitPlane();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> plane = new Plane(false);
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
0.0 0.0 0.001\n\
1.0 0.0 0.002\n\
1.0 1.0 0.004\n\
0.0 1.0 0.003\n\
");

    addInputObservations(data, function);

    bool res = function->exec(planeFeature);
    QVERIFY2(res, "exec");

    // QDEBUG : FunctionTest::testBestFitPlane() position= 0.5 , 0.5 , 0.0025 , direction= -0.0009999975 , -0.001999995 , 0.9999975 , stdev= 3.654760904e-10
    DEBUG_PLANE(plane);

    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(0), (-0.0009999975), 0.000001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(1), (-0.001999995), 0.000001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(2), (0.9999975), 0.000001);


    delete function.data();
}

// OI-557
void FunctionTest::testBestFitPlane_DummyPoint1() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitPlane();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> plane = new Plane(false);
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
0.0 0.0 0.001\n\
1.0 0.0 0.002\n\
1.0 1.0 0.004\n\
0.0 1.0 0.003\n\
");

    addInputObservations(data, function);
    addInputObservations("0.0 0.0 -10\n", function, 3000, InputElementKey::eDummyPoint);

    bool res = function->exec(planeFeature);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(plane);

    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(0), (0.0009999975), 0.000001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(1), (0.001999995), 0.000001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(2), (-0.9999975), 0.000001);


    delete function.data();
}

// OI-557
void FunctionTest::testBestFitPlane_DummyPoint2() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitPlane();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> plane = new Plane(false);
    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
0.0 0.0 0.001\n\
1.0 0.0 0.002\n\
1.0 1.0 0.004\n\
0.0 1.0 0.003\n\
");

    addInputObservations(data, function);
    addInputObservations("0.0 0.0 10\n", function, 3000, InputElementKey::eDummyPoint);

    bool res = function->exec(planeFeature);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(plane);

    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(0), (-0.0009999975), 0.000001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(1), (-0.001999995), 0.000001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(2), (0.9999975), 0.000001);


    delete function.data();
}


// OI-577: old behavior
void FunctionTest::testBestFitCircleInPlane()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCircleInPlane();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Circle> circle = new Circle(false);
    QPointer<FeatureWrapper> circleFeature = new FeatureWrapper();
    circleFeature->setCircle(circle);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
0.0 0.0 0.001\n\
1.0 0.0 0.002\n\
1.0 1.0 0.004\n\
0.0 1.0 0.003\n\
");

    addInputObservations(data, function);

    bool res = function->exec(circleFeature);
    QVERIFY2(res, "exec");

    // QDEBUG : FunctionTest::testBestFitCircleInPlane() position= 0.5 , 0.5 , 0.0025 , direction= -0.0009999975 , -0.001999995 , 0.9999975 , stdev= 1.414211795e-06
    DEBUG_PLANE(circle);

    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(0), (-0.0009999975), 0.000001);
    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(1), (-0.001999995), 0.000001);
    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(2), (0.9999975), 0.000001);


    delete function.data();
}

void FunctionTest::testBestFitCircleInPlane_DummyPoint1()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCircleInPlane();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Circle> circle = new Circle(false);
    QPointer<FeatureWrapper> circleFeature = new FeatureWrapper();
    circleFeature->setCircle(circle);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
0.0 0.0 0.001\n\
1.0 0.0 0.002\n\
1.0 1.0 0.004\n\
0.0 1.0 0.003\n\
");

    addInputObservations(data, function);
    addInputObservations("0.0 0.0 10\n", function, 3000, InputElementKey::eDummyPoint);

    bool res = function->exec(circleFeature);
    QVERIFY2(res, "exec");

    // QDEBUG : FunctionTest::testBestFitCircleInPlane() position= 0.5 , 0.5 , 0.0025 , direction= -0.0009999975 , -0.001999995 , 0.9999975 , stdev= 1.414211795e-06
    DEBUG_PLANE(circle);

    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(0), (-0.0009999975), 0.000001);
    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(1), (-0.001999995), 0.000001);
    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(2), (0.9999975), 0.000001);


    delete function.data();
}

void FunctionTest::testBestFitCircleInPlane_DummyPoint2()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCircleInPlane();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Circle> circle = new Circle(false);
    QPointer<FeatureWrapper> circleFeature = new FeatureWrapper();
    circleFeature->setCircle(circle);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
0.0 0.0 0.001\n\
1.0 0.0 0.002\n\
1.0 1.0 0.004\n\
0.0 1.0 0.003\n\
");

    addInputObservations(data, function);
    addInputObservations("0.0 0.0 -10\n", function, 3000, InputElementKey::eDummyPoint);

    bool res = function->exec(circleFeature);
    QVERIFY2(res, "exec");

    // QDEBUG : FunctionTest::testBestFitCircleInPlane() position= 0.5 , 0.5 , 0.0025 , direction= -0.0009999975 , -0.001999995 , 0.9999975 , stdev= 1.414211795e-06
    DEBUG_PLANE(circle);

    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(0), (0.0009999975), 0.000001);
    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(1), (0.001999995), 0.000001);
    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(2), (-0.9999975), 0.000001);


    delete function.data();
}

void FunctionTest::testBestFitCylinder1__DummyPoint1()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);


    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);


    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
-59.57 -7.72 17.57\n\
-34.43 -5.00 18.49\n\
-59.44 -18.23 5.86\n\
-58.84 10.00 16.30\n\
-33.61 11.09 15.63\n\
-53.82 -4.73 18.56\n\
-58.87 15.80 10.89\n\
-41.04 16.85 9.11\n\
");


    addInputObservations(data, function);
    addInputObservations("0. 0. 0.\n2000. 10. 10.\n", function, 2000, InputElementKey::eDummyPoint);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "first two dummy points");
    function->setScalarInputParams(scalarInputParams);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    //  position= -49.95613858 , 0.002538946657 , 0.003555186998 , direction= 0.9999999383 , 0.00019742765 , -0.0002907078214 , radius= 19.15680458 , stdev= 0.03371648532
    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(0), (0.9999999383), 0.00001);
    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(1), (0.00019742765), 0.00001);
    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(2),  (-0.0002907078214), 0.00001);
    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.156, 0.01);

    delete function.data();
}
void FunctionTest::testBestFitCylinder1__DummyPoint2()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);


    QPointer<Function> function = new BestFitCylinder();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> cylinder = new Cylinder(false);
    QPointer<FeatureWrapper> cylinderFeature = new FeatureWrapper();
    cylinderFeature->setCylinder(cylinder);


    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
-59.57 -7.72 17.57\n\
-34.43 -5.00 18.49\n\
-59.44 -18.23 5.86\n\
-58.84 10.00 16.30\n\
-33.61 11.09 15.63\n\
-53.82 -4.73 18.56\n\
-58.87 15.80 10.89\n\
-41.04 16.85 9.11\n\
");


    addInputObservations(data, function);
    addInputObservations("2000. 0. 0.\n0. 10. 10.\n", function, 2000, InputElementKey::eDummyPoint);

    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("approximation", "first two dummy points");
    function->setScalarInputParams(scalarInputParams);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    //  position= -49.95613858 , 0.002538946657 , 0.003555186998 , direction= 0.9999999383 , 0.00019742765 , -0.0002907078214 , radius= 19.15680458 , stdev= 0.03371648532
    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(0), (-0.9999999383), 0.00001);
    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(1), (-0.00019742765), 0.00001);
    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(2),  (0.0002907078214), 0.00001);
    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.156, 0.01);

    delete function.data();
}

void FunctionTest::testIntersectLineLine_parallel()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new IntersectLineLine();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> point = new Point(false);
    OiVec posv;
    Position pos(posv);
    point->setPoint(pos);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);


    //ScalarInputParams scalarInputParams;
    //scalarInputParams.stringParameter.insert("TODO", "TODO");
    //function->setScalarInputParams(scalarInputParams);

    addInputLine(1., 1., 1., 0.5772, 0.5772, 0.5772, function, 2000, 0);
    addInputLine(2., 2., 2., 0.5772, 0.5772, 0.5772, function, 2001, 1);

    bool res = function->exec(pointFeature);
    QVERIFY2(!res, "exec");

    delete function.data();
}
void FunctionTest::testIntersectLineLine_intersect1_atfirstline()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new IntersectLineLine();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> point = new Point(false);
    OiVec posv;
    Position pos(posv);
    point->setPoint(pos);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);


    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("intersection", "at first line");
    function->setScalarInputParams(scalarInputParams);

    addInputLine(1., 1., 1., 0.5772, 0.5772, 0.5772, function, 2000, 0);
    addInputLine(2., 2., 2., 0.0, 1., 0.0, function, 2001, 1);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    DEBUG_POINT(point);

    COMPARE_DOUBLE(point->getPosition().getVector().getAt(0), 2, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(1), 2, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(2), 2, 0.0001);

    delete function.data();

}

void FunctionTest::testIntersectLineLine_intersect1_atsecondline()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new IntersectLineLine();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> point = new Point(false);
    OiVec posv;
    Position pos(posv);
    point->setPoint(pos);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);


    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("intersection", "at second line");
    function->setScalarInputParams(scalarInputParams);

    addInputLine(1., 1., 1., 0.5772, 0.5772, 0.5772, function, 2000, 0);
    addInputLine(2., 2., 2., 0.0, 1., 0.0, function, 2001, 1);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    DEBUG_POINT(point);

    COMPARE_DOUBLE(point->getPosition().getVector().getAt(0), 2, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(1), 2, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(2), 2, 0.0001);

    delete function.data();

}

void FunctionTest::testIntersectLineLine_intersect1_midpoint()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new IntersectLineLine();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> point = new Point(false);
    OiVec posv;
    Position pos(posv);
    point->setPoint(pos);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);


    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("intersection", "midpoint");
    function->setScalarInputParams(scalarInputParams);

    addInputLine(1., 1., 1., 0.5772, 0.5772, 0.5772, function, 2000, 0);
    addInputLine(2., 2., 2., 0.0, 1., 0.0, function, 2001, 1);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    DEBUG_POINT(point);

    COMPARE_DOUBLE(point->getPosition().getVector().getAt(0), 2, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(1), 2, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(2), 2, 0.0001);

    delete function.data();

}

void FunctionTest::testIntersectLineLine_intersect2_atfirstline()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new IntersectLineLine();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> point = new Point(false);
    OiVec posv;
    Position pos(posv);
    point->setPoint(pos);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);


    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("intersection", "at first line");
    function->setScalarInputParams(scalarInputParams);

    addInputLine(1., 1., 1., 0.5773, 0.5773, 0.5773, function, 2000, 0);
    addInputLine(2., 2., 1., 0.0, 1., 0.0, function, 2001, 1);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    DEBUG_POINT(point);

    COMPARE_DOUBLE(point->getPosition().getVector().getAt(0), 1.5, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(1), 1.5, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(2), 1.5, 0.0001);

    delete function.data();

}

void FunctionTest::testIntersectLineLine_intersect2_atsecondline()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new IntersectLineLine();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> point = new Point(false);
    OiVec posv;
    Position pos(posv);
    point->setPoint(pos);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);


    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("intersection", "at second line");
    function->setScalarInputParams(scalarInputParams);

    addInputLine(1., 1., 1., 0.5773, 0.5773, 0.5773, function, 2000, 0);
    addInputLine(2., 2., 1., 0.0, 1., 0.0, function, 2001, 1);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    DEBUG_POINT(point);
    COMPARE_DOUBLE(point->getDistance(), 0.7071, 0.0001);

    COMPARE_DOUBLE(point->getPosition().getVector().getAt(0), 2., 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(1), 1.5, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(2), 1., 0.0001);

    delete function.data();

}

void FunctionTest::testIntersectLineLine_intersect2_midpoint()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new IntersectLineLine();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> point = new Point(false);
    OiVec posv;
    Position pos(posv);
    point->setPoint(pos);
    QPointer<FeatureWrapper> pointFeature = new FeatureWrapper();
    pointFeature->setPoint(point);


    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("intersection", "midpoint");
    function->setScalarInputParams(scalarInputParams);

    addInputLine(1., 1., 1., 0.5773, 0.5773, 0.5773, function, 2000, 0);
    addInputLine(2., 2., 1., 0.0, 1., 0.0, function, 2001, 1);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    DEBUG_POINT(point);

    COMPARE_DOUBLE(point->getPosition().getVector().getAt(0), 1.75, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(1), 1.5, 0.0001);
    COMPARE_DOUBLE(point->getPosition().getVector().getAt(2), 1.25, 0.0001);

    delete function.data();

}

// OI-42
void FunctionTest::testRectifyToPoint_Plane_negative() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> plane = createPlane(0, 0, 0, 0, 0, 1.);

    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputPoint(1., 1., -5., function);

    bool res = function->exec(planeFeature);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(plane);

    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(2), (1.0), 0.0001);

    delete function.data();
}

void FunctionTest::testRectifyToPoint_Plane_positive() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> plane = createPlane(0, 0, 0, 0, 0, 1.);

    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    const bool sense = true;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputPoint(1., 1., -5., function);

    bool res = function->exec(planeFeature);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(plane);

    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(2), (-1.0), 0.0001);

    delete function.data();
}

void FunctionTest::testRectifyToPoint_Plane_Station() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> plane = createPlane(0, 0, 0, 0, 0, 1.);

    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    const bool sense = true;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputPoint(1., 1., -5., function);

    bool res = function->exec(planeFeature);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(plane);

    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(2), (-1.0), 0.0001);

    delete function.data();
}

// OI-42
void FunctionTest::testRectifyToPoint_Circle_negative() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Circle> geometry = createCircle(0, 0, 0, 0, 0, 1., 10.);

    QPointer<FeatureWrapper> feature = new FeatureWrapper();
    feature->setCircle(geometry);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputPoint(1., 1., -5., function);

    bool res = function->exec(feature);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(geometry);

    COMPARE_DOUBLE(geometry->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(geometry->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(geometry->getDirection().getVector().getAt(2), (1.0), 0.0001);

    delete function.data();
}

// OI-42
void FunctionTest::testRectifyToPoint_Line_negative() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Line> geometry = createLine(0, 0, 0, 0, 0, 1.);

    QPointer<FeatureWrapper> feature = new FeatureWrapper();
    feature->setLine(geometry);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputPoint(1., 1., -5., function);

    bool res = function->exec(feature);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(geometry);

    COMPARE_DOUBLE(geometry->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(geometry->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(geometry->getDirection().getVector().getAt(2), (1.0), 0.0001);

    delete function.data();
}

// OI-42
void FunctionTest::testRectifyToPoint_Cylinder_negative() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> geometry = createCylinder(0, 0, 0, 0, 0, 1., 10.);

    QPointer<FeatureWrapper> feature = new FeatureWrapper();
    feature->setCylinder(geometry);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputPoint(1., 1., -5., function);

    bool res = function->exec(feature);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(geometry);

    COMPARE_DOUBLE(geometry->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(geometry->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(geometry->getDirection().getVector().getAt(2), (1.0), 0.0001);

    delete function.data();
}

// OI-42
void FunctionTest::testRectifyToVector_PlaneToPlane_negative() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToVector();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> plane = createPlane(0, 0, 0, 0, 0, 1.);

    QPointer<FeatureWrapper> planeFeature = new FeatureWrapper();
    planeFeature->setPlane(plane);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputStation(0, 0, 0, -0.001042, 0.000211, 0.999999, function, 3000, 1);

    bool res = function->exec(planeFeature);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(plane);

    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(plane->getDirection().getVector().getAt(2), (-1.0), 0.0001);

    delete function.data();
}
void FunctionTest::testRectifyToVector_PlaneToStation() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToVector();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> feature = createPlane(0, 0, 0, 0, 0, -1.);

    QPointer<FeatureWrapper> featureWrapper = new FeatureWrapper();
    featureWrapper->setPlane(feature);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputStation(0, 0, 0, -0.001042, 0.000211, 0.999999, function, 3000, 1);

    bool res = function->exec(featureWrapper);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (-1.0), 0.0001);

    delete function.data();
}


void FunctionTest::testRectifyToVector_CircleToStation() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToVector();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Circle> feature = createCircle(0, 0, 0, 0, 0, -1., 10.);

    QPointer<FeatureWrapper> featureWrapper = new FeatureWrapper();
    featureWrapper->setCircle(feature);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputStation(0, 0, 0, -0.001042, 0.000211, 0.999999, function, 3000, 1);

    bool res = function->exec(featureWrapper);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (-1.0), 0.0001);

    delete function.data();
}

void FunctionTest::testRectifyToVector_LineToStation() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToVector();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Line> feature = createLine(0, 0, 0, 0, 0, -1.);

    QPointer<FeatureWrapper> featureWrapper = new FeatureWrapper();
    featureWrapper->setLine(feature);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputStation(0, 0, 0, -0.001042, 0.000211, 0.999999, function, 3000, 1);

    bool res = function->exec(featureWrapper);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (-1.0), 0.0001);

    delete function.data();
}

void FunctionTest::testRectifyToVector_CylinderToStation() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToVector();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Cylinder> feature = createCylinder(0, 0, 0, 0, 0, -1., 10.);

    QPointer<FeatureWrapper> featureWrapper = new FeatureWrapper();
    featureWrapper->setCylinder(feature);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputStation(0, 0, 0, -0.001042, 0.000211, 0.999999, function, 3000, 1);

    bool res = function->exec(featureWrapper);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (-1.0), 0.0001);

    delete function.data();
}

// OI-527
void FunctionTest::testRectifyToVector_PlaneToCoodinateSystem_zAxis() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToVector();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> feature = createPlane(0, 0, 0, 0, 0, 1.);

    QPointer<FeatureWrapper> featureWrapper = new FeatureWrapper();
    featureWrapper->setPlane(feature);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    function->setScalarInputParams(scalarInputParams);

    addInputCoordinateSystem(0, 0, 0, -0.001042, 0.000211, 0.999999, function, 2000, 2);

    bool res = function->exec(featureWrapper);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), 0.0, 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (-1.0), 0.0001);

    delete function.data();
}

// OI-527
void FunctionTest::testRectifyToVector_PlaneToCoodinateSystem_xAxis() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToVector();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> feature = createPlane(0, 0, 0, 1, 0, 0);

    QPointer<FeatureWrapper> featureWrapper = new FeatureWrapper();
    featureWrapper->setPlane(feature);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    scalarInputParams.stringParameter.insert("rectify to station / coordinate system", "xAxis");
    function->setScalarInputParams(scalarInputParams);

    OiMat trafo(4, 4);
    trafo.setAt(0, 0, 0.9998379552);
    trafo.setAt(0, 1, -0.0101018638);
    trafo.setAt(0, 2, -0.0010422656);
    trafo.setAt(0, 3, 18.4149563832);
    trafo.setAt(1, 0, 0.0101020893);
    trafo.setAt(1, 1, 0.9998384738);
    trafo.setAt(1, 2, 0.0002113152);
    trafo.setAt(1, 3, 0.0007839294);
    trafo.setAt(2, 0, 0.0010400775);
    trafo.setAt(2, 1, -0.0002218345);
    trafo.setAt(2, 2, 0.9998889637);
    trafo.setAt(2, 3, -2.1009030443);
    trafo.setAt(3, 0, 0.);
    trafo.setAt(3, 1, 0.);
    trafo.setAt(3, 2, 0.);
    trafo.setAt(3, 3, 1.);

    addInputCoordinateSystem(trafo, function, 2000, 2);

    bool res = function->exec(featureWrapper);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), (-1.0), 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), (0.0), 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (0.0), 0.0001);

    delete function.data();
}

// OI-527
void FunctionTest::testRectifyToVector_PlaneToCoodinateSystem_yAxis() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new RectifyToVector();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> feature = createPlane(0, 0, 0, 0, 1, 0);

    QPointer<FeatureWrapper> featureWrapper = new FeatureWrapper();
    featureWrapper->setPlane(feature);

    const bool sense = false;
    ScalarInputParams scalarInputParams;
    scalarInputParams.stringParameter.insert("sense", sense ? "positive" : "negative");
    scalarInputParams.stringParameter.insert("rectify to station / coordinate system", "yAxis");
    function->setScalarInputParams(scalarInputParams);

    OiMat trafo(4, 4);
    trafo.setAt(0, 0, 0.9998379552);
    trafo.setAt(0, 1, -0.0101018638);
    trafo.setAt(0, 2, -0.0010422656);
    trafo.setAt(0, 3, 18.4149563832);
    trafo.setAt(1, 0, 0.0101020893);
    trafo.setAt(1, 1, 0.9998384738);
    trafo.setAt(1, 2, 0.0002113152);
    trafo.setAt(1, 3, 0.0007839294);
    trafo.setAt(2, 0, 0.0010400775);
    trafo.setAt(2, 1, -0.0002218345);
    trafo.setAt(2, 2, 0.9998889637);
    trafo.setAt(2, 3, -2.1009030443);
    trafo.setAt(3, 0, 0.);
    trafo.setAt(3, 1, 0.);
    trafo.setAt(3, 2, 0.);
    trafo.setAt(3, 3, 1.);

    addInputCoordinateSystem(trafo, function, 2000, 2);

    bool res = function->exec(featureWrapper);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), (0.0), 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), (-1.0), 0.0001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (0.0), 0.0001);

    delete function.data();
}

// OI-636
void FunctionTest::testCircleInPlaneFromPoints()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCircleInPlaneFromPoints();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Circle> circle = new Circle(false);
    QPointer<FeatureWrapper> circleFeature = new FeatureWrapper();
    circleFeature->setCircle(circle);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
0.0 0.0 0.001\n\
1.0 0.0 0.002\n\
1.0 1.0 0.004\n\
0.0 1.0 0.003\n\
");

    addInputPoints(data, function);

    bool res = function->exec(circleFeature);
    QVERIFY2(res, "exec");

    // QDEBUG : FunctionTest::testBestFitCircleInPlane() position= 0.5 , 0.5 , 0.0025 , direction= -0.0009999975 , -0.001999995 , 0.9999975 , stdev= 1.414211795e-06
    DEBUG_PLANE(circle);

    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(0), (-0.0009999975), 0.000001);
    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(1), (-0.001999995), 0.000001);
    COMPARE_DOUBLE(circle->getDirection().getVector().getAt(2), (0.9999975), 0.000001);


    delete function.data();
}

// OI-508
void FunctionTest::testBestFitPoint_residuals()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);


    QPointer<Function> function = new BestFitPoint();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> feature = new Point(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setPoint(feature);


    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
999.9912 1999.9865 2999.9741\n\
999.9942 1999.9931 2999.9893\n\
1001.9972 2000.9946 3000.9907\n\
");


    addInputObservations(data, function);

    bool res = function->exec(wrapper);
    QVERIFY2(res, "exec");

    DEBUG_POINT(feature);

    COMPARE_DOUBLE(feature->getStatistic().getStdev(), 1.42, 0.01);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1000).corrections.value("v", -1), ( 0.825), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("v", -1), ( 0.814), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("v", -1), ( 1.639), 0.001);

    delete function.data();
}

void FunctionTest::testBestFitPlane_residuals()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitPlane();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Plane> feature = new Plane(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setPlane(feature);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
1000.0014 1000.0030 1001.0031\n\
1999.9994 2000.0043 998.0100\n\
999.9965 1999.9965 1099.9985\n\
1499.9884 1499.9889 999.9898\n\
");

    addInputObservations(data, function);
    addInputObservations("2000.0007 999.9968 1002.0012", function, 3000, InputElementKey::eDefault, false);

    bool res = function->exec(wrapper);
    QVERIFY2(res, "exec");

    DEBUG_PLANE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), (0.100818), 0.000001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), (-0.097854), 0.000001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (0.990081), 0.000001);

    COMPARE_DOUBLE(feature->getStatistic().getStdev(), 0.39, 0.01);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1000).corrections.value("v", -1), ( -  0.1595), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("v", -1), ( -  0.1595), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("v", -1), (    0.0000), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1003).corrections.value("v", -1), (    0.3191), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3000).corrections.value("v", -1), (  101.6470), 0.001); // shouldInUse == false

    delete function.data();
}

void FunctionTest::testBestFitLine_residuals()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitLine();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Line> feature = new Line(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setLine(feature);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
999.9916 999.9958 999.9917\n\
2000.0015 2000.0008 999.9970\n\
3000.0270 3000.0247 1000.0083\n\
4999.9732 4999.9797 1000.0041\n\
");

    addInputObservations(data, function);
    addInputObservations("3899.9642 4099.9690 999.9721", function, 3000, InputElementKey::eDefault, false);

    bool res = function->exec(wrapper);
    QVERIFY2(res, "exec");

    DEBUG_LINE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), (0.707106), 0.000001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), (0.707107), 0.000001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (0.000002), 0.000001);

    COMPARE_DOUBLE(feature->getStatistic().getStdev(), 0.0069, 0.001);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1000).corrections.value("v", -1), (    0.0040), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("v", -1), (    0.0017), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("v", -1), (    0.0079), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1003).corrections.value("v", -1), (    0.0039), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3000).corrections.value("v", -1), (  141.4227), 0.001); // shouldInUse == false

    delete function.data();
}

void FunctionTest::testBestFitCircleInPlane_residuals()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCircleInPlane();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Circle> feature = new Circle(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setCircle(feature);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
999.9962 999.9970 999.9958\n\
-0.0001 999.9971 999.9975\n\
-0.0002 0.0017 999.9884\n\
-0.0002 999.9924 999.9938\n\
");

    addInputObservations(data, function);
    addInputObservations("1100.0093 1100.0075 1000.0071", function, 3000, InputElementKey::eDefault, false);
    addInputObservations("400. 400. 1000.1", function, 3001, InputElementKey::eDefault, false);
    addInputObservations("900. 900. 1000.1", function, 3002, InputElementKey::eDefault, false);

    bool res = function->exec(wrapper);
    QVERIFY2(res, "exec");

    // position= 499.99915 , 499.9982 , 999.9921 , direction= -1.499801648e-07 , -7.25006333e-06 , 1 , stdev= 0.002300006535
    DEBUG_CIRCLE(feature);
    // 499.9991	499.9982	999.9921	0.0023	4/5	FastPoint	BestFitCircleInPlane		-0.000000	-0.000007	1.000000	707.1038

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), (0.0), 0.000001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), (-0.000007), 0.000001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (1.), 0.000001);

    COMPARE_DOUBLE(feature->getStatistic().getStdev(), 0.0023, 0.001);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1000).corrections.value("vr", -1), (    0.0000), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("vr", -1), (    0.0025), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("vr", -1), (    0.0000), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1003).corrections.value("vr", -1), (   -0.0025), 0.001);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3000).corrections.value("vr", -1), (  141.4381), 0.001); // shouldInUse == false

    // - sign
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3001).corrections.value("vr", -1), (  -565.684), 0.001);  // shouldInUse == false
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3002).corrections.value("vr", -1), (  -141.4171), 0.001); // shouldInUse == false

    delete function.data();
}

void FunctionTest::testBestFitCircleInPlane_residuals2()
{
    QSKIP("");
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitCircleInPlane();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Circle> feature = new Circle(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setCircle(feature);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
0.100568 1.002327 0.9999\n\
0.669641 0.750807 0.9998\n\
0.980149 0.213177 0.9997\n\
0.922815 -0.405656 0.9996\n\
0.505371 -0.865865 1.0002\n\
-0.100048 -0.997141 1.0003\n\
-0.671723 -0.753143 1.0004\n\
-0.985862 -0.214420 1.0001\n\
-0.919378 0.404145 1.0002\n\
-0.508547 0.871306 1.0003\n\
");

    addInputObservations(data, function);
//    addInputObservations("1100.0093 1100.0075 1000.0071", function, 3000, InputElementKey::eDefault, false);
//    addInputObservations("400. 400. 1000.1", function, 3001, InputElementKey::eDefault, false);
//    addInputObservations("900. 900. 1000.1", function, 3002, InputElementKey::eDefault, false);

    bool res = function->exec(wrapper);
    QVERIFY2(res, "exec");

    // position= -0.001404037774 , 0.001101723963 , 1.000050134 , direction= -0.0002842264613 , -0.0001194733372 , -0.9999999525 , radius= 1.006048006 , stdev= 0.002803002496
    DEBUG_CIRCLE(feature);

    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), (-0.0002842264613), 0.000001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), (-0.0001194733372), 0.000001);
    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (-0.9999999525), 0.000001);

    COMPARE_DOUBLE(feature->getStatistic().getStdev(), 0.0028, 0.001);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1000).corrections.value("vr", -1), (    0.000036), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("vr", -1), (    0.000012), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("vr", -1), (   -0.000018), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1003).corrections.value("vr", -1), (    0.003728), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1004).corrections.value("vr", -1), (   -0.001841), 0.00001);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1005).corrections.value("vr", -1), (   -0.002945), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1006).corrections.value("vr", -1), (    0.003018), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1007).corrections.value("vr", -1), (   -0.001744), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1008).corrections.value("vr", -1), (   -0.003492), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1009).corrections.value("vr", -1), (   -0.001170), 0.00001);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3000).corrections.value("vr", -1), (  141.4381), 0.001); // shouldInUse == false

    // - sign
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3001).corrections.value("vr", -1), (  -565.684), 0.001);  // shouldInUse == false
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3002).corrections.value("vr", -1), (  -141.4171), 0.001); // shouldInUse == false

    delete function.data();
}


void FunctionTest::testBestFitSphere_residuals()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new BestFitSphere();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Sphere> feature = new Sphere(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setSphere(feature);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    QString data("\
0.010757 0.107215 1.004511\n\
0.084127 0.068784 1.000029\n\
0.102412 -0.010275 1.003127\n\
0.064782 -0.079232 0.996630\n\
-0.010505 -0.104704 0.995745\n\
-0.077842 -0.063646 0.997522\n\
-0.102593 0.010294 0.995853\n\
-0.068755 0.084091 1.001206\n\
0.078093 0.778322 0.636901\n\
0.605526 0.495096 0.635753\n\
0.770792 -0.077337 0.638643\n\
0.494094 -0.604301 0.638498\n\
-0.078274 -0.780132 0.638571\n\
-0.600530 -0.491010 0.633805\n\
-0.777325 0.077993 0.638047\n\
-0.491122 0.600666 0.635220\n\
0.100016 0.996825 -0.100212\n\
0.774789 0.633490 -0.100801\n\
0.997101 -0.100044 -0.100286\n\
0.634610 -0.776159 -0.100562\n\
-0.100191 -0.998567 -0.100784\n\
-0.774254 -0.633052 -0.100123\n\
-0.993433 0.099676 -0.100003\n\
-0.636018 0.777882 -0.100647\n\
0.063588 0.633755 -0.779228\n\
0.490677 0.401192 -0.779711\n\
0.635817 -0.063794 -0.778091\n\
0.403917 -0.494010 -0.778308\n\
-0.063645 -0.634327 -0.775701\n\
-0.490056 -0.400684 -0.774851\n\
-0.633104 0.063522 -0.781068\n\
-0.402384 0.492135 -0.781833\n\
");

    addInputObservations(data, function);
    addInputObservations("2. 2. 2.", function, 3000, InputElementKey::eDefault, false);
    addInputObservations(".5 .5 .5", function, 3001, InputElementKey::eDefault, false);

    bool res = function->exec(wrapper);
    QVERIFY2(res, "exec");

    //  position= 0.0009435528999 , 0.0004326516194 , -0.0002321768279 , direction= 0 , 0 , 0 , radius= 1.006002313 , stdev= 0.001720371939
    DEBUG_SPHERE(feature);

//    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(0), (-0.0002842264613), 0.000001);
//    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(1), (-0.0001194733372), 0.000001);
//    COMPARE_DOUBLE(feature->getDirection().getVector().getAt(2), (-0.9999999525), 0.000001);

    COMPARE_DOUBLE(feature->getStatistic().getStdev(), 0.0017, 0.001);


//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1000 :  QMap(("v", 0.004422)("vx", -4.32221e-05)("vy", -0.00046997)("vz", -0.00442179))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1001 :  QMap(("v", 3.62772e-05)("vx", -3.0068e-06)("vy", -2.47065e-06)("vz", -3.61524e-05))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1002 :  QMap(("v", 0.0025312)("vx", -0.0002547)("vy", 2.68612e-05)("vz", -0.00251835))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1003 :  QMap(("v", 0.00391404)("vx", 0.000250172)("vy", -0.000312133)("vz", 0.00390604))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1004 :  QMap(("v", 0.00440145)("vx", -5.05573e-05)("vy", -0.000464571)("vz", 0.00440115))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1005 :  QMap(("v", 0.00308691)("vx", -0.000242974)("vy", -0.000197619)("vz", 0.00307733))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1006 :  QMap(("v", 0.00450185)("vx", -0.000465401)("vy", 4.43563e-05)("vz", 0.00447773))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1007 :  QMap(("v", 0.00133364)("vx", 9.25858e-05)("vy", -0.000111149)("vz", -0.00133042))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1008 :  QMap(("v", 0.00156752)("vx", -0.000188453)("vy", -0.00189998)("vz", -0.00155615))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1009 :  QMap(("v", 0.00114584)("vx", -0.00078948)("vy", -0.000645944)("vz", -0.000830463))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1010 :  QMap(("v", 0.00256235)("vx", 0.00197183)("vy", -0.000199176)("vz", 0.00163632))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1011 :  QMap(("v", 0.00192102)("vx", -0.00117401)("vy", 0.00143961)("vz", -0.00152053))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1012 :  QMap(("v", 0.00365304)("vx", 0.000449531)("vy", 0.00442982)("vz", -0.00362527))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1013 :  QMap(("v", 0.00292964)("vx", -0.00201627)("vy", -0.00164743)("vz", 0.00212543))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1014 :  QMap(("v", 0.00350098)("vx", 0.00270703)("vy", -0.000269799)("vz", -0.0022201))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1015 :  QMap(("v", 0.00232597)("vx", -0.00142408)("vy", 0.00173718)("vz", 0.00183906))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1016 :  QMap(("v", 3.95106e-05)("vx", -2.78103e-05)("vy", -0.000279674)("vz", 2.80656e-05))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1017 :  QMap(("v", 0.000899147)("vx", 0.000891648)("vy", 0.000729427)("vz", -0.000115889))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1018 :  QMap(("v", 0.000195485)("vx", -0.000194506)("vy", 1.96174e-05)("vz", 1.95379e-05))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1019 :  QMap(("v", 0.000839322)("vx", -0.000828993)("vy", 0.00101596)("vz", 0.000131268))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1020 :  QMap(("v", 0.000441733)("vx", 0.000313226)("vy", 0.00309424)("vz", 0.000311475))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1021 :  QMap(("v", 6.62449e-05)("vx", 6.57015e-05)("vy", 5.36907e-05)("vz", 8.46712e-06))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1022 :  QMap(("v", 0.00170899)("vx", -0.00170045)("vy", 0.000169724)("vz", -0.000170633))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1023 :  QMap(("v", 0.0025934)("vx", 0.00256175)("vy", -0.00312683)("vz", 0.000403896))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1024 :  QMap(("v", 7.12998e-05)("vx", 5.71588e-06)("vy", 5.77798e-05)("vz", -7.10703e-05))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1025 :  QMap(("v", 0.0018276)("vx", 0.00097228)("vy", 0.000795636)("vz", -0.00154751))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1026 :  QMap(("v", 0.000105669)("vx", -6.68151e-05)("vy", 6.75863e-06)("vz", 8.18632e-05))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1027 :  QMap(("v", 9.6292e-05)("vx", -4.42841e-05)("vy", 5.43343e-05)("vz", 8.55048e-05))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1028 :  QMap(("v", 0.00138667)("vx", -0.000115082)("vy", -0.00113112)("vz", -0.00138189))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1029 :  QMap(("v", 0.00457928)("vx", -0.00245156)("vy", -0.00200277)("vz", -0.00386778))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1030 :  QMap(("v", 0.00181412)("vx", 0.00114354)("vy", -0.000113797)("vz", 0.00140831))
//QDEBUG : FunctionTest::testBestFitSphere_residuals() 1031 :  QMap(("v", 0.00143198)("vx", 0.000656647)("vy", -0.000800552)("vz", 0.00127254))

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1000).corrections.value("vr", -1), (0.00444691), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("vr", -1), (3.63613e-05), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("vr", -1), (0.00253134), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1003).corrections.value("vr", -1), (-0.00392647), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1004).corrections.value("vr", -1), (-0.00442589), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1005).corrections.value("vr", -1), (-0.00309323), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1006).corrections.value("vr", -1), (-0.00450207), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1007).corrections.value("vr", -1), (0.00133826), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1008).corrections.value("vr", -1), (0.00246314), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1009).corrections.value("vr", -1), (0.00131537), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1010).corrections.value("vr", -1), (-0.00257008), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1011).corrections.value("vr", -1), (0.00240058), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1012).corrections.value("vr", -1), (0.00574177), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1013).corrections.value("vr", -1), (-0.00336107), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1014).corrections.value("vr", -1), (0.00351136), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1015).corrections.value("vr", -1), (-0.00290309), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1016).corrections.value("vr", -1), (0.000282451), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1017).corrections.value("vr", -1), (-0.00115781), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1018).corrections.value("vr", -1), (0.000196467), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1019).corrections.value("vr", -1), (0.00131781), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1020).corrections.value("vr", -1), (0.00312561), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1021).corrections.value("vr", -1), (8.52707e-05), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1022).corrections.value("vr", -1), (-0.0017174), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1023).corrections.value("vr", -1), (0.00406236), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1024).corrections.value("vr", -1), (-9.17723e-05), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1025).corrections.value("vr", -1), (-0.00199328), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1026).corrections.value("vr", -1), (0.000105884), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1027).corrections.value("vr", -1), (0.000110564), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1028).corrections.value("vr", -1), (-0.00178949), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1029).corrections.value("vr", -1), (-0.00499809), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1030).corrections.value("vr", -1), (0.00181768), 0.00001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1031).corrections.value("vr", -1), (0.00164056), 0.00001);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3000).corrections.value("vr", -1), (2.45744), 0.00001);   // shouldInUse == false
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(3001).corrections.value("vr", -1), (-0.140637), 0.00001); // shouldInUse == false

    delete function.data();
}


void FunctionTest::testPointFromPoints_point()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new PointFromPoints();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> feature = new Point(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setPoint(feature);

    addInputPoint(1000.6609, 2000.3247, 3000.3180, function);

    bool res = function->exec(wrapper);
    QVERIFY2(res, "exec");

    // QDEBUG : FunctionTest::testBestFitCircleInPlane() position= 0.5 , 0.5 , 0.0025 , direction= -0.0009999975 , -0.001999995 , 0.9999975 , stdev= 1.414211795e-06
    DEBUG_POINT(feature);

    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(0), (1000.6609), 0.0001);
    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(1), (2000.3247), 0.0001);
    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(2), (3000.3180), 0.0001);

    delete function.data();
}

void FunctionTest::testPointFromPoints_circle()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new PointFromPoints();
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    QPointer<Point> feature = new Point(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setPoint(feature);

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    addInputCircle(1000.6609, 2000.3247, 3000.3180, 0.0, 0.0, 1.0, 1.0, function);

    bool res = function->exec(wrapper);
    QVERIFY2(res, "exec");

    // QDEBUG : FunctionTest::testBestFitCircleInPlane() position= 0.5 , 0.5 , 0.0025 , direction= -0.0009999975 , -0.001999995 , 0.9999975 , stdev= 1.414211795e-06
    DEBUG_POINT(feature);

    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(0), (1000.6609), 0.0001);
    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(1), (2000.3247), 0.0001);
    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(2), (3000.3180), 0.0001);

    delete function.data();
}

void FunctionTest::testPointFromPoints_Register() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Point> feature = new Point(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setPoint(feature);

    QPointer<Function> function1 = new PointFromPoints();
    function1->init();
    QObject::connect(function1.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);
    feature->addFunction(function1);

    QPointer<Function> function2 = new Register();
    function2->init();
    QObject::connect(function2.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);
    feature->addFunction(function2);

    QVERIFY2(feature->getDisplayFunctions().compare("PointFromPoints, Register")==0, "getDisplayFunctions");

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    addInputCircle(1000.6609, 2000.3247, 3000.3180, 0.0, 0.0, 1.0, 1.0, function1);

    addInputPlane(0., 0., 0., 0.10080018, -0.09785417,  0.99008277, function2);

    feature->recalc();
    QVERIFY2(feature->getIsSolved(), "recalc");

    // QDEBUG : FunctionTest::testFunctions() position= 710.7908481 , 2281.722941 , 153.1470537 , direction= 0 , 0 , 0 , stdev= nan
    DEBUG_POINT(feature);

    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(0), (  710.79084819), 0.0001);
    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(1), ( 2281.722941), 0.0001);
    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(2), (  153.1470537), 0.0001);

}

QPointer<Function> FunctionTest::createFunction(QString name) {
    OiTemplatePlugin plugin;

    QPointer<Function> function = plugin.createFunction(name);
    function->init();
    QObject::connect(function.data(), &Function::sendMessage, this, &FunctionTest::printMessage, Qt::AutoConnection);

    return function;
}


struct InputElementMapping {
    int functionIndex;
    int srcInputElementIndex;
    int dstsrcInputElementIndex;
};
struct ConfiguredFunctionConfig {
    QString name;
    QList<QString> functionNames;
    QMultiMap<int, InputElementMapping> inputElementsMapping;
    QList<NeededElement> neededElements;
    QList<FeatureTypes> applicableFor;

};

class ConfiguredFunction : public Function {
    friend class Feature;
    Q_OBJECT

public:
    ConfiguredFunction(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent = 0): Function(parent), functions(functions), config(config) {
    }

    void init() override {
        this->metaData.name = this->config.name;
        this->applicableFor = this->config.applicableFor;
    }

    bool exec(const QPointer<FeatureWrapper> &feature) {

        //execute all functions in the specified order
        for(int functionIndex = 0; functionIndex < this->functions.size(); functionIndex++){

            // break if the function pointer is not valid
            if(functions[functionIndex].isNull()){
                return false;
            }

            foreach(InputElementMapping mapping, this->config.inputElementsMapping.values(functionIndex)) {
                foreach(InputElement inputElement, this->inputElements.value(mapping.srcInputElementIndex)) {
                    functions[mapping.functionIndex]->addInputElement(inputElement, mapping.dstsrcInputElementIndex);
                }
            }

            // try to solve the current function
            if(!functions[functionIndex]->exec(feature)){
                return false;
            }
        }

        return true;
    }


private:
    ConfiguredFunctionConfig config;

    QList<QPointer<Function> > functions;
};

void FunctionTest::testPointFromPoints_Register2() {
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Point> feature = new Point(false);
    QPointer<FeatureWrapper> wrapper = new FeatureWrapper();
    wrapper->setPoint(feature);

    ConfiguredFunctionConfig config;
    config.name = "MyTestFunction";
    config.functionNames.append("PointFromPoints");
    config.functionNames.append("Register");
    config.applicableFor.append(ePointFeature);

    InputElementMapping m1;
    m1.functionIndex = 0;
    m1.srcInputElementIndex = 0;
    m1.dstsrcInputElementIndex = 0;
    config.inputElementsMapping.insert(m1.functionIndex, m1);

    InputElementMapping m2;
    m2.functionIndex = 1;
    m2.srcInputElementIndex = 1;
    m2.dstsrcInputElementIndex = 0;
    config.inputElementsMapping.insert(m2.functionIndex, m2);

    QList<QPointer<Function> > functions;
    foreach(QString name, config.functionNames) {
        QPointer<Function> f = createFunction(name);
        functions.append(f);
    }

    QVERIFY2(feature->getDisplayFunctions().compare("PointFromPoints, Register")==0, "getDisplayFunctions");

    // colum delim: " "
    // line ending: "\n"
    // unit:        [mm]
    addInputCircle(1000.6609, 2000.3247, 3000.3180, 0.0, 0.0, 1.0, 1.0, function1);

    addInputPlane(0., 0., 0., 0.10080018, -0.09785417,  0.99008277, function2);

    feature->recalc();
    QVERIFY2(feature->getIsSolved(), "recalc");

    // QDEBUG : FunctionTest::testFunctions() position= 710.7908481 , 2281.722941 , 153.1470537 , direction= 0 , 0 , 0 , stdev= nan
    DEBUG_POINT(feature);

    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(0), (  710.79084819), 0.0001);
    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(1), ( 2281.722941), 0.0001);
    COMPARE_DOUBLE(feature->getPosition().getVector().getAt(2), (  153.1470537), 0.0001);

}
QTEST_APPLESS_MAIN(FunctionTest)

#include "tst_function.moc"
