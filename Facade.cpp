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
    // TODO set value for preprocessor
}

/*!
 * \brief Facade::startProcess
 */
void Facade::startProcess() {
    // TODO get training data
    // TODO get training result
    // TODO get testing data
    // TODO get testing results
    // TODO pass all to the network
    QThreadPool::globalInstance()->start( &networkRef );
}

/*!
 * \brief Facade::stopProcess
 */
void Facade::stopProcess() {
    networkRef.stop();
}
