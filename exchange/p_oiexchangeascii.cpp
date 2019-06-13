#include "p_oiexchangeascii.h"

struct ColumnData {
    QString featureName;
    QString comment;
    QString groupName;
    QString oiFeatureCommonState;
    OiVec position;
    OiVec direction;
};

/*!
 * \brief OiExchangeAscii::init
 */
void OiExchangeAscii::init(){

    //set plugin meta data
    this->metaData.name = "OiExchangeAscii";
    this->metaData.pluginName = "OpenIndy Default Plugin";
    this->metaData.author = "bra";
    this->metaData.description = "Read simple Ascii files.";
    this->metaData.iid = ExchangeSimpleAscii_iidd;

    //set supported delimiters
    this->supportedDelimiters.append("whitespace [ ]");
    this->supportedDelimiters.append("semicolon [;]");

    //set supported geometries
    this->supportedGeometries.append(ePointGeometry);
    this->supportedGeometries.append(ePlaneGeometry);

}

/*!
 * \brief OiExchangeAscii::getDefaultColumnOrder
 * \param typeOfGeometry
 * \return
 */
QList<ExchangeSimpleAscii::ColumnType> OiExchangeAscii::getDefaultColumnOrder(const GeometryTypes &typeOfGeometry){

    QList<ExchangeSimpleAscii::ColumnType> defaultColumnOrder;

    try{

        //check if device exists
        if(this->device.isNull()){
            return defaultColumnOrder;
        }

        //if device is not opened yet, open it
        if(!this->device->isOpen()){
            this->device->open(QIODevice::ReadOnly | QIODevice::Text);
        }

        //read the first twenty lines to get the maximum number of columns
        int numColumns = 0;
        int sampleSize = 20;
        QTextStream in(this->device.data());
        while (!in.atEnd() && sampleSize >= 0){

            QString line = in.readLine();

            if(line.startsWith("#") // skip comment
               || line.startsWith(";") // skip comment
               || line.trimmed().isEmpty() // skip empty lines
                ) {
                continue;
            }

            //split the line and compare its column count to the maximum column count found before
            QStringList columns = line.split(this->getDelimiter(this->usedDelimiter));
            if(columns.size() > numColumns){
                numColumns = columns.size();
            }

        }

        //close the device
        this->device->close();

        //depending on the geometry type and the number of columns fill the default columns order
        switch(typeOfGeometry){
        case ePlaneGeometry:
            if(numColumns == 7){
                defaultColumnOrder.append(OiExchangeAscii::eColumnFeatureName);
                defaultColumnOrder.append(OiExchangeAscii::eColumnX);
                defaultColumnOrder.append(OiExchangeAscii::eColumnY);
                defaultColumnOrder.append(OiExchangeAscii::eColumnZ);
                defaultColumnOrder.append(OiExchangeAscii::eColumnPrimaryI);
                defaultColumnOrder.append(OiExchangeAscii::eColumnPrimaryJ);
                defaultColumnOrder.append(OiExchangeAscii::eColumnPrimaryK);
            }
            break;
        case ePointGeometry:
            if(numColumns == 1){
                defaultColumnOrder.append(OiExchangeAscii::eColumnX);
            }else if(numColumns == 2){
                defaultColumnOrder.append(OiExchangeAscii::eColumnX);
                defaultColumnOrder.append(OiExchangeAscii::eColumnY);
            }else if(numColumns == 3){
                defaultColumnOrder.append(OiExchangeAscii::eColumnX);
                defaultColumnOrder.append(OiExchangeAscii::eColumnY);
                defaultColumnOrder.append(OiExchangeAscii::eColumnZ);
            }else if(numColumns == 4){
                defaultColumnOrder.append(OiExchangeAscii::eColumnFeatureName);
                defaultColumnOrder.append(OiExchangeAscii::eColumnX);
                defaultColumnOrder.append(OiExchangeAscii::eColumnY);
                defaultColumnOrder.append(OiExchangeAscii::eColumnZ);
            }else if(numColumns == 5){
                defaultColumnOrder.append(OiExchangeAscii::eColumnFeatureName);
                defaultColumnOrder.append(OiExchangeAscii::eColumnX);
                defaultColumnOrder.append(OiExchangeAscii::eColumnY);
                defaultColumnOrder.append(OiExchangeAscii::eColumnZ);
                defaultColumnOrder.append(OiExchangeAscii::eColumnGroupName);
            }else if(numColumns == 6){
                defaultColumnOrder.append(OiExchangeAscii::eColumnFeatureName);
                defaultColumnOrder.append(OiExchangeAscii::eColumnX);
                defaultColumnOrder.append(OiExchangeAscii::eColumnY);
                defaultColumnOrder.append(OiExchangeAscii::eColumnZ);
                defaultColumnOrder.append(OiExchangeAscii::eColumnGroupName);
                defaultColumnOrder.append(OiExchangeAscii::eColumnComment);
            }else if(numColumns > 6){
                defaultColumnOrder.append(OiExchangeAscii::eColumnFeatureName);
                defaultColumnOrder.append(OiExchangeAscii::eColumnX);
                defaultColumnOrder.append(OiExchangeAscii::eColumnY);
                defaultColumnOrder.append(OiExchangeAscii::eColumnZ);
                defaultColumnOrder.append(OiExchangeAscii::eColumnGroupName);
                defaultColumnOrder.append(OiExchangeAscii::eColumnComment);
                for(int i = 6; i < numColumns; i++){
                    defaultColumnOrder.append(OiExchangeAscii::eColumnIgnore);
                }
            }
            break;
        }

    }catch(const exception &e){
        emit this->sendMessage(e.what(), eErrorMessage);
    }

    return defaultColumnOrder;

}

/*!
 * \brief OiExchangeAscii::getFilePreview
 * \param typeOfGeometry
 * \return
 */
QMap<ExchangeSimpleAscii::ColumnType, QVariantList> OiExchangeAscii::getFilePreview(const GeometryTypes &typeOfGeometry){

    QMap<ExchangeSimpleAscii::ColumnType, QVariantList> filePreview;

    //get the default column order
    QList<ExchangeSimpleAscii::ColumnType> defaultColumnOrder = this->getDefaultColumnOrder(typeOfGeometry);

    //insert an empty list for each column as default
    for(int i = 0; i < defaultColumnOrder.size(); i++){
        filePreview.insert(defaultColumnOrder.at(i), QVariantList());
    }

    try{

        //check if device exists
        if(this->device.isNull()){
            return filePreview;
        }

        //if device is not opened yet, open it
        if(!this->device->isOpen()){
            this->device->open(QIODevice::ReadOnly | QIODevice::Text);
        }

        //get the first twenty lines as preview
        int sampleSize = 20;
        QTextStream in(this->device);
        while (!in.atEnd() && sampleSize >= 0){

            QString line = in.readLine();

            //split the line at delimiter
            QStringList columns = line.split(this->getDelimiter(this->usedDelimiter));

            //insert the column entries
            for(int i = 0; i < defaultColumnOrder.size(); i++){

                //add an empty entry if the current row has not enough columns
                if(columns.size() <= i){
                    QVariantList rows = filePreview.value(defaultColumnOrder.at(i));
                    rows.append(QString());
                    continue;
                }

                //get the rows that already exist and append the current one
                QVariantList rows = filePreview.value(defaultColumnOrder.at(i));
                rows.append(columns.at(i));

            }

            sampleSize--;

        }

        //close the device
        this->device->close();

    }catch(const exception &e){
        emit this->sendMessage(e.what(), eErrorMessage);
    }

    return filePreview;

}

/*!
 * \brief OiExchangeAscii::getPossibleColumns
 * \param typeOfGeometry
 * \return
 */
QList<ExchangeSimpleAscii::ColumnType> OiExchangeAscii::getPossibleColumns(const GeometryTypes &typeOfGeometry){

    QList<ExchangeSimpleAscii::ColumnType> possibleColumns;

    //add all possible columns depending on the type of geometry
    switch(typeOfGeometry){
    case ePointGeometry:
        possibleColumns.append(ExchangeSimpleAscii::eColumnX);
        possibleColumns.append(ExchangeSimpleAscii::eColumnY);
        possibleColumns.append(ExchangeSimpleAscii::eColumnZ);
        possibleColumns.append(ExchangeSimpleAscii::eColumnFeatureName);
        possibleColumns.append(ExchangeSimpleAscii::eColumnGroupName);
        possibleColumns.append(ExchangeSimpleAscii::eColumnComment);
        break;
    }

    return possibleColumns;

}

/*!
 * \brief OiExchangeAscii::getDelimiter
 * \param delimiterName
 * \return
 */
QRegExp OiExchangeAscii::getDelimiter(const QString &delimiterName) const{

    if(delimiterName.compare("whitespace [ ]") == 0){
        return QRegExp("\\s+");
    }else if(delimiterName.compare("semicolon [;]") == 0){
        return QRegExp("[;]");
    }

    return QRegExp(".*");

}

/*!
 * \brief OiExchangeAscii::importOiData
 */
void OiExchangeAscii::importOiData(){

    try{

        this->features.clear();

        //check if nominal system is valid
        if(this->nominalSystem.isNull()){
            emit this->importFinished(false);
            return;
        }

        //set the number of error prone lines to 0
        int numErrors = 0;

        //check if device exists
        if(this->device.isNull()){
            emit this->importFinished(false);
            return;
        }

        //if device is not opened yet, open it
        if(!this->device->isOpen()){
            this->device->open(QIODevice::ReadOnly | QIODevice::Text);
        }

        qint64 fileSize = this->device->size();
        qint64 readSize = 0;
        qint64 numPoints = 0;

        bool notSkipped = this->getSkipFirstLine();
        //read all lines
        QTextStream in(this->device);
        while (!in.atEnd()){

            QString line = in.readLine();
            readSize += line.size();

            if(notSkipped // skip first line
               || line.startsWith("#") // skip comment
               || line.startsWith(";") // skip comment
               || line.trimmed().isEmpty() // skip empty lines
                ) {
                notSkipped = false;
                continue;
            }

            //convert , to .
            line.replace(",", "."); // decimal separator TODO use locale format

            //split the line at delimiter
            QStringList columns = line.split(this->getDelimiter(this->usedDelimiter));

            bool errorWhileParsing = false;

            ColumnData columnData = { "", "", "", "", OiVec(3), OiVec(3)};

            for(int i = 0; i < columns.size(); i++){

                //stop parsing if the current line has too many columns
                if(this->userDefinedColumns.size() <= i){
                    break;
                }

                //set the point attribute depending on the current column
                switch(this->userDefinedColumns.at(i)){
                case ExchangeSimpleAscii::eColumnCommonState:
                    // I use QT property system for transportation, because "common" is not "common" of nominal point but actual point!
                    columnData.oiFeatureCommonState = columns.at(i);
                    break;
                case ExchangeSimpleAscii::eColumnFeatureName:
                    columnData.featureName = columns.at(i);
                    break;
                case ExchangeSimpleAscii::eColumnComment:
                    columnData.comment = columns.at(i);
                    break;
                case ExchangeSimpleAscii::eColumnGroupName:
                    columnData.groupName = columns.at(i);
                    break;
                case ExchangeSimpleAscii::eColumnX:{

                    double x = 0.0;
                    x = columns.at(i).toDouble(&errorWhileParsing);
                    errorWhileParsing = !errorWhileParsing;

                    //transform the unit of the imported coordinate to [m]
                    if(this->units.contains(eMetric) && this->units.value(eMetric) != eUnitMeter){
                        x = convertToDefault(x, this->units.value(eMetric));
                    }

                    if(!errorWhileParsing){
                        columnData.position.setAt(0, x);
                    }

                    break;

                }case ExchangeSimpleAscii::eColumnY:{

                    double y = 0.0;
                    y = columns.at(i).toDouble(&errorWhileParsing);
                    errorWhileParsing = !errorWhileParsing;

                    //transform the unit of the imported coordinate to [m]
                    if(this->units.contains(eMetric) && this->units.value(eMetric) != eUnitMeter){
                        y = convertToDefault(y, this->units.value(eMetric));
                    }

                    if(!errorWhileParsing){
                        columnData.position.setAt(1, y);
                    }

                    break;

                }case ExchangeSimpleAscii::eColumnZ:{

                    double z = 0.0;
                    z = columns.at(i).toDouble(&errorWhileParsing);
                    errorWhileParsing = !errorWhileParsing;

                    //transform the unit of the imported coordinate to [m]
                    if(this->units.contains(eMetric) && this->units.value(eMetric) != eUnitMeter){
                        z = convertToDefault(z, this->units.value(eMetric));
                    }

                    if(!errorWhileParsing){
                        columnData.position.setAt(2, z);
                    }

                    break;

                }case ExchangeSimpleAscii::eColumnIgnore:
                    break;
                case ExchangeSimpleAscii::eColumnPrimaryI:
                {
                    double value = columns.at(i).toDouble(&errorWhileParsing);
                    errorWhileParsing = !errorWhileParsing;

                    //transform the unit of the imported coordinate to [m]
                    if(this->units.contains(eAngular) && this->units.value(eAngular) != eUnitDecimalDegree){
                        value = convertToDefault(value, this->units.value(eAngular));
                    }

                    if(!errorWhileParsing){
                        columnData.direction.setAt(0, value);
                    }
                    break;
                }
                case ExchangeSimpleAscii::eColumnPrimaryJ:
                {
                    double value = columns.at(i).toDouble(&errorWhileParsing);
                    errorWhileParsing = !errorWhileParsing;

                    //transform the unit of the imported coordinate to [m]
                    if(this->units.contains(eAngular) && this->units.value(eAngular) != eUnitDecimalDegree){
                        value = convertToDefault(value, this->units.value(eAngular));
                    }

                    if(!errorWhileParsing){
                        columnData.direction.setAt(1, value);
                    }
                    break;
                }
                case ExchangeSimpleAscii::eColumnPrimaryK:
                {
                    double value = columns.at(i).toDouble(&errorWhileParsing);
                    errorWhileParsing = !errorWhileParsing;

                    //transform the unit of the imported coordinate to [m]
                    if(this->units.contains(eAngular) && this->units.value(eAngular) != eUnitDecimalDegree){
                        value = convertToDefault(value, this->units.value(eAngular));
                    }

                    if(!errorWhileParsing){
                        columnData.direction.setAt(2, value);
                    }
                    break;
                }
                }

                //if an error occured continue with the next line
                if(errorWhileParsing){
                    numErrors++;
                    break;
                }

            } // loop over colums



            //create geometry and add the imported nominal to OpenIndy
            if(!errorWhileParsing){
                switch (this->typeOfGeometry) {
                case ePointGeometry:

                    QPointer<Point> myNominal = new Point(true);
                    myNominal->setProperty("OI_FEATURE_COMMONSTATE",columnData.oiFeatureCommonState);

                    myNominal->setFeatureName(columnData.featureName);
                    myNominal->setGroupName(columnData.groupName);
                    myNominal->setComment(columnData.comment);
                    myNominal->setPoint(Position(columnData.position));

                    //set group of the geometry
                    if(this->groupName.compare("") != 0){
                        myNominal->setGroupName(this->groupName);
                    }

                    //set nominal system
                    myNominal->setNominalSystem(this->nominalSystem);

                    QPointer<FeatureWrapper> myGeometry = new FeatureWrapper();
                    myGeometry->setPoint(myNominal);
                    this->features.append(myGeometry);

                    break;

                }




            }

            //update import progress
            int progress = (int)(((float)readSize / (float)fileSize) * 100.0);
            if(progress == 100){
                progress = 99;
            }
            numPoints++;
            emit this->updateProgress(progress, QString("%1 nominal(s) loaded").arg(numPoints) );
            readSize += 2;

        }

        //close the device
        this->device->close();

        //emit import finished signal
        emit this->importFinished(true);

    }catch(const exception &e){
        emit this->sendMessage(e.what(), eErrorMessage, eMessageBoxMessage);
    }

}

/*!
 * \brief OiExchangeAscii::exportOiData
 */
void OiExchangeAscii::exportOiData(){

    try{
        if(!this->device.isNull()){
            this->device->open(QIODevice::ReadWrite);
            QTextStream out(this->device);

            foreach (QPointer<FeatureWrapper> fw, this->currentJob->getGeometriesList()) {

                if(fw->getGeometry()->getIsSolved() && (!fw->getGeometry()->getIsNominal())){

                    //write to file
                    QString line = "";
                    //QString delimiter = this->getDelimiter(this->usedDelimiter);
                    for(int i=0; i< this->userDefinedColumns.size(); i++){

                        if(i>0){
                            line.append("\t");
                        }
                        switch (this->userDefinedColumns.at(i)) {
                        case eColumnFeatureName:
                            line.append(fw->getFeature()->getFeatureName());
                            break;
                        case eColumnX:
                            if(fw->getGeometry()->hasPosition()){
                                line.append(QString::number(convertFromDefault(fw->getGeometry()->getPosition().getVector().getAt(0),this->units[eMetric]),'f',this->getDistanceDigits()));
                            }else{
                                line.append("-");
                            }
                            break;
                        case eColumnY:
                            if(fw->getGeometry()->hasPosition()){
                                line.append(QString::number(convertFromDefault(fw->getGeometry()->getPosition().getVector().getAt(1),this->units[eMetric]),'f',this->getDistanceDigits()));
                            }else{
                                line.append("-");
                            }
                            break;
                        case eColumnZ:
                            if(fw->getGeometry()->hasPosition()){
                                line.append(QString::number(convertFromDefault(fw->getGeometry()->getPosition().getVector().getAt(2),this->units[eMetric]),'f',this->getDistanceDigits()));
                            }else{
                                line.append("-");
                            }
                            break;
                        case eColumnPrimaryI:
                            if(fw->getGeometry()->hasDirection()){
                                line.append(QString::number(fw->getGeometry()->getDirection().getVector().getAt(0),'f',this->getAngleDigits()));
                            }else{
                                line.append("-");
                            }
                            break;
                        case eColumnPrimaryJ:
                            if(fw->getGeometry()->hasDirection()){
                                line.append(QString::number(fw->getGeometry()->getDirection().getVector().getAt(1),'f',this->getAngleDigits()));
                            }else{
                                line.append("-");
                            }
                            break;
                        case eColumnPrimaryK:
                            if(fw->getGeometry()->hasDirection()){
                                line.append(QString::number(fw->getGeometry()->getDirection().getVector().getAt(2),'f',this->getAngleDigits()));
                            }else{
                                line.append("-");
                            }
                            break;
                        case eColumnRadiusA:
                            if(fw->getGeometry()->hasRadius()){
                                line.append(QString::number(convertFromDefault(fw->getGeometry()->getRadius().getRadius(),this->units[eMetric]),'f',this->getDistanceDigits()));
                            }else{
                                line.append("-");
                            }
                            line.append("\n");
                            break;
                        default:
                            break;
                        }
                    }
                    //write to file
                    out << line;
                }
            }
            this->device->close();
            emit this->exportFinished(true);

        }else{
            qDebug() << "export device is NULL";
            emit this->exportFinished(false);
        }

    }catch(const exception &e){
        emit this->sendMessage(e.what(), eErrorMessage, eMessageBoxMessage);
    }

}
