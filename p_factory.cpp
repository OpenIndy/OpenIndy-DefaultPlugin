#include "p_factory.h"
#include "sensorfacade.h"

using namespace oi;

//int    gDebugLevel=0;
//TReal  gConvergence=1e-9;

OiTemplatePlugin::OiTemplatePlugin(QObject *parent) : QObject(parent){

}

/*!
 * \brief OiTemplatePlugin::createSensors
 * Returns a list with all sensors
 * \return
 */
QList<QPointer<Sensor> > OiTemplatePlugin::createSensors(){
    QList<QPointer<Sensor> > resultSet;
    QPointer<Sensor> pTracker = new SensorFacade(new PseudoTracker());
    QPointer<Sensor> LeicaTachy = new LeicaTachymeter();
    resultSet.append(pTracker);
    resultSet.append(LeicaTachy);
    return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createFunctions
 * Returns a list with all functions
 * \return
 */
QList<QPointer<Function> > OiTemplatePlugin::createFunctions(){
    QList<QPointer<Function> > resultSet;

    resultSet.append(new BestFitPoint());
    resultSet.append(new BestFitLine());
    resultSet.append(new BestFitPlane());
    //resultSet.append(new BestFitPlane_kern());
    //resultSet.append(new BestFitSphere_kern());
    resultSet.append(new PointFromPoints());
    resultSet.append(new LineFromPoints());
    resultSet.append(new PlaneFromPoints());
    resultSet.append(new IntersectLineLine());
    resultSet.append(new IntersectLinePlane());
    resultSet.append(new IntersectPlanePlane());
    resultSet.append(new SphereFromPoints());
    resultSet.append(new ChangeRadius());
    resultSet.append(new TranslateByValue());
    resultSet.append(new Register());
    resultSet.append(new BestFitSphere());
    resultSet.append(new BestFitCircleInPlane());
    resultSet.append(new BestFitCircleInPlaneFromPoints());
    resultSet.append(new RectifyToPoint());
    resultSet.append(new RectifyToVector());
    resultSet.append(new Shift());
    resultSet.append(new BestFitCylinder());
    resultSet.append(new BestFitCylinderAppxDirection());
    resultSet.append(new BestFitCylinderAppxDummyPoint());
    resultSet.append(new BestFitCylinderFromPoints());
    resultSet.append(new BestFitCylinderFromPointsAppxDirection());
    resultSet.append(new BestFitCylinderFromPointsAppxDummyPoint());

    FunctionConfigParser parser;
    foreach(ConfiguredFunctionConfig config, parser.readConfigFromJson()) {
        QList<QPointer<Function> > functions;
        if(config.version == 2) {
            resultSet.append(new ConfiguredFunction(config, functions));// this instance contains only function name!
        }
    }

    return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createBundleAdjustments
 * Returns a list with all network adjustments
 * \return
 */
QList<QPointer<BundleAdjustment> > OiTemplatePlugin::createBundleAdjustments(){
    QList<QPointer<BundleAdjustment> > resultSet;
    return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createSimulations
 * Returns a list with all simulations
 * \return
 */
QList<QPointer<SimulationModel> > OiTemplatePlugin::createSimulations(){
    QList<QPointer<SimulationModel> > resultSet;
    resultSet.append(new SimplePolarMeasurement());
    return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createTools
 * \return
 */
QList<QPointer<Tool> > OiTemplatePlugin::createTools(){
    QList<QPointer<Tool> > resultSet;
    return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createSimpleAsciiExchanges
 * \return
 */
QList<QPointer<ExchangeSimpleAscii> > OiTemplatePlugin::createSimpleAsciiExchanges(){
    QList<QPointer<ExchangeSimpleAscii> > resultSet;
    resultSet.append(new OiExchangeAscii());
    return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createDefinedFormatExchanges
 * \return
 */
QList<QPointer<ExchangeDefinedFormat> > OiTemplatePlugin::createDefinedFormatExchanges(){
    QList<QPointer<ExchangeDefinedFormat> > resultSet;
    return resultSet;
}

/*!
 * \brief OiTemplatePlugin::createSensor
 * Returns the sensor with the specified name
 * \param name
 * \return
 */
QPointer<Sensor> OiTemplatePlugin::createSensor(const QString &name){
    QPointer<Sensor> result(NULL);
    if(name.compare("PseudoTracker") == 0){
        result = new SensorFacade(new PseudoTracker());
    }else if(name.compare("LeicaTachymeter") == 0){
        result = new LeicaTachymeter();
    }
    return result;
}

/*!
 * \brief OiTemplatePlugin::createFunction
 * Returns the function with the specified name
 * \param name
 * \return
 */
QPointer<Function> OiTemplatePlugin::createFunction(const QString &name){
    QPointer<Function> result(NULL);
    if(name.compare("BestFitPoint") == 0){
        result = new BestFitPoint();
    }else if(name.compare("BestFitLine") == 0){
        result = new BestFitLine();
    }else if(name.compare("BestFitPlane") == 0){
        result = new BestFitPlane();
    }/*else if(name.compare("BestFitPlane_kern") == 0){
        result = new BestFitPlane_kern();
    }else if(name.compare("BestFitSphere_kern") == 0){
        result = new BestFitSphere_kern();
    }*/else if(name.compare("PointFromPoints") == 0){
        result = new PointFromPoints();
    }else if(name.compare("LineFromPoints") == 0){
        result = new LineFromPoints();
    }else if(name.compare("PlaneFromPoints") == 0){
        result = new PlaneFromPoints();
    }else if(name.compare("IntersectLineLine") == 0){
        result = new IntersectLineLine();
    }else if(name.compare("IntersectLinePlane") == 0){
        result = new IntersectLinePlane();
    }else if(name.compare("IntersectPlanePlane") == 0){
        result = new IntersectPlanePlane();
    }else if(name.compare("SphereFromPoints") == 0){
        result = new SphereFromPoints();
    }else if(name.compare("ChangeRadius") == 0){
        result = new ChangeRadius();
    }else if(name.compare("TranslateByValue") == 0){
        result = new TranslateByValue();
    }else if(name.compare("Register") == 0){
        result = new Register();
    /*}else if(name.compare("HelmertTransformation") == 0){
        result = new Helmert7Param();*/
    }/*else if(name.compare("BestFitCircle") == 0){
        result = new BestFitCircle();
    }*/else if(name.compare("BestFitCircleInPlane") == 0){
        result = new BestFitCircleInPlane();
    } else if(name.compare("BestFitCircleInPlaneFromPoints") == 0){
        result = new BestFitCircleInPlaneFromPoints();
    }/*else if(name.compare("9ParameterHelmertTransformation") == 0){
        result = new ExtendedTemperatureCompensation();
    }*/else if(name.compare("BestFitSphere") == 0){
        result = new BestFitSphere();
    }else if(name.compare("RectifyToPoint") == 0){
        result = new RectifyToPoint();
    }else if(name.compare("Shift") == 0){
        result = new Shift();
    }else if(name.compare("RectifyToVector") == 0){
        return new RectifyToVector();
    }else if(name.compare("BestFitCylinder") == 0){
        return new BestFitCylinder();
    }else if(name.compare("BestFitCylinderAppxDirection") == 0){
        return new BestFitCylinderAppxDirection();
    }else if(name.compare("BestFitCylinderAppxDummyPoint") == 0){
        return new BestFitCylinderAppxDummyPoint();
    }else if(name.compare("BestFitCylinderFromPoints") == 0){
        return new BestFitCylinderFromPoints();
    }else if(name.compare("BestFitCylinderFromPointsAppxDirection") == 0){
        return new BestFitCylinderFromPointsAppxDirection();
    }else if(name.compare("BestFitCylinderFromPointsAppxDummyPoint") == 0){
        return new BestFitCylinderFromPointsAppxDummyPoint();
    } else {
        return createFunctionFromConfig(name);
    }
    return result;
}

QPointer<Function> OiTemplatePlugin::createFunctionFromConfig(const QString &functionName){
    // internal functions
    if(functionName.compare("DistanceBetweenTwoPoints") == 0){
        return new DistanceBetweenTwoPoints();
    } else if(functionName.compare("XYFilter") == 0){
        return new XYZFilter("XYFilter", true, true, false); // keep Z
    } else if(functionName.compare("XZFilter") == 0){
        return new XYZFilter("XZFilter", true, false, true); // keep Y
    } else if(functionName.compare("YZFilter") == 0){
        return new XYZFilter("YZFilter", false, true, true); // keep X
    }

    QPointer<Function> function(NULL);

    FunctionConfigParser parser;
    foreach(ConfiguredFunctionConfig config, parser.readConfigFromJson()) {
        if(config.name.compare(functionName) == 0) {
            QList<QPointer<Function> > functions;
            foreach(QString name, config.getFunctionNames()) {
                QPointer<Function> f = createFunction(name);
                if(f.isNull()) {
                    throw logic_error(QString("no function found: %1").arg(name).toLocal8Bit().data());
                }
                f->init();
                functions.append(f);
            }

            if(config.version == 2) {
                function = new ConfiguredFunction(config, functions);
                function->init();
                break;
            }
        }

        if(!function.isNull()) {
            return function;
        }

    }

    return function;
}

/*!
 * \brief OiTemplatePlugin::createBundleAdjustment
 * Returns the network adjustment with the specified name
 * \param name
 * \return
 */
QPointer<BundleAdjustment> OiTemplatePlugin::createBundleAdjustment(const QString &name){
    QPointer<BundleAdjustment> result(NULL);
    return result;
}

/*!
 * \brief OiTemplatePlugin::createSimulation
 * Returns the simulation with the specified name
 * \param name
 * \return
 */
QPointer<SimulationModel> OiTemplatePlugin::createSimulation(const QString &name){
    QPointer<SimulationModel> result(NULL);

    if(name.compare("SimplePolarMeasurement") == 0){
        result = new SimplePolarMeasurement();
    }

    return result;
}

/*!
 * \brief OiTemplatePlugin::createTool
 * \param name
 * \return
 */
QPointer<Tool> OiTemplatePlugin::createTool(const QString &name){
    QPointer<Tool> result(NULL);
    return result;
}

/*!
 * \brief OiTemplatePlugin::createSimpleAsciiExchange
 * \param name
 * \return
 */
QPointer<ExchangeSimpleAscii> OiTemplatePlugin::createSimpleAsciiExchange(const QString &name){

    QPointer<ExchangeSimpleAscii> result(NULL);

    if(name.compare("OiExchangeAscii") == 0){
        result = new OiExchangeAscii();
    }

    return result;

}

/*!
 * \brief OiTemplatePlugin::createDefinedFormatExchange
 * \param name
 * \return
 */
QPointer<ExchangeDefinedFormat> OiTemplatePlugin::createDefinedFormatExchange(const QString &name){
    QPointer<ExchangeDefinedFormat> result(NULL);
    return result;
}
