// compare double with threshold
#define COMPARE_DOUBLE(actual, expected, threshold) QVERIFY2(std::abs(actual-expected)< threshold, QString("actual: %1, expected: %2").arg(actual).arg(expected).toLatin1().data());

// helper
#define _OI_VEC(v) v.getAt(0) << "," << v.getAt(1) << "," << v.getAt(2)

// debug / pretty print OI feature objects
#define DEBUG_CYLINDER(feature) qDebug() << qSetRealNumberPrecision(10) \
        << "position=" << _OI_VEC(feature->getPosition().getVector()) \
        << ", direction=" << _OI_VEC(feature->getDirection().getVector()) \
        << ", radius=" << feature->getRadius().getRadius() \
        << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_CIRCLE(feature)   qDebug() << qSetRealNumberPrecision(10) \
        << "position=" << _OI_VEC(feature->getPosition().getVector()) \
        << ", direction=" << _OI_VEC(feature->getDirection().getVector()) \
        << ", radius=" << feature->getRadius().getRadius() \
        << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_SPHERE(feature)   qDebug() << qSetRealNumberPrecision(10) \
        << "position=" << _OI_VEC(feature->getPosition().getVector()) \
        << ", direction=" << _OI_VEC(feature->getDirection().getVector()) \
        << ", radius=" << feature->getRadius().getRadius() \
        << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_PLANE(feature)    qDebug() << qSetRealNumberPrecision(10) \
        << "position=" << _OI_VEC(feature->getPosition().getVector()) \
        << ", direction=" << _OI_VEC(feature->getDirection().getVector()) \
        << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_POINT(feature)    qDebug() << qSetRealNumberPrecision(10) \
        << "position=" << _OI_VEC(feature->getPosition().getVector()) \
        << ", direction=" << _OI_VEC(feature->getDirection().getVector()) \
        << ", stdev=" << feature->getStatistic().getStdev();
#define DEBUG_LINE(feature)     qDebug() << qSetRealNumberPrecision(10) \
        << "position=" << _OI_VEC(feature->getPosition().getVector()) \
        << ", direction=" << _OI_VEC(feature->getDirection().getVector()) \
        << ", stdev=" << feature->getStatistic().getStdev();

#define DEBUG_DISTANCE(feature) qDebug() << qSetRealNumberPrecision(10) \
        << "distance=" << feature->getDistance();

#define DEBUG_RESIDULAS(function) foreach(Residual r, function->getStatistic().getDisplayResiduals()) { qDebug() << qSetRealNumberPrecision(10) << r.elementId << ": " << r.corrections; }

#define DEBUG_TRAFOPARAM(feature) qDebug() << qSetRealNumberPrecision(10) \
        << "r [radian]=" << _OI_VEC(feature.getRotation()) \
        << ", r [degree]=" << _OI_VEC((feature.getRotation()* RHO_DEGREE)) \
        << ", t=" << _OI_VEC(feature.getTranslation()) \
        << ", s=" << _OI_VEC(feature.getScale()) \
        << ", stdev=" << feature.getStatistic().getStdev();

