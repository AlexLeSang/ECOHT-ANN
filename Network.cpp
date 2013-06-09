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
        std::for_each( layers.constBegin(), layers.constEnd(), [&]( const Layer & layer ) {
            auto result = layer.process( intermidResult.last() );
            intermidResult.append( result );
        } );
        intermidResult.removeFirst();
    }
    return intermidResult.toVector();
}

qreal Network::calculateNetworkError(const QVector< qreal > & desiredResult, const QVector< qreal > & obtainedResult) const {
    Q_ASSERT( desiredResult.size() == obtainedResult.size() );
    qreal networkError = 0.0;
    auto obtainedIt = obtainedResult.constBegin();
    std::for_each( desiredResult.constBegin(), desiredResult.constEnd(), [&]( const qreal & desired ) {
        const qreal obtained = (*obtainedIt);
        ++ obtainedIt;
        networkError += std::pow( (desired - obtained), 2.0 );
    } );
    return networkError;
}

const QVector< qreal > Network::calculateOutputGradient(const QVector<qreal> &desiredResult, const QVector<qreal> &obtainedResult) const {
    Q_ASSERT( desiredResult.size() == obtainedResult.size() );
    QVector< qreal > gradient( desiredResult.size() );
    auto obtainedIt = obtainedResult.constBegin();
    auto gradientIt = gradient.begin();
    std::for_each( desiredResult.constBegin(), desiredResult.constEnd(), [&]( const qreal & desired ) {
        const qreal & obtained = (*obtainedIt);
        //        qDebug() << "obtained : " << obtained;
        //        qDebug() << "desired : " << desired;

        //        const auto grad = desired * ( 1 - desired ) * ( obtained - desired );

        //        outputValue * ( 1 - outputValue ) * ( desiredValue - outputValue );
        auto grad = obtained * ( 1 - obtained ) * ( desired - obtained );
        //        qDebug() << "grad: " << grad;

        (*gradientIt) = grad;
        ++ obtainedIt;
        ++ gradientIt;
    } );
    return gradient;
}

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

        QVector< QVector < QVector < QVector< qreal > > > > delta( desiredResult.size() ); // Result_size( Layers_size( Neurons_size ( Weights_size ) ) )
        std::for_each( delta.begin(), delta.end(), [&]( QVector < QVector < QVector < qreal > > > & layersDelta ) {
            layersDelta.resize( layers.size() );
            auto index = 0;
            std::for_each( layersDelta.begin(), layersDelta.end(), [&]( QVector < QVector < qreal > > & neuronDelta ) {
                neuronDelta.resize( layers.at( index ).getNeurons().size() );
                auto neuronIndex = 0;
                std::for_each( neuronDelta.begin(), neuronDelta.end(), [&] ( QVector < qreal > & weightDelta ) {
                    weightDelta.resize( layers.at( index ).getNeurons()[ neuronIndex ].getWeights().size() );
                    ++neuronIndex;
                } );
                ++ index;
            } );
        } );

        // For each part of data
        auto desiredResultIt = desiredResult.begin();
        auto deltaIt = delta.begin();
        std::for_each( dataSet.constBegin(), dataSet.constEnd(), [&]( Data::const_reference dataSample ) {
            // Process data through the network
            //            qDebug() << "Process in network";
            //            qDebug() << "==========================================================================";
            const QVector < QVector < qreal > > intermidResult = processInNetwork( dataSample );
            for ( auto layerIndex = 0; layerIndex < intermidResult.size(); ++ layerIndex ) {
                //                qDebug() << "layerIndex: " << layerIndex;
                //                qDebug() << intermidResult.at( layerIndex );
            }
            //            qDebug() << "==========================================================================";

            // Calculate network error for this data and add it to error in current epoch
            const QVector< qreal > & obtainedResult = intermidResult.last(); // Get network result
            const QVector< qreal > & currentDesiredResult = (*desiredResultIt).getData();

            //            qDebug() << "Calculate network error";
            //            qDebug() << "==========================================================================";
            networkError = calculateNetworkError( currentDesiredResult, obtainedResult );
            //            qDebug() << "networkError: " << networkError;
            //            qDebug() << "==========================================================================";


            //            qDebug() << "Calculate output gradient";
            //            qDebug() << "==========================================================================";
            const QVector< qreal > outputGradient = calculateOutputGradient( currentDesiredResult, obtainedResult );
            //            qDebug() << "outputGradient: " << outputGradient;
            //            qDebug() << "==========================================================================";


            QVector < QVector < QVector < qreal > > > & deltaForCurrentSample = (*deltaIt);

            //modify deltas between hidden and output layers
            //            qDebug() << "Last layer delta";
            //            qDebug() << "==========================================================================";
            {
                QVector < QVector < qreal > > & lastLayerDelta = deltaForCurrentSample.last();
                QVector < Neuron > & lastLayer = layers.last().getNeurons();
                Q_ASSERT( lastLayerDelta.size() == lastLayer.size() );

                // For each neuron in the last layer

                for ( auto lastLayerNeuronIndex = 0; lastLayerNeuronIndex < lastLayer.size(); ++ lastLayerNeuronIndex ) {

                    //                    qDebug() << "lastLayerNeuronIndex: " << lastLayerNeuronIndex;

                    QVector < qreal > & lastLayerDeltaNeurons = lastLayerDelta[ lastLayerNeuronIndex ];
                    QVector< qreal > & currentNeuronWeights = lastLayer[ lastLayerNeuronIndex ].getWeights();
                    const qreal & grad = outputGradient.at( lastLayerNeuronIndex );
                    Q_ASSERT( lastLayerDeltaNeurons.size() == currentNeuronWeights.size() );
                    // For each weight in the current neuron
                    for ( auto weightIndex = 0; weightIndex < currentNeuronWeights.size(); ++ weightIndex ) {

                        //                        qDebug() << "weightIndex: " << weightIndex;

                        //                        qDebug() << "currentNeuronWeights[ weightIndex ]: " << currentNeuronWeights[ weightIndex ];

                        const auto lastLayerDeltaNeuronsWeightValue = alpha * currentNeuronWeights[ weightIndex ] * grad + beta * lastLayerDeltaNeurons[ weightIndex ];

                        //                        qDebug() << "lastLayerDeltaNeuronsWeightValue: " << lastLayerDeltaNeuronsWeightValue;

                        lastLayerDeltaNeurons[ weightIndex ] = lastLayerDeltaNeuronsWeightValue;
                    }
                }

                //                qDebug() << "lastLayerDelta: " << lastLayerDelta;

            }
            //            qDebug() << "==========================================================================";

            // For each layer from the first to before last
            //            qDebug() << "Intermid layer delta";
            //            qDebug() << "==========================================================================";
            {
                for ( auto layerIndex = layers.size() - 2; layerIndex >= 0; -- layerIndex ) {

                    //                    qDebug() << "\nlayerIndex: " << layerIndex;

                    QVector < QVector < qreal > > & currentLayerDelta = deltaForCurrentSample[ layerIndex ];
                    QVector < Neuron > & currentLayer = layers[ layerIndex ].getNeurons();

                    const QVector < QVector < qreal > > & nextLayerDelta = deltaForCurrentSample[ layerIndex + 1 ];
                    const QVector < Neuron > & nextLayer = layers[ layerIndex + 1 ].getNeurons();

                    Q_ASSERT( currentLayerDelta.size() == currentLayer.size() );
                    // For each neuron in current layer
                    for ( auto neuronIndex = 0; neuronIndex < currentLayer.size(); ++ neuronIndex ) {

                        //                        qDebug() << "neuronIndex: " << neuronIndex;

                        QVector < qreal > & currentNeuronWeights = currentLayer[ neuronIndex ].getWeights();
                        QVector < qreal > & currentLayerDeltaNeurons = currentLayerDelta[ neuronIndex ];
                        qreal grad = 0.0;
                        {
                            // For each neuron with from the next layer
                            for ( auto nextLayerIndex = 0; nextLayerIndex < nextLayer.size(); ++ nextLayerIndex ) {

                                //                                qDebug() << "nextLayerIndex: " << nextLayerIndex;

                                // Get this neurons delta
                                const QVector < qreal > & nextLayerDeltaNeuron = nextLayerDelta[ nextLayerIndex ];
                                // Multiply it by it's delta
                                const qreal & connectedDelta = nextLayerDeltaNeuron[ neuronIndex ];
                                // Get weight connected with this neuron
                                const qreal & connectedWeight = nextLayer[ nextLayerIndex ].getWeights()[ neuronIndex ];
                                // Sum it's and use as a gradient

                                //                                qDebug() << "connectedDelta: " << connectedDelta;
                                //                                qDebug() << "connectedWeight: " << connectedWeight;

                                grad += connectedDelta * connectedWeight;
                            }
                        }

                        //                        qDebug() << "grad: " << grad;

                        Q_ASSERT( currentNeuronWeights.size() == currentLayerDeltaNeurons.size() );
                        // For each weight in current neuron
                        for ( auto weightIndex = 0; weightIndex < currentLayerDeltaNeurons.size(); ++ weightIndex ) {

                            //                            qDebug() << "weightIndex: " << weightIndex;

                            const auto deltaValue = alpha * currentNeuronWeights[ weightIndex ] * grad + beta * currentLayerDeltaNeurons[ weightIndex ];
                            //                            qDebug() << "deltaValue: " << deltaValue;

                            currentLayerDeltaNeurons[ weightIndex ] = deltaValue;
                        }
                    }
                }
            }
            //            qDebug() << "==========================================================================";

            //            qDebug() << "Update weights";
            //            qDebug() << "==========================================================================";
            // Update weights
            {
                for ( auto layerIndex = 0; layerIndex < layers.size(); ++ layerIndex ) {

                    //                    qDebug() << "\nlayerIndex: " << layerIndex;

                    const QVector < QVector < qreal > > & currentLayerDeltas = deltaForCurrentSample[ layerIndex ];
                    QVector < Neuron > & currentLayerNeurons = layers[ layerIndex ].getNeurons();
                    Q_ASSERT( currentLayerDeltas.size() == currentLayerNeurons.size() );

                    for ( auto neuronIndex = 0; neuronIndex < currentLayerNeurons.size(); ++ neuronIndex ) {

                        //                        qDebug() << "neuronIndex: " << neuronIndex ;

                        const QVector < qreal > & currentNeuronDeltas = currentLayerDeltas[ neuronIndex ];
                        QVector< qreal > & currentNeuronWeights = currentLayerNeurons[ neuronIndex ].getWeights();
                        Q_ASSERT( currentNeuronWeights.size() == currentNeuronDeltas.size() );
                        for ( auto weightIndex = 0; weightIndex < currentNeuronWeights.size(); ++ weightIndex ) {

                            //                            qDebug() << "weightIndex: " << weightIndex;

                            const qreal & currentDelta = currentNeuronDeltas[ weightIndex ];
                            qreal & currentWeight = currentNeuronWeights[ weightIndex ];


                            //                            qDebug() << "currentDelta: " << currentDelta;
                            //                            qDebug() << "currentWeight: " << currentWeight;

                            currentWeight += currentDelta;

                            //                            qDebug() << "Updated currentWeight: " << currentWeight;
                        }
                    }
                }
            }
            //            qDebug() << "==========================================================================";

            // Go to the next part of trainign data
            ++ deltaIt;
            ++ desiredResultIt;
        } );

//        qDebug() << "Epoch# " << epochCounter << " error:" << networkError << '\n';

        if ( epochCounter > 0 ) {
            if ( errorList.last() < networkError ) break;
        }

        errorList.append( networkError ); // Save current error
        achievedAccuracy = networkError; // Get last error
        ++ epochCounter;
    }

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
        std::transform ( obtainedTestingResult.constBegin(), obtainedTestingResult.constEnd(), desiredResult.constBegin(), testingError.begin(), []( Result::const_reference obtained, Result::const_reference desired ) {
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
    //     TODO remove debug output
    {
        qDebug() << "Max testing error" << ( *(std::max_element( testingError.constBegin(), testingError.constEnd() )) );
        qDebug() << "Min testing error" << ( *(std::min_element( testingError.constBegin(), testingError.constEnd() )) );
        qDebug() << "Average error = " << ( std::accumulate( testingError.constBegin(), testingError.constEnd(), 0.0 ) / testingError.size() );
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

#ifdef TEST_MODE
void NetworkTest::ProcessTest()
{

    constexpr quint32 numberOfDataSamples = 5;
    constexpr quint32 numberOfInputs = 2;
    constexpr quint32 numberOfOutputs = 1;

    Network & network = Network::getInstance();
    QVector< LayerDescription > layersDesciption;
    //    layersDesciption.append(LayerDescription(numberOfOutputs, numberOfInputs));
    layersDesciption.append( LayerDescription( 6, numberOfInputs ) ); // Input
    layersDesciption.append( LayerDescription( 7, 6 ) ); // Hidden
    layersDesciption.append( LayerDescription( 4, 7 ) ); // Hidden
    layersDesciption.append( LayerDescription( numberOfOutputs, 4 ) ); // Ouput

    QVector < QVector < qreal > > data( numberOfDataSamples );
    auto index = 0;
    auto step = ( 1.0 / (numberOfDataSamples * 2) );
    std::for_each( data.begin(), data.end(), [&]( QVector< qreal > & sample ) {
        sample.resize( numberOfInputs );
        std::for_each( sample.begin(), sample.end(), [&] ( qreal & val ) {
            val = (index++)*step;
        } );
    } );


    index = 0;
    step = ( 1.0 / (numberOfDataSamples ) );
    QVector < QVector < qreal > > result( data.size() );
    std::for_each( result.begin(), result.end(), [&]( QVector< qreal > & sample ) {
        sample.resize( numberOfOutputs );
        std::for_each( sample.begin(), sample.end(), [&] ( qreal & val ) {
              val = (index++)*step;
//            val = 0.6;
        } );
    } );

    //    std::random_shuffle( data.begin(), data.end() );
    //    std::random_shuffle( result.begin(), result.end() );

    //        qDebug() << "data: " << data;
            qDebug() << "result: " << result;
    //    return;


    network.setTestingData( data );
    network.setTestingResult( result );

    network.setTrainingData( data );
    network.setTrainigResult( result );

    network.setAccuracy( 1e-16 );


    network.setAlpha( 2e-3 );
    network.setBeta( 1e-3 );
    network.setMaxNumberOfEpoch( 300 );

    network.setLayersDescription( layersDesciption );
    network.run();
    auto testringRes = network.getObtainedTestingResult();
    qDebug() << "testringRes: ";
    std::for_each( testringRes.constBegin(), testringRes.constEnd(), [] ( const Result::value_type & val ) {
        qDebug() << val.getData();
    } );
}


void NetworkTest::test()
{
    ProcessTest();
}

#endif
