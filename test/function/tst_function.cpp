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

    void testBestFitCylinder5();
    void testBestFitCylinder6();
    void testBestFitCylinder7();
    void testBestFitCylinder8();
    void testBestFitCylinder9();
    void testBestFitCylinder10();

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

        Observation * observation = new Observation(*vec, true);
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
void FunctionTest::testBestFitCylinder2()
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
void FunctionTest::testBestFitCylinder3()
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
void FunctionTest::testBestFitCylinder4()
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

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-298
void FunctionTest::testBestFitCylinder6()
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
void FunctionTest::testBestFitCylinder7()
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

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-298
void FunctionTest::testBestFitCylinder8()
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

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-298
void FunctionTest::testBestFitCylinder9()
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

    bool res = function->exec(cylinderFeature);
    QVERIFY2(res, "exec");

    DEBUG_CYLINDER(cylinder);

    COMPARE_DOUBLE(cylinder->getRadius().getRadius(), 19.16, 0.005);
    COMPARE_DOUBLE(cylinder->getStatistic().getStdev(), 0.03, 0.01);

    delete function.data();
}

// OI-298 direction
void FunctionTest::testBestFitCylinder10()
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

QTEST_APPLESS_MAIN(FunctionTest)

#include "tst_function.moc"
