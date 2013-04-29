#include "Facade.hpp"

Facade::Facade() : networkRef( Network::getInstance() ), preprocessorRef( Preprocessor::getInstance() ) {}

/*!
 * \brief Facade::getInstance
 * \return
 */
Facade &Facade::getInstance() {
    static Facade instance; return instance;
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
void Facade::setBeta(const qreal val) {
    beta = val;
}

/*!
 * \brief Facade::setAlhpa
 * \param val
 */
void Facade::setAlhpa(const qreal val) {
    alpha = val;
}

/*!
 * \brief Facade::setAccuracy
 * \param val
 */
void Facade::setAccuracy(const qreal val) {
    accuracy = val;
}

/*!
 * \brief Facade::setMaxNumberOfEpoh
 * \param val
 */
void Facade::setMaxNumberOfEpoh(const quint32 val) {
    maxNumberOfEpoch = val;
}

/*!
 * \brief Facade::setTrainingDataPercent
 * \param val
 */
void Facade::setTrainingDataPercent(const quint32 val) {
    preprocessorRef.setPercentageOfTest( val );
}

/*!
 * \brief Facade::startProcess
 */
void Facade::startProcess() {
    const auto & trainingData = preprocessorRef.getTrainingData();
    const auto & trainingResult = preprocessorRef.getTestingResult();
    const auto & testingData = preprocessorRef.getTestingData();
    const auto & testingResult = preprocessorRef.getTestingResult();

    networkRef.setTrainingData( trainingData );
    networkRef.setTrainigResult( trainingResult );
    networkRef.setTestingData( testingData );
    networkRef.setTestingResult( testingResult );

    QThreadPool::globalInstance()->start( &networkRef );
}

/*!
 * \brief Facade::stopProcess
 */
void Facade::stopProcess() {
    networkRef.stop();
}
