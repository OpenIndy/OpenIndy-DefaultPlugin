#include <QString>
#include <QtTest>
#include <QPointer>
#include <QList>
#include "plugin.h"

using namespace oi;

class LoadPluginTest : public QObject
{
    Q_OBJECT

public:
    LoadPluginTest();

private Q_SLOTS:
    void testPseudoTracker_eCartesianReading();
    void testLoadPlugin();

private:

};

LoadPluginTest::LoadPluginTest()
{
}

/*
 * PATH:
 * OpenIndy-DefaultPlugin\lib\OpenIndy-Core\bin\debug\;
 * OpenIndy-DefaultPlugin\lib\OpenIndy-Core\lib\OpenIndy-Math\bin\debug\;
 * OpenIndy-DefaultPlugin\lib\OpenIndy-Core\lib\OpenIndy-Math\lib\armadillo-3.910.0\examples\lib_win64\
 *
 * Cannot load library ... Das angegebene Modul wurde nicht gefunden.
 * -> dll kann nicht geladen werden, weil z. B. benötigte dll nicht im Pfad ist
 *
 * Cannot load library ... Die angegebene Prozedur wurde nicht gefunden.
 * -> ???
 */
void LoadPluginTest::testLoadPlugin(){

    QDir dir;

#ifdef QT_DEBUG
    QString pluginPath = dir.absoluteFilePath("../../bin/debug/p_defaultPlugind1.dll");
#else
    QString pluginPath = dir.absoluteFilePath("../../bin/release/p_defaultPlugin1.dll");
#endif

    QVERIFY2(QFile::exists(pluginPath), pluginPath.toLatin1().data());
    QPluginLoader pluginLoader(pluginPath);

    QObject *plugin = pluginLoader.instance();
    QVERIFY2(plugin, pluginLoader.errorString().toLatin1().data());

    Plugin *oiPlugin = qobject_cast<Plugin *>(plugin);
    QVERIFY2(oiPlugin, "qobject_cast");

    QList<QPointer<Function> > functions = oiPlugin->createFunctions();
    QVERIFY2(functions.size() > 0, "no functions found");

}
void LoadPluginTest::testPseudoTracker_eCartesianReading(){

    QDir dir;

#ifdef QT_DEBUG
    QString pluginPath = dir.absoluteFilePath("../../bin/debug/p_defaultPlugind1.dll");
#else
    QString pluginPath = dir.absoluteFilePath("../../bin/release/p_defaultPlugin1.dll");
#endif

    QVERIFY2(QFile::exists(pluginPath), pluginPath.toLatin1().data());
    QPluginLoader pluginLoader(pluginPath);

    QObject *plugin = pluginLoader.instance();
    QVERIFY2(plugin, pluginLoader.errorString().toLatin1().data());

    Plugin *oiPlugin = qobject_cast<Plugin *>(plugin);
    QVERIFY2(oiPlugin, "qobject_cast");

    QPointer<Sensor> sensor = oiPlugin->createSensor("PseudoTracker");

    sensor->init();

    MeasurementConfig config;
    config.setMeasurementType(eSinglePoint_MeasurementType);
    SensorConfiguration sc = sensor->getSensorConfiguration();
    QMap<QString, QString> sp = sc.getStringParameter();
    sp.insert("reading type", "cartesian");
    sc.setStringParameter(sp);
    sensor->setSensorConfiguration(sc);
    QList<QPointer<Reading> > readings = sensor->measure(config);
    QVERIFY2(readings.size() > 0, "no readings");

}

QTEST_APPLESS_MAIN(LoadPluginTest)

#include "tst_loadplugin.moc"
