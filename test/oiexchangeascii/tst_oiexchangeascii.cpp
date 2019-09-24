#include <QString>
#include <QtTest>
#include <QPointer>

#include "p_oiexchangeascii.h"
#include "featurewrapper.h"
#include "types.h"
#include "chooselalib.h"

using namespace oi;

class OiExchangeAsciiTest : public QObject
{
    Q_OBJECT

public:
    OiExchangeAsciiTest();

private Q_SLOTS:
    void testImportNominal();
    void testImportLevel();
};

OiExchangeAsciiTest::OiExchangeAsciiTest()
{
}

void OiExchangeAsciiTest::testImportNominal()
{

    QString data("\
Name	x	y	z\r\n\
P1004	-969.17	-566.68	-514.68\r\n\
P14	-956.26	-224.04	302.29\r\n\
P16	-876.67	1052.96	281.97\r\n\
P17	-870.8	1034.19	-450.19\r\n\
P19	-802.72	2226.45	296.64\r\n\
P20	-749.56	2987.57	-453.59\r\n\
");

    // DataExchanger::importData line 115 ...
    OiExchangeAscii *exchange = new OiExchangeAscii();
    exchange->init();

    //setup
    exchange->setGeometryType(ePointGeometry);
    exchange->setSkipFirstLine(true);
    exchange->setDelimiter("whitespace [ ]");

    QList<ExchangeSimpleAscii::ColumnType> columns;
    columns.append(ExchangeSimpleAscii::eColumnFeatureName);
    columns.append(ExchangeSimpleAscii::eColumnX);
    columns.append(ExchangeSimpleAscii::eColumnY);
    columns.append(ExchangeSimpleAscii::eColumnZ);
    exchange->setUserDefinedColumns(columns);


    QByteArray ba = data.toUtf8();
    exchange->setDevice(QPointer<QIODevice>(new QBuffer(&ba)));

    exchange->setFeatures(QList<QPointer<FeatureWrapper> >()); // only export
    exchange->setExportObservations(false); // only export
    exchange->setGroupName("Group01");
    exchange->setNominalSystem(QPointer<CoordinateSystem>(new CoordinateSystem(QPointer<Station>())) /*features.at(0)->getCoordinateSystem()*/);

    exchange->setUnit(eMetric, eUnitMilliMeter);
    exchange->setUnit(eAngular, eUnitDecimalDegree);
    exchange->setUnit(eTemperature, eUnitGrad);


    //import data
    exchange->importOiData();

    // test
    QVERIFY(exchange->getFeatures().size() == 6);

    QPointer<Point> point = exchange->getFeatures().at(0)->getPoint();
    QCOMPARE(point->getFeatureName(), QString("P1004"));
    QCOMPARE(point->getGroupName(), QString("Group01"));
    OiVec xyz = point->getPosition().getVector();
    QCOMPARE(xyz.getAt(0), -.96917);
    QCOMPARE(xyz.getAt(1), -.56668);
    QCOMPARE(xyz.getAt(2), -.51468);

    QCOMPARE(exchange->getFeatures().at(1)->getPoint()->getFeatureName(), QString("P14"));
    QCOMPARE(exchange->getFeatures().at(2)->getPoint()->getFeatureName(), QString("P16"));
    QCOMPARE(exchange->getFeatures().at(3)->getPoint()->getFeatureName(), QString("P17"));
    QCOMPARE(exchange->getFeatures().at(4)->getPoint()->getFeatureName(), QString("P19"));
    QCOMPARE(exchange->getFeatures().at(5)->getPoint()->getFeatureName(), QString("P20"));

    delete exchange;
}

void OiExchangeAsciiTest::testImportLevel()
{
    ChooseLALib::setLinearAlgebra(ChooseLALib::Armadillo);

    QString data("\
name x y z i j k\r\n\
# Level as Plane\r\n\
\r\n\
Level_1 0.0 0.0 0.0 0.001310 -0.001903 0.999997\r\n\
\r\n\
Level_2  0.0 0.0 0.0 0.001023 -0.001945 0.999989\r\n\
#Level_2  0.0 0.0 0.0 0.001018 -0.001951 0.999994\r\n\
");

    // DataExchanger::importData line 115 ...
    OiExchangeAscii *exchange = new OiExchangeAscii();
    exchange->init();

    //setup
    exchange->setGeometryType(ePlaneGeometry);
    exchange->setSkipFirstLine(true);
    exchange->setDelimiter("whitespace [ ]");

    QByteArray ba = data.toUtf8();
    exchange->setDevice(QPointer<QIODevice>(new QBuffer(&ba)));

    // read column count from file / device
    exchange->setUserDefinedColumns(exchange->getDefaultColumnOrder(ePlaneGeometry));

    exchange->setFeatures(QList<QPointer<FeatureWrapper> >()); // only export
    exchange->setExportObservations(false); // only export
    exchange->setGroupName("Group01");
    exchange->setNominalSystem(QPointer<CoordinateSystem>(new CoordinateSystem(QPointer<Station>())) /*features.at(0)->getCoordinateSystem()*/);

    exchange->setUnit(eMetric, eUnitMilliMeter);
    exchange->setUnit(eAngular, eUnitDecimalDegree);
    exchange->setUnit(eTemperature, eUnitGrad);


    //import data
    exchange->importOiData();

    // test
    QVERIFY(exchange->getFeatures().size() == 2);

    QPointer<Plane> plane = exchange->getFeatures().at(0)->getPlane();
    QCOMPARE(plane->getFeatureName(), QString("Level_1"));
    QCOMPARE(plane->getGroupName(), QString("Group01"));
    OiVec xyz = plane->getPosition().getVector();
    QCOMPARE(xyz.getAt(0), .0);
    QCOMPARE(xyz.getAt(1), .0);
    QCOMPARE(xyz.getAt(2), .0);

    OiVec ijk = plane->getDirection().getVector();
    QCOMPARE(ijk.getAt(0), 0.001310);
    QCOMPARE(ijk.getAt(1), -0.001903);
    QCOMPARE(ijk.getAt(2), 0.999997);

    QCOMPARE(exchange->getFeatures().at(1)->getPlane()->getFeatureName(), QString("Level_2"));

    delete exchange;
}

QTEST_APPLESS_MAIN(OiExchangeAsciiTest)

#include "tst_oiexchangeascii.moc"
