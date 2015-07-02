#ifndef P_LEICATACHYMETER_H
#define P_LEICATACHYMETER_H

#include <QtGlobal>
#include <QDateTime>
#include <QObject>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QRegExp>
#include <QThread>
#include <QStringList>
#include <QVariantMap>

#include "totalstation.h"

using namespace oi;

/*!
 * \brief The LeicaTachymeter class
 */
class LeicaTachymeter : public TotalStation
{
    Q_OBJECT

public:

    //############################
    //sensor initialization method
    //############################

    void init();

    //########################
    //sensor state and actions
    //########################

    bool doSelfDefinedAction(const QString &action);

    bool abortAction();

    bool connectSensor();
    bool disconnectSensor();

    QList<QPointer<Reading> > measure(const MeasurementConfig &mConfig);
    QVariantMap readingStream(const ReadingTypes &streamFormat);

    bool getConnectionState();
    bool getIsReadyForMeasurement();
    QMap<QString, QString> getSensorStatus();
    bool getIsBusy();

protected:

    //############################
    //special totalstation methods
    //############################

    bool move(const double &azimuth, const double &zenith, const double &distance, const bool &isRelative);
    bool move(const double &x, const double &y, const double &z);

    bool toggleSightOrientation();

    //#########################
    //Leica tachymeter specific
    //#########################

    //bool getLOCKState();
    //void deactiveLockState();
    //bool setLOCKState(QString currentState);
    //bool startTargetTracking();
    //bool fineAdjust();

private:

    QPointer<QSerialPort> serial;

    QList<QPointer<Reading> > measurePolar(const MeasurementConfig &mConfig);
    QList<QPointer<Reading> > measureDistance(const MeasurementConfig &mConfig);
    QList<QPointer<Reading> > measureDirection(const MeasurementConfig &mConfig);
    QList<QPointer<Reading> > measureCartesian(const MeasurementConfig &mConfig);

    QSerialPort::BaudRate myBaudRate;
    QSerialPort::DataBits myDataBits;
    QSerialPort::Parity myParity;
    QSerialPort::StopBits myStopBits;
    QSerialPort::FlowControl myFlowControl;

    QString receive();
    //void getError(QSerialPort::SerialPortError);
    bool executeEDM();
    bool executeCommand(QString);

    bool checkCommandRC(QString command);

    SensorFaces getCurrentFace(double zenith);

    bool setTargetTypeMeasure();
    bool setTargetTypeStream();
    //bool setAdjustMode();

    //void stopTrackingAfterMeasure();

    //bool laserOn;
    //bool fineAdjusted;

    //bool measureWatchWindow;
    //bool watchWindowOpen;
    //ReadingTypes currentStreamFormat;
    //void stopWatchWindowForMeasurement();
    //void restartWatchWindowAfterMeasurement();


    //QPointer<Reading> getQuickMeasReading(QString receive);

    //void activateLaserPointer();
    //void deactivateLaserPointer();

    //void stopTrackingMode();

    QPointer<Reading> getStreamValues();

};

#endif // P_LEICATACHYMETER_H
