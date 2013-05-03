#include "Network.hpp"

#include "Layer.hpp"

#include <QDebug>

#include "Facade.hpp"

/*!
 * \brief Network::Network
 */
Network::Network() {
    setAutoDelete( false );
}

/*!
 * \brief Network::getInstance
 * \return
 */
Network &Network::getInstance() {
    static Network instance;
    return instance;
}

/*!
 * \brief Network::run
 */
void Network::run() {
    training( trainingData, trainigResult );
    testing( testingData, testingResult );
    Facade::getInstance().processFinished();
}

/*!
 * \brief Network::training
 * \param dataSet
 * \param desiredResult
 */
void Network::training(const Data &dataSet, const Result &desiredResult) {
    Q_ASSERT( dataSet.size() == desiredResult.size() );
    Q_ASSERT( maxNumberOfEpoch > 0 );
    quint32 epochCounter = 0;
    qreal achievedAccuracy = 1.0;

    errorList.clear(); // List contains all errors
    stopFlag = false;

    while ( true ) {

        if ( epochCounter >= maxNumberOfEpoch ) break;
        if ( achievedAccuracy <= accuracy ) break;
        if ( stopFlag ) break;

        qreal networkError = 0.0;

        // TODO for each part of data
        auto desiredResultIt = desiredResult.begin();
        std::for_each( dataSet.constBegin(), dataSet.constEnd(), [&]( Data::const_reference dataSample ) {

            // Process data through the network
            QVector < QVector < qreal > > intermidResult( layers.size() + 1 );
            {
                auto intermidIt = intermidResult.begin();
                (*intermidIt) = dataSample.getData();
                ++ intermidIt;
                // Iterate over each layer
                std::for_each( layers.constBegin(), layers.constEnd(), [&]( const Layer & layer ) {
                    // Get prev result
                    auto prevResult = intermidIt;
                    std::advance( prevResult, -1 );
                    (*intermidIt) = layer.process( (*prevResult) );
                    ++ intermidIt;
                } );
                intermidResult.remove(0);
            }

            // Calculate network error for this data and add it to error in current epoch
            {
                const auto & obtainedResult = intermidResult.last(); // Get network result
                auto obtainedResutlIt = obtainedResult.constBegin(); // Result sample
                std::for_each( (*desiredResultIt).getData().constBegin(), (*desiredResultIt).getData().constEnd(), [&]( const Result::value_type::value_type & sampleData  ) {
                    networkError += std::pow( ( sampleData - (*obtainedResutlIt) ), 2.0 );
                    ++ obtainedResutlIt;
                } );
            }

            // Calculate difference between obtained and desired output for the last layer
            QVector < QVector < qreal > > deltaVector( layers.size() );
            {
                QVector < qreal > lastLayerDiff( layers.last().getNeurons().size() );
                const auto & obtainedResult = intermidResult.last();
                {
                    Q_ASSERT( lastLayerDiff.size() == (*desiredResultIt).getData().size() );
                    const auto & desiredData = (*desiredResultIt).getData();
                    Q_ASSERT( obtainedResult.size() == desiredData.size() );
                    std::transform( obtainedResult.constBegin(), obtainedResult.constEnd(), desiredData.constBegin(), lastLayerDiff.begin(), [] ( const qreal & obtained, const qreal & desired ) {
                        return desired - obtained;
                    } );

                }

                // Calculate \delta for the last layer
                {
                    auto & lastLayerDeltaIt = deltaVector.last();
                    lastLayerDeltaIt.resize( lastLayerDiff.size() );
                    Q_ASSERT( lastLayerDeltaIt.size() == layers.last().getNeurons().size() );
                    std::transform( obtainedResult.constBegin(), obtainedResult.constEnd(), lastLayerDiff.constBegin(), lastLayerDeltaIt.begin(), [] ( const qreal & obtained, const qreal & diff ) {
                        return -obtained * derivLinLambda( obtained ) * diff;
                    } );
                }
            }

            // For each neuron from nonlast layer calculate product of childrens \delta and weights
            for ( qint32 layerIndex = layers.size() - 2; layerIndex >= 0; -- layerIndex ) {
                // Calculate sum of children
                qreal sum = 0.0;
                {
                    const auto & nextLayer = layers.at( layerIndex + 1 );
                    const auto & nextLayerDeltas = deltaVector.at( layerIndex + 1 );
                    auto deltaIt = nextLayerDeltas.begin();
                    Q_ASSERT( nextLayer.getNeurons().size() == nextLayerDeltas.size() );
                    std::for_each( nextLayer.getNeurons().constBegin(), nextLayer.getNeurons().constEnd(), [&]( const Neuron & neuron ) {
                        const auto & delta = (*deltaIt);
                        std::for_each( neuron.getWeights().constBegin(), neuron.getWeights().constEnd(), [&]( const qreal & weight ) {
                            sum += weight * delta;
                        } );
                        ++ deltaIt;
                    } );
                }
                // For each neuron calculate delta
                {
                    const auto & currentLayer = layers.at( layerIndex );
                    const auto & currentIntermidOutput = intermidResult.at( layerIndex );
                    Q_ASSERT( currentLayer.getNeurons().size() == currentIntermidOutput.size() );
                    auto & currentDeltaVector = deltaVector[ layerIndex ];
                    currentDeltaVector.resize( currentIntermidOutput.size() );
                    std::transform( currentIntermidOutput.constBegin(), currentIntermidOutput.constEnd(), currentDeltaVector.begin(), [&]( const qreal & output ) {
                        return - output * derivTanhLambda( output, beta ) * sum;
                    } );
                }
            }
            // For each layer and neuron change weights
            auto deltaIt = deltaVector.constBegin();
            std::for_each( layers.begin(), layers.end(), [&]( Layer & layer ) {
                auto currentDeltaVector = (*deltaIt);
                Q_ASSERT( layer.getNeurons().size() == currentDeltaVector.size() );

            } );

            // Go to the next part of trainign data
            ++ desiredResultIt;
        } );


        networkError /= 2;
        errorList.append( networkError ); // Save current error
        achievedAccuracy = networkError; // Get last error
        ++ epochCounter;
    }
    // TODO Oleksandr Halushko remove debug output
    qDebug() << "Error list size = " << errorList.size();
    qDebug() << "First error = " << errorList.first();
    qDebug() << "Last error = " << errorList.last();
}

/*!
 * \brief Network::testing
 * \param data
 * \return
 */
void Network::testing(const Data &data, const Result & desiredResult) {
    // TODO process all data
    // TODO calculate difference between result obtained from the network and desired result
}

/*!
 * \brief Network::getLayers
 * \return
 */
QVector<Layer> &Network::getLayers() {
    return layers;
}

/*!
 * \brief Network::getLayers
 * \return
 */
const QVector<Layer> &Network::getLayers() const {
    return layers;
}

/*!
 * \brief Network::getNetworkError
 * \return
 */
const QVector<qreal> Network::getNetworkError() const {
    const QVector< qreal > vector( errorList.toVector() );
    qDebug() << "Error vector = " << vector; // TODO remove debug output
    return vector;
}

/*!
 * \brief Network::stop
 */
void Network::stop() {
    stopFlag = true;
}

/*!
 * \brief Network::setTestingResult
 * \param value
 */
void Network::setTestingResult(const QVector<QVector<qreal> > &value) {
    testingResult.resize( value.size() );
    auto testingResultIt = testingResult.begin();
    for ( auto valueIt = value.begin(); valueIt != value.end(); ++ valueIt, ++ testingResultIt ) {
        Q_ASSERT( static_cast< quint32 >( (*valueIt).size() ) == numberOfOutputs );
        (*testingResultIt) = Result::value_type( (*valueIt) );
    }
    Q_ASSERT( testingResult.size() > 0 );
}

/*!
 * \brief Network::setTestingData
 * \param value
 */
void Network::setTestingData(const QVector<QVector<qreal> > &value) {
    testingData.resize( value.size() );
    auto testingDatatIt = testingData.begin();
    for ( auto valueIt = value.begin(); valueIt != value.end(); ++ valueIt, ++ testingDatatIt ) {
        Q_ASSERT( static_cast< quint32 >( (*valueIt).size() ) == numberOfInputs );
        (*testingDatatIt) = Data::value_type( (*valueIt) );
    }

    Q_ASSERT( testingData.size() > 0 );
}

/*!
 * \brief Network::setTrainingData
 * \param value
 */
void Network::setTrainingData(const QVector<QVector<qreal> > &value) {
    trainingData.resize( value.size() );
    auto trainigDatatIt = trainingData.begin();
    for ( auto valueIt = value.begin(); valueIt != value.end(); ++ valueIt, ++ trainigDatatIt ) {
        Q_ASSERT( static_cast< quint32 >( (*valueIt).size() ) == numberOfInputs );
        (*trainigDatatIt) = Data::value_type( (*valueIt) );
    }
    Q_ASSERT( trainingData.size() > 0 );
}

/*!
 * \brief Network::setTrainigResult
 * \param value
 */
void Network::setTrainigResult(const QVector < QVector< qreal > > &value) {
    trainigResult.resize( value.size() );
    auto trainigResultIt = trainigResult.begin();
    for ( auto valueIt = value.begin(); valueIt != value.end(); ++ valueIt, ++ trainigResultIt ) {
        Q_ASSERT( static_cast< quint32 >( (*valueIt).size() ) == numberOfOutputs );
        (*trainigResultIt) = Result::value_type( (*valueIt) );
    }
    Q_ASSERT( trainigResult.size() > 0 );
}

/*!
 * \brief Network::setLayersDescription
 * \param value
 */
void Network::setLayersDescription(const QVector<LayerDescription> &value) {
    layersDescription = value;

    Q_ASSERT( layersDescription.size() != 0 );
    Q_ASSERT( layersDescription.first().second == numberOfInputs ); // "Number of inputs to the network"
    Q_ASSERT( layersDescription.last().first == numberOfOutputs ); // "Number of outputs of the network")

    // Init new layer structure
    layers = QVector < Layer >( layersDescription.size() );

    // Init layers
    auto descriptionIt = layersDescription.begin();
    qint32 layerCount = 0;
    for ( auto layerIt = layers.begin(); layerIt != layers.end(); ++ layerIt,  ++ descriptionIt, ++ layerCount ) {
        (*layerIt).initLayer( (*descriptionIt).first, (*descriptionIt).second, beta, layerCount == (layers.size() - 1) );
    }
}

/*!
 * \brief Network::getObtainedTestingError
 * \return
 */
QVector<qreal> Network::getObtainedTestingError() const {
    return obtainedTestingError;
}

/*!
 * \brief Network::getObtainedTestingResult
 * \return
 */
Result Network::getObtainedTestingResult() const {
    return obtainedTestingResult;
}


/*!
 * \brief Network::getBeta
 * \return
 */
qreal Network::getBeta() const {
    return beta;
}

/*!
 * \brief Network::setBeta
 * \param value
 */
void Network::setBeta(const qreal &value) {
    beta = value;
    Q_ASSERT( beta != 0.0 );
}

/*!
 * \brief Network::getAlpha
 * \return
 */
qreal Network::getAlpha() const {
    return alpha;
}

/*!
 * \brief Network::setAlpha
 * \param value
 */
void Network::setAlpha(const qreal &value) {
    alpha = value;
    Q_ASSERT( alpha != 0.0 );
}

/*!
 * \brief Network::getAccuracy
 * \return
 */
qreal Network::getAccuracy() const {
    return accuracy;
}

/*!
 * \brief Network::setAccuracy
 * \param value
 */
void Network::setAccuracy(const qreal &value) {
    accuracy = value;
    Q_ASSERT( accuracy < 1.0 );
}

/*!
 * \brief Network::getMaxNumberOfEpoch
 * \return
 */
quint32 Network::getMaxNumberOfEpoch() const {
    return maxNumberOfEpoch;
}

/*!
 * \brief Network::setMaxNumberOfEpoch
 * \param value
 */
void Network::setMaxNumberOfEpoch(const quint32 &value) {
    maxNumberOfEpoch = value;
    Q_ASSERT( maxNumberOfEpoch >= 1 );
}

#ifdef TEST_MODE
void NetworkTest::ProcessTest() {

    constexpr quint32 numberOfDataSamples = 3;
    constexpr quint32 numberOfInputs = 2;
    constexpr quint32 numberOfOutputs = 1;

    Network & network = Network::getInstance();
    QVector< LayerDescription > layersDesciption;
    //    layersDesciption.append(LayerDescription(numberOfOutputs, numberOfInputs));
    layersDesciption.append(LayerDescription(5, numberOfInputs)); // Input
    layersDesciption.append(LayerDescription(10, 5)); // Hidden
    layersDesciption.append(LayerDescription(5, 10)); // Hidden
    layersDesciption.append(LayerDescription(numberOfOutputs, 5)); // Ouput

    QVector < QVector < qreal > > data( numberOfDataSamples );
    std::for_each( data.begin(), data.end(), []( QVector< qreal > & sample ) {
        sample.resize( numberOfInputs );
        std::for_each( sample.begin(), sample.end(), [] ( qreal & val ) {
            val = 2.0;
        } );
    } );

    QVector < QVector < qreal > > result( data.size() );
    std::for_each( result.begin(), result.end(), []( QVector< qreal > & sample ) {
        sample.resize( numberOfOutputs );
        std::for_each( sample.begin(), sample.end(), [] ( qreal & val ) {
            val = 10.0;
        } );
    } );


    network.setTestingData( data );
    network.setTestingResult( result );

    network.setTrainingData( data );
    network.setTrainigResult( result );

    network.setAccuracy( 1e-16 );
    network.setAlpha( 1e-5 );
    network.setBeta( 1.0 );
    network.setMaxNumberOfEpoch( 3 );

    network.setLayersDescription( layersDesciption );
    network.run();
}
#endif
