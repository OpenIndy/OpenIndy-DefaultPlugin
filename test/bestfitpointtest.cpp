#include <QtTest>
#include <QObject>
#include <QPluginLoader>
#include <QPointer>
#include <QString>
#include <QDebug>

#include <plugin.h>
#include <function.h>


class BestFitPointTest : public QObject
{
    Q_OBJECT

private slots:
    void exampleTest();
    void bestfitpointTest();

private:
    static QPointer<Function> loadFunctionPlugin(const QString &path, const QString &name);
};


QPointer<Function> BestFitPointTest::loadFunctionPlugin(const QString &path, const QString &name){
   // QFileInfo check_file(path);
    //Q_ASSERT_X(check_file.exists() && check_file.isFile(), "loadFunctionPlugin", QString("dll not found: %1").arg(path).toLatin1().data());

    QPointer<Function> function(NULL);

    QPluginLoader pluginLoader(path);
    QObject *plugin = pluginLoader.instance();
    Q_ASSERT_X(pluginLoader.isLoaded(), "loadFunctionPlugin", pluginLoader.errorString().toLatin1().data());

    if(plugin){
        Plugin *functionFactory = qobject_cast<Plugin *>(plugin);
        qDebug() << functionFactory;
        Q_ASSERT_X(plugin != 0, "loadFunctionPlugin", "functionFactory is null");
        if(functionFactory) {
            function = functionFactory->createFunction(name);
            Q_ASSERT_X(!function.isNull(), "loadFunctionPlugin", QString("function not found").arg(name).toLatin1().data());

            function->init();

        }
    }

    return function;

}

void BestFitPointTest::exampleTest(){
    QString str = "Hello";
    QVERIFY(str.toUpper() == "HELLO");
}

void BestFitPointTest::bestfitpointTest(){
    qDebug() << QString(P_DEFAULT_PLUGIN_DLL);

    QPointer<Function> function = BestFitPointTest::loadFunctionPlugin(P_DEFAULT_PLUGIN_DLL, "BestFitPoint");

    qDebug() << function->getId();

  //BestFitPoint bestFitPoint;

  //qDebug() << "bestFitPoint.getId(): " << bestFitPoint.getId();
  // QPointer<FeatureWrapper> feature = QPointer(0);
  // bestFitPoint.exec(feature);
}

QTEST_MAIN(BestFitPointTest)
#include "bestfitpointtest.moc"
