#include "Network.hpp"

#include "Layer.hpp"

#include <QDebug>

/*!
 * \brief Network::Network
 */
Network::Network() {}

/*!
 * \brief Network::getInstance
 * \return
 */
Network &Network::getInstance() {
    static Network instance;
    return instance;
}

/*!
 * \brief Network::initNetwork
 * \param layersDescription
 * \param accuracy
 * \param maxNumberOfEpoch
 * \param alpha
 * \param beta
 */
void Network::initNetwork(const QVector< LayerDescription > &layersDescription, const qreal accuracy, const quint32 maxNumberOfEpoch, const qreal alpha, const qreal beta) {
    Q_ASSERT(layersDescription.size() != 0);
    Q_ASSERT(layersDescription.first().second == numberOfInputs); // "Number of inputs to the network"
    Q_ASSERT(layersDescription.last().first == numberOfOutputs); // "Number of outputs of the network")

    Q_ASSERT(accuracy < 1.0);
    Q_ASSERT(maxNumberOfEpoch >= 1);
    Q_ASSERT(alpha != 0.0);
    Q_ASSERT(beta != 0.0);

    this->accuracy = accuracy;
    this->maxNumberOfEpoch = maxNumberOfEpoch;
    this->alpha = alpha;
    this->beta = beta;

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
 * \brief Network::training
 * \param dataSet
 * \param desiredResult
 */
void Network::training(const Data &dataSet, const Result &desiredResult) {
    Q_ASSERT(dataSet.size() == desiredResult.size());

    quint32 epochCounter = 0;
    qreal achievedAccuracy = 1.0;

    errorList.clear(); // List contains all errors

    while ( true ) {

        if ( epochCounter >= maxNumberOfEpoch ) break;
        if ( achievedAccuracy <= accuracy ) break;

        // Process all example and receive network output
        const auto obtainedResultPair = process( dataSet );
        const auto obtainedResult = obtainedResultPair.first;
        const auto intermidVector = obtainedResultPair.second;
        Q_ASSERT( obtainedResult.size() == desiredResult.size() );
        // Calculate error of the network
        {
            qreal error = 0.0;
            // Iterate over all data samples
            // NOTE implement in parallel
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
    // TODO remove debug output
    qDebug() << "Error list size = " << errorList.size();
    qDebug() << "Firt error = " << errorList.first();
    qDebug() << "Last error = " << errorList.last();
}

/*!
 * \brief Network::testing
 * \param data
 * \return
 */
const Result Network::testing(const Data &data) {
    const auto result = process( data );
    return result.first;
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
    return errorList.toVector();
}

/*!
 * \brief Network::getBeta
 * \return
 */
qreal Network::getBeta() const {
    return beta;
}

/*!
 * \brief Network::process
 * \param data
 * \return
 */
QPair < Result, QVector < QVector< qreal > > >  Network::process(const Data &data) {
    Q_ASSERT(data.size() != 0);

    Result result( data.size() );
    QVector < QVector< qreal > > intermidResultVector( layers.size() );
    // Initialization
    {
        auto layerIt = layers.constBegin();
        for ( auto it = intermidResultVector.begin(); it != intermidResultVector.end(); ++ it, ++ layerIt ) {
            (*it).resize( (*layerIt).getNeurons().size() );
        }
    }

    auto resultIt = result.begin();
    QVector < qreal > prevIterResult;
    for ( auto dataIt = data.constBegin(); dataIt != data.constEnd(); ++ dataIt, ++resultIt ) {
        prevIterResult = (*dataIt).getData(); // Initialization on first iteration

        // Process data through all layers
        auto intermidIt = intermidResultVector.begin();
        for ( auto layerIt = layers.constBegin(); layerIt != layers.constEnd(); ++ layerIt, ++ intermidIt ) {
            // Process data through layers
            prevIterResult = (*layerIt).process( prevIterResult );
            {
                // Sum intermid result and intermidResultVector
                auto itRes = prevIterResult.begin();
                for ( auto it = (*intermidIt).begin(); it != (*intermidIt).end(); ++ it, ++ itRes ) {
                    (*it) += (*itRes)/data.size();
                }
            }
        }

        // Create new result sample
        Result::value_type resultSample( prevIterResult );
        // Save result sample
        (*resultIt) = resultSample;
    }

    return QPair < Result, QVector < QVector< qreal > > >( result, intermidResultVector );
}

#ifdef TEST_MODE
void NetworkTest::ProcessTest()  {
    constexpr quint32 numberOfDataSamples = 3;
    constexpr quint32 numberOfInputs = 2;
    constexpr quint32 numberOfOutputs = 1;

    Network & network = Network::getInstance();
    QVector< LayerDescription > layersDesciption;
    /*
    layersDesciption.append( LayerDescription( numberOfOutputs, numberOfInputs ) );
    network.initNetwork( layersDesciption, 1e-5, 1e2, 0.8 );
    */
    //    /*
    layersDesciption.append(LayerDescription(5, numberOfInputs)); // Input
    layersDesciption.append(LayerDescription(10, 5)); // Hidden
    layersDesciption.append(LayerDescription(5, 10)); // Hidden
    layersDesciption.append(LayerDescription(numberOfOutputs, 5)); // Ouput
    network.initNetwork( layersDesciption, 1e-16, 1e5, 1e-4 );
    //    */

    Data data( numberOfDataSamples );
    std::for_each( data.begin(), data.end(), []( Data::reference sample ) {
        sample.getData().resize( numberOfInputs );
        std::for_each( sample.getData().begin(), sample.getData().end(), [] ( qreal & val ) {
            val = 2.0;
        } );
    } );

    /*
    std::for_each( data.constBegin(), data.constEnd(), []( Data::const_reference constSample ) {
        static quint32 dataSampleCount = 0;
        qDebug() << "dataSampleCount #" << dataSampleCount++ << constSample.getData();
    } );
    */

    Result result( data.size() );
    std::for_each( result.begin(), result.end(), []( Result::reference sample ) {
        sample.getData().resize( numberOfOutputs );
        std::for_each( sample.getData().begin(), sample.getData().end(), [] ( qreal & val ) {
            val = 10.0;
        } );
    } );

    /*
    qDebug() << "\n";
    std::for_each( result.constBegin(), result.constEnd(), []( Result::const_reference constSample ) {
        static quint32 resultSampleCount = 0;
        qDebug() << "resultSampleCount #" << resultSampleCount++ << constSample.getData();
    } );
    */


    network.training( data, result );
}


void NetworkTest::InitializationTest() {
    Network & network = Network::getInstance();
    QVector< LayerDescription > layersDesciption;
    layersDesciption.append(LayerDescription(2, numberOfInputs));
    layersDesciption.append(LayerDescription(4, 2));
    layersDesciption.append(LayerDescription(numberOfOutputs, 2));
    network.initNetwork(layersDesciption, 1e-5, 1, 0.1);
    const auto & constNetworkRef = network;
    QCOMPARE( constNetworkRef.getLayers().size(), layersDesciption.size() );
    auto descIter = layersDesciption.begin();
    for ( auto iter = constNetworkRef.getLayers().constBegin(); iter != constNetworkRef.getLayers().constEnd(); ++iter ) {

        const auto requiredNeurons = (*descIter).first;
        const auto requiredWeights = (*descIter).second;
        const auto & neurons = (*iter).getNeurons();
        const auto achievedNeurons = static_cast< decltype(requiredNeurons) >( neurons.size() );

        const bool equal = achievedNeurons == requiredNeurons;
        QCOMPARE( true, equal );

        std::for_each( neurons.constBegin(), neurons.constEnd(), [&](const Neuron & neuron) {
            const auto achievedWeights = static_cast< decltype(requiredWeights) >( neuron.getWeights().size() );
            const bool equal = achievedWeights == requiredWeights;
            QCOMPARE( true, equal );
        } );

        ++ descIter;
    }
}

#endif
