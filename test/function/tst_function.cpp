#include <QString>
#include <QtTest>
#include <QPointer>

//#include "p_oiexchangeascii.h"
//#include "featurewrapper.h"
//#include "types.h"
//#include "chooselalib.h"

//using namespace oi;

class FunctionTest : public QObject
{
    Q_OBJECT

public:
    FunctionTest();

private Q_SLOTS:
    void testRegister();
};

FunctionTest::FunctionTest()
{
}

void FunctionTest::testRegister()
{

}

QTEST_APPLESS_MAIN(FunctionTest)

#include "tst_function.moc"
