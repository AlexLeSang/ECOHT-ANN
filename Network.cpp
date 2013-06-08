#include "Network.hpp"

#include "Layer.hpp"

#include <QDebug>

#include "Facade.hpp"

/*!
 * \brief Network::Network
 */
Network::Network() : maxNumberOfEpoch( 50 ), accuracy( 1e-4 ), alpha( 1.0 ), beta( 1.0 )
{
    setAutoDelete( false );
}

/*!
 * \brief Network::getInstance
 * \return
 */
Network &Network::getInstance()
{
    static Network instance;
    return instance;
}

/*!
 * \brief Network::initLayers
 */
void Network::initLayers()
{
    Q_ASSERT( beta != 0.0 );
    Q_ASSERT( alpha != 0.0 );
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
 * \brief Network::run
 */
void Network::run()
{
    initLayers();
    training( trainingData, trainigResult );
    testing( testingData, testingResult );
    Facade::getInstance().processFinished();
}

/*!
 * \brief Network::processInNetwork
 * \param dataSample
 * \return
 */
QVector < QVector< qreal > > Network::processInNetwork( Data::const_reference dataSample )
{
    QList < QVector < qreal > > intermidResult;
    {
        intermidResult.append( dataSample.getData() );
        // Iterate over each layer
        std::for_each( layers.constBegin(), layers.constEnd(), [&]( const Layer & layer ) {
            auto result = layer.process( intermidResult.last() );
            intermidResult.append( result );
        } );
        intermidResult.removeFirst();
    }

    return intermidResult.toVector();
}

/*!
 * \brief Network::training
 * \param dataSet
 * \param desiredResult
 */
void Network::training(const Data &dataSet, const Result &desiredResult)
{
    Q_ASSERT( dataSet.size() == desiredResult.size() );
    Q_ASSERT( maxNumberOfEpoch >= 1 );
    quint32 epochCounter = 0;
    qreal achievedAccuracy = 1.0;

    errorList.clear(); // List contains all errors
    stopFlag = false;

    while ( true ) {

        if ( epochCounter >= maxNumberOfEpoch ) break;
        if ( achievedAccuracy <= accuracy ) break;
        if ( stopFlag ) break;

        qreal networkError = 0.0;

        // For each part of data
        auto desiredResultIt = desiredResult.begin();
        std::for_each( dataSet.constBegin(), dataSet.constEnd(), [&]( Data::const_reference dataSample ) {

            // Process data through the network
            const QVector < QVector < qreal > > intermidResult = processInNetwork( dataSample );

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
//                         return - output * derivTanhLambda( output, beta ) * sum;
                         return - output * derivLinLambda( output ) * sum;
                    } );
                }
            }
            // For each layer and neuron change weights
            {
                auto deltaIt = deltaVector.constBegin();
                std::for_each( layers.begin(), layers.end(), [&]( Layer & layer ) {
                    auto currentDeltaVector = (*deltaIt);
                    Q_ASSERT( layer.getNeurons().size() == currentDeltaVector.size() );
                    auto currentDetlaIt = currentDeltaVector.begin();
                    std::for_each( layer.getNeurons().begin(), layer.getNeurons().end(), [&]( Neuron & neuron ) {
                        const auto currentDelta = (*currentDetlaIt);
                        std::for_each( neuron.getWeights().begin(), neuron.getWeights().end(), [&] ( qreal & weight ) {
                            weight += -alpha * currentDelta;
                        } );
                        ++ currentDetlaIt;
                    } );
                    ++ deltaIt;
                } );
            }
            // Go to the next part of trainign data
            ++ desiredResultIt;
        } );


        networkError /= 2;
        errorList.append( networkError ); // Save current error
        achievedAccuracy = networkError; // Get last error
        ++ epochCounter;
    }
    // TODO Oleksandr Halushko remove debug output
    /*
    {
        qDebug() << "Error list: " << errorList;
        qDebug() << "Error list size = " << errorList.size();
        qDebug() << "First error = " << errorList.first();
        qDebug() << "Last error = " << errorList.last();
    }
    */
}

/*!
 * \brief Network::testing
 * \param data
 * \param desiredResult
 */
void Network::testing(const Data &data, const Result & desiredResult)
{
    Q_ASSERT( data.size() == desiredResult.size() );
    // Process all data
    {
        obtainedTestingResult.resize( data.size() );
        auto obtainedResultIt = obtainedTestingResult.begin();
        std::for_each( data.constBegin(), data.constEnd(), [&, this] ( Data::const_reference dataSample ) {
            const auto result = ( processInNetwork( dataSample ) ).last();
            (*obtainedResultIt).getData() = result;
            ++ obtainedResultIt;
        } );
    }
    // TODO remove debug output
    /*
    {
        qDebug() << "obtainedTestingResult = ";
        std::for_each( obtainedTestingResult.constBegin(), obtainedTestingResult.constEnd(), [] ( Result::const_reference result ) {
            qDebug() << result.getData();
        } );
    }
    */
    // TODO calculate difference between result obtained from the network and desired result
    {
        Q_ASSERT( obtainedTestingResult.size() == desiredResult.size() );
        testingError.resize( obtainedTestingResult.size() );
        std::transform ( obtainedTestingResult.constBegin(), obtainedTestingResult.constEnd(), desiredResult.constBegin(), testingError.begin(),
                        []( Result::const_reference obtained, Result::const_reference desired ) {
            Q_ASSERT( obtained.getData().size() == desired.getData().size() );
            qreal diff = 0.0;
            auto obtainedDataIt = obtained.getData().constBegin();
            std::for_each ( desired.getData().constBegin(), desired.getData().constEnd(), [&]( const Result::value_type::value_type value ) {
                diff += std::sqrt( std::pow( (*obtainedDataIt) - value, 2 ) );
                ++ obtainedDataIt;
            } );
            return diff;
        } );
    }
    // TODO remove debug output
    {
        qDebug() << "\nMax testing error" << ( *(std::max_element( testingError.constBegin(), testingError.constEnd() )) );
        qDebug() << "\nMin testing error" << ( *(std::min_element( testingError.constBegin(), testingError.constEnd() )) );
        qDebug() << "\nAverage error = " << ( std::accumulate( testingError.constBegin(), testingError.constEnd(), 0.0 ) / testingError.size() );
    }

}

/*!
 * \brief Network::getLayers
 * \return
 */
QVector<Layer> &Network::getLayers()
{
    return layers;
}

/*!
 * \brief Network::getLayers
 * \return
 */
const QVector<Layer> &Network::getLayers() const
{
    return layers;
}

/*!
 * \brief Network::getNetworkError
 * \return
 */
const QVector<qreal> Network::getNetworkError() const
{
   // const QVector< qreal > vector( errorList.toVector() );
    return testingError;
}

/*!
 * \brief Network::stop
 */
void Network::stop()
{
    stopFlag = true;
}

/*!
 * \brief Network::setTestingResult
 * \param value
 */
void Network::setTestingResult(const QVector<QVector<qreal> > &value)
{
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
void Network::setTestingData(const QVector<QVector<qreal> > &value)
{
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
void Network::setTrainingData(const QVector<QVector<qreal> > &value)
{
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
void Network::setTrainigResult(const QVector < QVector< qreal > > &value)
{
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
void Network::setLayersDescription(const QVector<LayerDescription> &value)
{
    layersDescription = value;
}

/*!
 * \brief Network::getObtainedTestingError
 * \return
 */
QVector<qreal> Network::getObtainedTestingError() const
{
    return testingError;
}

/*!
 * \brief Network::getObtainedTestingResult
 * \return
 */
Result Network::getObtainedTestingResult() const
{
    return obtainedTestingResult;
}


/*!
 * \brief Network::getBeta
 * \return
 */
qreal Network::getBeta() const
{
    return beta;
}

/*!
 * \brief Network::setBeta
 * \param value
 */
void Network::setBeta(const qreal value)
{
    beta = value;
}

/*!
 * \brief Network::getAlpha
 * \return
 */
qreal Network::getAlpha() const
{
    return alpha;
}

/*!
 * \brief Network::setAlpha
 * \param value
 */
void Network::setAlpha(const qreal value)
{
    alpha = value;
}

/*!
 * \brief Network::getAccuracy
 * \return
 */
qreal Network::getAccuracy() const
{
    return accuracy;
}

/*!
 * \brief Network::setAccuracy
 * \param value
 */
void Network::setAccuracy(const qreal value)
{
    accuracy = value;
    Q_ASSERT( accuracy < 1.0 );
}

/*!
 * \brief Network::getMaxNumberOfEpoch
 * \return
 */
quint32 Network::getMaxNumberOfEpoch() const
{
    return maxNumberOfEpoch;
}

/*!
 * \brief Network::setMaxNumberOfEpoch
 * \param value
 */
void Network::setMaxNumberOfEpoch(const quint32 value)
{
    maxNumberOfEpoch = value;
}
/*
void Network::training(const Data &dataSet, const Result &desiredResult) {
    Q_ASSERT(dataSet.size() == desiredResult.size());

    quint32 epochCounter = 0;
    qreal achievedAccuracy = 1.0;

    errorList.clear(); // List contains all errors

    stopFlag = false;

    while ( true ) {

        if ( epochCounter >= maxNumberOfEpoch ) break;
        if ( achievedAccuracy <= accuracy ) break;
        if ( stopFlag ) break;

        // Process all example and receive network output
        const auto obtainedResultPair = process( dataSet );
        const auto obtainedResult = obtainedResultPair.first;
        const auto intermidVector = obtainedResultPair.second;
        Q_ASSERT( obtainedResult.size() == desiredResult.size() );
        // Calculate error of the network
        {
            qreal error = 0.0;
            // Iterate over all data samples
            // NOTE Oleksandr Halushko implement in parallel
            for ( auto desIt = desiredResult.constBegin(), obtIt = obtainedResult.begin(); desIt != desiredResult.constEnd(); ++ desIt, ++ obtIt ) {
                Q_ASSERT( (*desIt).getData().size() == (*obtIt).getData().size() );
                // Iterate over all part of results
                for ( auto desDataIt = (*desIt).getData().constBegin(), obtDataIt = (*obtIt).getData().constBegin(); desDataIt != (*desIt).getData().constEnd(); ++ desDataIt, ++ obtDataIt ) {
                    error += std::pow( (*desDataIt) - (*obtDataIt), 2 );
                }
            }
            error /= 2.0;
            // Save error
            errorList.append( error );
        }

        // Apply back propagation
        {
            // For each neuron of the last layer
            QVector < qreal > deltaVector( layers.last().getNeurons().size() ); // Vector of delta corrective
            auto deltaIt = deltaVector.begin();
            for ( auto lastLayerNeuronIt = layers.last().getNeurons().begin(); lastLayerNeuronIt != layers.last().getNeurons().end(); ++ lastLayerNeuronIt, ++ deltaIt ) {
                qreal delta = 0.0;
                // Calculate error on each output and multipy it by input
                auto dataIt = dataSet.constBegin();
                for ( auto desIt = desiredResult.constBegin(), obtIt = obtainedResult.begin(); desIt != desiredResult.constEnd(); ++ desIt, ++ obtIt, ++ dataIt ) {
                    Q_ASSERT( (*desIt).getData().size() == (*obtIt).getData().size() );
                    qreal error = 0.0;
                    // Calculate error for each part of data sample
                    for ( auto desDataIt = (*desIt).getData().constBegin(), obtDataIt = (*obtIt).getData().constBegin(); desDataIt != (*desIt).getData().constEnd(); ++ desDataIt, ++ obtDataIt ) {
                        error += (*desDataIt) - (*obtDataIt);
                    }
                    // Multiply on each part of data
                    for ( auto sampleDataIt = (*dataIt).getData().constBegin(); sampleDataIt != (*dataIt).getData().constEnd(); ++ sampleDataIt ) {
                        error *= derivLinLambda(*sampleDataIt);
                    }
                    error /= (*dataIt).getData().size();
                    delta += error;
                }
                // Update weights
                std::for_each( (*lastLayerNeuronIt).getWeights().begin(), (*lastLayerNeuronIt).getWeights().end(), [&]( qreal & weight ) {
                    weight += alpha * delta;
                } );
                (*deltaIt) = delta; // Store delta
            }

            // For next layers
            auto intermidIt = intermidVector.constEnd();
            auto layerIt = layers.end();
            for ( std::advance( layerIt, -2 ), std::advance( intermidIt, -2 ); std::distance( layerIt, layers.begin() ) != 0; -- layerIt, --intermidIt ) {
                // Sum deltas and weigths for next layer
                qreal sum = 0.0;
                {
                    // Get next layer
                    auto nextLayer = layerIt;
                    std::advance( nextLayer, 1 );

                    Q_ASSERT( deltaVector.size() == (*nextLayer).getNeurons().size() );

                    // Multiply all weights on correcive delta
                    auto deltaIt = deltaVector.constBegin();
                    for ( auto neuronIt = (*nextLayer).getNeurons().constBegin(); neuronIt != (*nextLayer).getNeurons().constEnd(); ++ neuronIt, ++ deltaIt ) {
                        std::for_each( (*neuronIt).getWeights().constBegin(), (*neuronIt).getWeights().constEnd(), [&]( const qreal & weight ) {
                            sum += weight * (*deltaIt);
                        } );
                    }
                }
                Q_ASSERT( (*layerIt).getNeurons().size() == (*intermidIt).size() );
                // For each neuron calculate weight update
                {
                    auto intermidNeuronResultIt = (*intermidIt).begin();
                    decltype( deltaVector ) currentLayerDeltaVector( (*intermidIt).size() );
                    auto layerDeltaIt = currentLayerDeltaVector.begin();
                    for ( auto neuronIt = (*layerIt).getNeurons().begin(); neuronIt != (*layerIt).getNeurons().end(); ++neuronIt, ++ intermidNeuronResultIt, ++ layerDeltaIt ) {
                        (*layerDeltaIt) = derivTanhLambda( *intermidNeuronResultIt, beta ) * sum;
                        // Update weights
                        std::for_each( (*neuronIt).getWeights().begin(), (*neuronIt).getWeights().end(), [&]( qreal & weight ) {
                            weight += alpha * (*layerDeltaIt);
                        } );
                    }
                    deltaVector = currentLayerDeltaVector;
                }
            }
        }
        achievedAccuracy = errorList.last(); // Get last error
        ++ epochCounter;
    }
    // TODO Oleksandr Halushko remove debug output
    qDebug() << "Error list size = " << errorList.size();
    qDebug() << "Firt error = " << errorList.first();
    qDebug() << "Last error = " << errorList.last();
}*/

#ifdef TEST_MODE
void NetworkTest::ProcessTest()
{

    constexpr quint32 numberOfDataSamples = 3;
    constexpr quint32 numberOfInputs = 2;
    constexpr quint32 numberOfOutputs = 1;

    Network & network = Network::getInstance();
    QVector< LayerDescription > layersDesciption;
    //    layersDesciption.append(LayerDescription(numberOfOutputs, numberOfInputs));
    layersDesciption.append( LayerDescription( 2, numberOfInputs ) ); // Input
    layersDesciption.append( LayerDescription( 3, 2 ) ); // Hidden
    layersDesciption.append( LayerDescription( 5, 3 ) ); // Hidden
    layersDesciption.append( LayerDescription( numberOfOutputs, 5 ) ); // Ouput

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
    network.setAlpha( 1e-4 );
    network.setBeta( 10.0 );
    network.setMaxNumberOfEpoch( 1000 );

    network.setLayersDescription( layersDesciption );
    network.run();
}
#endif
