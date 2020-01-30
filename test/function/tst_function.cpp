#include <QString>
#include <QtTest>
#include <QPointer>
#include <QList>

#include "p_register.h"
#include "p_bestfitcylinder.h"
#include "featurewrapper.h"
#include "types.h"
#include "chooselalib.h"

#define COMPARE_DOUBLE(actual, expected, threshold) QVERIFY(std::abs(actual-expected)< threshold);
#define _OI_VEC(v) v.getAt(0) << "," << v.getAt(1) << "," << v.getAt(2)
#define DEBUG_CYLINDER(cylinder) qDebug() << qSetRealNumberPrecision(10) << "position=" << _OI_VEC(cylinder->getPosition().getVector()) << ", direction=" << _OI_VEC(cylinder->getDirection().getVector()) << ", radius=" << cylinder->getRadius().getRadius() << ", stdev=" << cylinder->getStatistic().getStdev();

using namespace oi;

class FunctionTest : public QObject
{
    Q_OBJECT

public:
    FunctionTest();

private Q_SLOTS:
    void printMessage(const QString &msg, const MessageTypes &msgType, const MessageDestinations &msgDest = eConsoleMessage);

    void testRegisterPoint();
    void testRegisterSphere();
    void testRegisterCircle();

    void testBestFitCylinder1();
    void testBestFitCylinder2();
    void testBestFitCylinder3();
    void testBestFitCylinder4();
    void testBestFitCylinder5();
    void testVRadial();

private:
    void addInputObservations(QString data, QPointer<Function> function, double conversionFactor = 1.0 / 1.0);


private:
    void addInputObservations(QString data, QPointer<Function> function, double conversionFactor = 1.0 / 1.0);

};

FunctionTest::FunctionTest()
{
}

void FunctionTest::printMessage(const QString &msg, const MessageTypes &msgType, const MessageDestinations &msgDest) {
    qDebug() << msg;
}

void FunctionTest::addInputObservations(QString data, QPointer<Function> function, double conversionFactor) {
    QTextStream stream(data.toUtf8());
    int id=1000;
    while(!stream.atEnd()) {
        id++;

        QStringList xyz = stream.readLine().split(" ");
        if(xyz.size() == 0) {
            continue;
        }

        OiVec * vec = new OiVec(4);
        vec->setAt(0, xyz.at(0).toDouble() * conversionFactor);
        vec->setAt(1, xyz.at(1).toDouble() * conversionFactor);
        vec->setAt(2, xyz.at(2).toDouble() * conversionFactor);
        vec->setAt(3, 1.0);

        // vecList.append(vec);

        Observation * observation = new Observation(*vec, id, true);
        observation->setIsSolved(true);

        InputElement * element = new InputElement(id);
        element->typeOfElement = eObservationElement;
        element->observation = observation;


        function->addInputElement(*element, 0);
     }
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

    OiVec planePos = OiVec(3);
    planePos.setAt(0, 1374.9964);
    planePos.setAt(1, 1624.9982);
    planePos.setAt(2, 1024.7504);
    Position planePosition(planePos);

    OiVec planeDir = OiVec(3);
    planeDir.setAt(0, 0.100818);
    planeDir.setAt(1, -0.097854);
    planeDir.setAt(2, 0.990081);
    Direction planeDirection(planeDir);

    QPointer<Plane> plane = new Plane(false, planePosition, planeDirection);
    plane->setIsSolved(true);

    InputElement element(1234); // dummy id
    element.typeOfElement = ePlaneElement;
    element.plane = plane;
    function->addInputElement(element, 0);

    bool res = function->exec(pointFeature);
    QVERIFY2(res, "exec");

    OiVec xyz = point->getPosition().getVector();
    QCOMPARE(xyz.getAt(0), 810.34019818328215);
    QCOMPARE(xyz.getAt(1), 2184.0666108152623);
    QCOMPARE(xyz.getAt(2), 1137.5033860957583);

    delete plane.data();
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

    OiVec planePos = OiVec(3);
    planePos.setAt(0, 1374.9964);
    planePos.setAt(1, 1624.9982);
    planePos.setAt(2, 1024.7504);
    Position planePosition(planePos);

    OiVec planeDir = OiVec(3);
    planeDir.setAt(0, 0.100818);
    planeDir.setAt(1, -0.097854);
    planeDir.setAt(2, 0.990081);
    Direction planeDirection(planeDir);

    QPointer<Plane> plane = new Plane(false, planePosition, planeDirection);
    plane->setIsSolved(true);

    InputElement element(1234); // dummy id
    element.typeOfElement = ePlaneElement;
    element.plane = plane;
    function->addInputElement(element, 0);

    bool res = function->exec(sphereFeature);
    QVERIFY2(res, "exec");

    OiVec xyz = sphere->getPosition().getVector();
    QCOMPARE(xyz.getAt(0), 77.722886204991397);
    QCOMPARE(xyz.getAt(1), -124.73615424728945);
    QCOMPARE(xyz.getAt(2), 983.91537298026219);

    delete plane.data();
    delete sphere.data();
    delete sphereFeature.data();
    delete function.data();
}

void FunctionTest::testRegisterCircle()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QPointer<Function> function = new Register();
    function->init();


    OiVec circlePos = OiVec(3);
    circlePos.setAt(0, 600.0108);
    circlePos.setAt(1, 499.9982);
    circlePos.setAt(2, 999.9979);

    OiVec circleDir = OiVec(3);
    circleDir.setAt(0, 0.000010);
    circleDir.setAt(1, 0.000007);
    circleDir.setAt(2, -1.000000);

    Radius radius(781.0312);
    QPointer<Circle> circle = new Circle(false, Position(circlePos), Direction(circleDir), radius);
    QPointer<FeatureWrapper> circleFeature = new FeatureWrapper();
    circleFeature->setCircle(circle);

    OiVec planePos = OiVec(3);
    planePos.setAt(0, 1374.9964);
    planePos.setAt(1, 1624.9982);
    planePos.setAt(2, 1024.7504);
    Position planePosition(planePos);

    OiVec planeDir = OiVec(3);
    planeDir.setAt(0, 0.100818);
    planeDir.setAt(1, -0.097854);
    planeDir.setAt(2, 0.990081);
    Direction planeDirection(planeDir);

    QPointer<Plane> plane = new Plane(false, planePosition, planeDirection);
    plane->setIsSolved(true);

    InputElement element(1234); // dummy id
    element.typeOfElement = ePlaneElement;
    element.plane = plane;
    function->addInputElement(element, 0);

    bool res = function->exec(circleFeature);
    QVERIFY2(res, "exec");

    OiVec xyz = circle->getPosition().getVector();
    QCOMPARE(xyz.getAt(0), 599.2600818127703);
    QCOMPARE(xyz.getAt(1), 500.72684743888169);
    QCOMPARE(xyz.getAt(2), 992.62548819327355);

    delete plane.data();
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

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1001).corrections.value("vr", -1),   0.026, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1002).corrections.value("vr", -1), (-0.007), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1003).corrections.value("vr", -1), (-0.037), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1004).corrections.value("vr", -1),   0.010, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1005).corrections.value("vr", -1), (-0.003), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1006).corrections.value("vr", -1), (-0.007), 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1007).corrections.value("vr", -1),   0.030, 0.001);
    COMPARE_DOUBLE(function->getStatistic().getDisplayResidual(1008).corrections.value("vr", -1), (-0.012), 0.001);


    delete function.data();
}

// OI-566
void FunctionTest::testBestFitCylinder1()
{
    // ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);


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
900 19.05 1205\n\
700 12.34 -1205\n\
700 12.34 1205\n\
200 12.34 -1205\n\
200 12.34 1205\n\
0 19.05 -1205\n\
0 19.05 1205\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 1205.0, 0.005);

    delete function.data();
}

// OI-566
void FunctionTest::testBestFitCylinder2()
{
    // ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

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
900 19.05 1205\n\
700 12.34 -1205\n\
700 12.34 1205\n\
200 12.34 -1205\n\
200 12.34 1205\n\
0 19.05 -1205\n\
0 19.05 1205\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    delete function.data();
}

// OI-566
void FunctionTest::testBestFitCylinder3()
{
    // ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

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
900 19.05 1205\n\
700 12.34 -1205\n\
700 12.34 1205\n\
200 12.34 -1205\n\
200 12.34 1205\n\
0 19.05 -1205\n\
0 19.05 1205\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 1206.11, 0.005);

    delete function.data();
}

// OI-566
void FunctionTest::testBestFitCylinder4()
{
    // ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

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
900 19.05 1205\n\
700 12.34 -1205\n\
700 12.34 1205\n\
200 12.34 -1205\n\
200 12.34 1205\n\
0 19.05 -1205\n\
0 19.05 1205\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 1205.01, 0.005);

    delete function.data();
}

void FunctionTest::testBestFitCylinder5()
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
10.50 0.50 1\n\
9.13 5.49 1\n\
5.48 9.13 1\n\
0.50 10.53 1\n\
-4.49 9.14 1\n\
-8.13 5.49 1\n\
-9.45 0.50 1\n\
-8.18 -4.51 1\n\
-4.52 -8.19 1\n\
0.50 -9.47 1\n\
5.51 -8.17 1\n\
9.13 -4.48 1\n\
10.69 3.60 50\n\
8.07 8.07 50\n\
3.58 10.64 50\n\
-1.60 10.70 50\n\
-6.05 8.05 50\n\
-8.65 3.59 50\n\
-8.70 -1.60 50\n\
-6.05 -6.05 50\n\
-1.59 -8.65 50\n\
3.59 -8.67 50\n\
8.05 -6.05 50\n\
10.68 -1.59 50\n\
10.27 3.66 100\n\
7.57 8.06 100\n\
3.00 10.48 100\n\
-2.16 10.27 100\n\
-6.57 7.58 100\n\
-8.98 3.00 100\n\
-8.83 -2.18 100\n\
-6.04 -6.53 100\n\
-1.49 -8.94 100\n\
3.68 -8.84 100\n\
8.05 -6.06 100\n\
10.51 -1.50 100\n\
");


    addInputObservations(data, function);

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 10.00, 0.01);
    COMPARE_DOUBLE(cylinder->getDirection().getVector().getAt(2), 0.999993, 0.000001)
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.197, 0.001);

    delete function.data();
}
QTEST_APPLESS_MAIN(FunctionTest)

#include "tst_function.moc"
