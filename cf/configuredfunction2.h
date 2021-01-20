#ifndef CONFIGUREDFUNCTION2_H
#define CONFIGUREDFUNCTION2_H

#include <QObject>

class ConfiguredFunction2 : public Function {
    friend class Feature;
    friend class CFVisitor;

    Q_OBJECT

public:
    ConfiguredFunction2(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent = 0);

    QString prettyPrint();

public:
    QMap<int, QList<InputElement> > global_inputElements;

private:
    ConfiguredFunctionConfig config;
    // all necessary funktions
    QList<QPointer<Function> > functions;

};

#endif // CONFIGUREDFUNCTION2_H
