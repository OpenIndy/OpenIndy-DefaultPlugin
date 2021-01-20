#include "configuredfunction1.h"

ConfiguredFunction1::ConfiguredFunction1(ConfiguredFunctionConfig config, QList<QPointer<Function> > functions, QObject *parent): Function(parent), functions(functions), config(config) {
}

void ConfiguredFunction1::init() {
    this->metaData.name = this->config.name;
    this->applicableFor = this->config.applicableFor;
    this->neededElements = config.getNeededElements();
}

bool ConfiguredFunction1::exec(const QPointer<FeatureWrapper> &feature) {

    //execute all functions in the specified order
    for(int functionIndex = 0; functionIndex < this->functions.size(); functionIndex++){

        // break if the function pointer is not valid
        if(functions[functionIndex].isNull()){
            return false;
        }

        foreach(InputElementMapping mapping, this->config.inputElementsMapping.values(functionIndex)) {
            foreach(InputElement inputElement, this->inputElements.value(mapping.srcInputElementIndex)) {
                functions[mapping.functionIndex]->addInputElement(inputElement, mapping.dstInputElementIndex);
            }
        }

        // try to solve the current function
        if(!functions[functionIndex]->exec(feature)){
            return false;
        }
    }

    return true;
}
