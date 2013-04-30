#include "Facade.hpp"

/*!
 * \brief Facade::Facade
 */
Facade::Facade() :
    networkRef( Network::getInstance() ),
    preprocessorRef( Preprocessor::getInstance() ),
    beta( 1.0 ),
    alpha( 1.0 ),
    accuracy( 1e-4 ),
    maxNumberOfEpoch( 1 )
{}

/*!
 * \brief Facade::getInstance
 * \return
 */
Facade &Facade::getInstance() {
    static Facade instance;
    return instance;
}

/*!
 * \brief Facade::processFinished
 */
void Facade::processFinished() {
    emit processEnd();
}

/*!
 * \brief Facade::getErrors
 * \return
 */
QVector< qreal >  Facade::getErrors() const {
    return networkRef.getNetworkError();
}

/*!
 * \brief Facade::setBeta
 * \param val
 */
void Facade::setBeta(double val) {
    beta = val;
}

/*!
 * \brief Facade::setAlhpa
 * \param val
 */
void Facade::setAlhpa(double val) {
    alpha = val;
}

/*!
 * \brief Facade::setAccuracy
 * \param val
 */
void Facade::setAccuracy(double val) {
    accuracy = val;
}

/*!
 * \brief Facade::setMaxNumberOfEpoh
 * \param val
 */
void Facade::setMaxNumberOfEpoh(int val) {
    maxNumberOfEpoch = static_cast<quint32>(val);
}

/*!
 * \brief Facade::setTrainingDataPercent
 * \param val
 */
void Facade::setTrainingDataPercent(int val) {
    preprocessorRef.setPercentageOfTest( static_cast<quint32>(val) );
}

/*!
 * \brief Facade::setInputFileName
 * \param fileName
 */
void Facade::setInputFileName(const QString fileName) {
    preprocessorRef.setInputFileName( fileName );
}

/*!
 * \brief Facade::setOutputFileName
 * \param fileName
 */
void Facade::setOutputFileName(const QString fileName) {
    preprocessorRef.setOutputFileName( fileName );
}

/*!
 * \brief Facade::setLayersDescription
 * \param layersDescription
 */
void Facade::setLayersDescription(const QVector<QPair<quint32, quint32> > layersDescription) {
    networkRef.setLayersDescription( layersDescription ); // WARNING Oleksandr Halushko type hack
}

/*!
 * \brief Facade::startProcess
 */
void Facade::startProcess() {
    const auto & trainingData = preprocessorRef.getTrainingData();
    const auto & trainingResult = preprocessorRef.getTrainingResult();

    const auto & testingData = preprocessorRef.getTestingData();
    const auto & testingResult = preprocessorRef.getTestingResult();

    networkRef.setTrainingData( trainingData );
    networkRef.setTrainigResult( trainingResult );

    networkRef.setTestingData( testingData );
    networkRef.setTestingResult( testingResult );

    networkRef.setAlpha( alpha );
    networkRef.setAccuracy( accuracy );
    networkRef.setBeta( beta );
    networkRef.setMaxNumberOfEpoch( maxNumberOfEpoch );

    QThreadPool::globalInstance()->start( &networkRef );
}

/*!
 * \brief Facade::stopProcess
 */
void Facade::stopProcess() {
    networkRef.stop();
}

/*!
 * \brief Facade::setInitialLayerInfo
 * \param val
 */
void Facade::setInitialLayerInfo( const LayerDescription &val){
    //TODO send initial layer info to neural network
    emit sendInitialLayerInfo( val );
}
