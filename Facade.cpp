#include "Facade.hpp"

/*!
 * \brief Facade::Facade
 */
Facade::Facade() :
    networkRef( Network::getInstance() ),
    preprocessorRef( Preprocessor::getInstance() )
{}

/*!
 * \brief Facade::getInstance
 * \return
 */
Facade &Facade::getInstance()
{
    static Facade instance;
    return instance;
}

/*!
 * \brief Facade::processFinished
 */
void Facade::processFinished()
{
    emit processEnd();
}

/*!
 * \brief Facade::getErrors
 * \return
 */
QVector< qreal >  Facade::getErrors() const
{
    return networkRef.getNetworkError();
}

/*!
 * \brief Facade::setBeta
 * \param val
 */
void Facade::setBeta(double val)
{
    networkRef.setBeta( val );
}

/*!
 * \brief Facade::setAlhpa
 * \param val
 */
void Facade::setAlhpa( double val )
{
    networkRef.setAlpha( val );
}

/*!
 * \brief Facade::setAccuracy
 * \param val
 */
void Facade::setAccuracy(double val)
{
    networkRef.setAccuracy( val );
}

/*!
 * \brief Facade::setMaxNumberOfEpoh
 * \param val
 */
void Facade::setMaxNumberOfEpoh(int val)
{
    networkRef.setMaxNumberOfEpoch( val );
}

/*!
 * \brief Facade::setTrainingDataPercent
 * \param val
 */
void Facade::setTrainingDataPercent(int val)
{
    preprocessorRef.setPercentageOfTest( static_cast<quint32>(val) );
}

/*!
 * \brief Facade::setInputFileName
 * \param fileName
 */
void Facade::setInputFileName(const QString fileName)
{
    preprocessorRef.setInputFileName( fileName );
}

/*!
 * \brief Facade::setOutputFileName
 * \param fileName
 */
void Facade::setOutputFileName(const QString fileName)
{
    preprocessorRef.setOutputFileName( fileName );
    auto result = networkRef.getObtainedTestingResult();
    auto data = preprocessorRef.getTestingData();
    //auto trdata = preprocessorRef.getTrainingData();
    //auto trres = preprocessorRef.getTrainingResult();
    Dataset d;
    d.resize(data.size());
    for(auto it = d.begin(); it != d.end();++it){
        auto i = it - d.begin();
       // if(i < trdata.size()){
       //     (*it).first = trdata[i];
       //     (*it).second = trres[i];
       // }else{
            (*it).first = data[i/* - trdata.size()*/];
            (*it).second = result[i /*- trdata.size()*/].getData();
       // }
    }
    preprocessorRef.saveFile( d );
}

/*!
 * \brief Facade::setLayersDescription
 * \param layersDescription
 */
void Facade::setLayersDescription(const QVector<QPair<quint32, quint32> > layersDescription)
{
    networkRef.setLayersDescription( layersDescription ); // WARNING Oleksandr Halushko type hack
}

/*!
 * \brief Facade::startProcess
 */
void Facade::startProcess()
{
    const auto & trainingData = preprocessorRef.getTrainingData();
    const auto & trainingResult = preprocessorRef.getTrainingResult();

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
void Facade::stopProcess()
{
    networkRef.stop();
}

/*!
 * \brief Facade::setInitialLayerInfo
 * \param val
 */
void Facade::setInitialLayerInfo( const LayerDescription &val)
{
    emit sendInitialLayerInfo( val );
}
