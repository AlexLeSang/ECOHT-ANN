#include "Network.hpp"

#include "Layer.hpp"

#include <QDebug>

Network::Network() {}

Network &Network::getInstance() {
    static Network instance;
    return instance;
}

void Network::initNetwork(const QVector< LayerDescription > &layersDescription, const qreal accuracy, const quint32 maxNumberOfEpoch, const qreal trainingCoefficient, const qreal beta) {
    Q_ASSERT(layersDescription.size() != 0);
    //    Q_ASSERT(layersDescription.size() <= 3);
    Q_ASSERT(layersDescription.first().second == numberOfInputs); // "Number of inputs to the network"
    Q_ASSERT(layersDescription.last().first == numberOfOutputs); // "Number of outputs of the network")

    layers = QVector < Layer >( layersDescription.size() );
    auto descriptionIterator = layersDescription.begin();
    qint32 layerCount = 0;
    std::for_each( layers.begin(), layers.end(), [&](Layer & layer) {
        layer.initLayer( (*descriptionIterator).first, (*descriptionIterator).second, beta, layerCount == (layers.size() - 1) );
        ++descriptionIterator;
        ++layerCount;
    } );

    this->accuracy = accuracy;
    this->maxNumberOfEpoch = maxNumberOfEpoch;
    this->trainigCoefficien = trainingCoefficient;
    this->beta = beta;
}

void Network::training(const Data &dataSet, const Result &desiredResult) {
    Q_ASSERT(dataSet.size() == desiredResult.size());

    quint32 epochCounter = 0;
    qreal achievedAccuracy = 1.0;

    QList< qreal > errorList;

    while ( true ) {
        if ( epochCounter >= maxNumberOfEpoch ) break;
        if ( achievedAccuracy <= accuracy ) break;

        // Process all example and receive network output
        const auto obtainedResultPair = process( dataSet );
        const auto obtainedResult = obtainedResultPair.first;
        const auto intermidVector = obtainedResultPair.second;
        Q_ASSERT( obtainedResult.size() == desiredResult.size() );
        {
            qreal error = 0.0;
            // NOTE implement in parallel
            for ( auto desIt = desiredResult.constBegin(), obtIt = obtainedResult.begin(); desIt != desiredResult.constEnd(); ++ desIt, ++ obtIt ) {
                const auto & desData = (*desIt).getData();
                const auto & obtData = (*obtIt).getData();
                Q_ASSERT( desData.size() == obtData.size() );
                for ( auto desDataIt = desData.constBegin(), obtDataIt = obtData.constBegin(); desDataIt != desData.constEnd(); ++ desDataIt, ++ obtDataIt ) {
                    error += std::pow( (*desDataIt) - (*obtDataIt), 2 );
                }
            }
            error /= 2.0;
            errorList.append( error );
//            qDebug() << "Network error at " << epochCounter << " epoch = " << error; // TODO remove debug output
        }

        // Apply back propagation
        {
            // For each neuron of the last layer
            QVector < qreal > deltaVector( layers.last().getNeurons().size() );
            auto deltaIt = deltaVector.begin();
            for ( auto lastLayerNeuronIt = layers.last().getNeurons().begin(); lastLayerNeuronIt != layers.last().getNeurons().end(); ++ lastLayerNeuronIt, ++ deltaIt ) {
                qreal delta = 0.0;
                // Calculate error on each output and multipy it by input
                auto dataIt = dataSet.constBegin();
                for ( auto desIt = desiredResult.constBegin(), obtIt = obtainedResult.begin(); desIt != desiredResult.constEnd(); ++ desIt, ++ obtIt, ++ dataIt ) {
                    const auto & desData = (*desIt).getData();
                    const auto & obtData = (*obtIt).getData();
                    Q_ASSERT( desData.size() == obtData.size() );
                    qreal error = 0.0;
                    for ( auto desDataIt = desData.constBegin(), obtDataIt = obtData.constBegin(); desDataIt != desData.constEnd(); ++ desDataIt, ++ obtDataIt ) {
                        error += (*desDataIt) - (*obtDataIt);
                    }
                    // Multiply on each part of data
                    for ( auto sampleDataIt = (*dataIt).getData().constBegin(); sampleDataIt != (*dataIt).getData().constEnd(); ++ sampleDataIt ) {
                        const auto dataSample = *sampleDataIt;
                        error *= derivLinLambda(dataSample);
                    }
                    error /= (*dataIt).getData().size();
                    delta += error;
                }
                // Update weights
                std::for_each( (*lastLayerNeuronIt).getWeights().begin(), (*lastLayerNeuronIt).getWeights().end(), [&]( qreal & weight ) {
                    weight += trainigCoefficien * delta;
                } );
                (*deltaIt) = delta; // Store delta
            }
//            qDebug() << "deltaVector = " << deltaVector; // TODO remove debug output

            // For next layers
            auto intermidIt = intermidVector.constEnd();
            std::advance( intermidIt, -2 );
            auto layerIt = layers.end();
            for ( std::advance( layerIt, -2 ); std::distance( layerIt, layers.begin() ) != 0; -- layerIt, --intermidIt ) {
                // Sum deltas and weigths for next layer
                qreal sum = 0.0;
                {
                    auto nextLayer = layerIt;
                    std::advance( nextLayer, 1 );

                    Q_ASSERT( deltaVector.size() == (*nextLayer).getNeurons().size() );
                    auto deltaIt = deltaVector.constBegin();
                    for ( auto neuronIt = (*nextLayer).getNeurons().constBegin(); neuronIt != (*nextLayer).getNeurons().constEnd(); ++ neuronIt, ++ deltaIt ) {
                        std::for_each( (*neuronIt).getWeights().constBegin(), (*neuronIt).getWeights().constEnd(), [&]( const qreal & weight ) {
                            sum += weight * (*deltaIt);
                        } );
                    }
//                    qDebug() << "Sum = " << sum; // TODO remove debug output
                }
                Q_ASSERT( (*layerIt).getNeurons().size() == (*intermidIt).size() );
                auto intermidNeuronResultIt = (*intermidIt).begin();
                QVector < qreal > layerDelta( (*intermidIt).size() );
                auto layerDeltaIt = layerDelta.begin();
                for ( auto neuronIt = (*layerIt).getNeurons().begin(); neuronIt != (*layerIt).getNeurons().end(); ++neuronIt, ++ intermidNeuronResultIt, ++ layerDeltaIt ) {
                    (*layerDeltaIt) = derivTanhLambda( *intermidNeuronResultIt, beta ) * sum;
                    std::for_each( (*neuronIt).getWeights().begin(), (*neuronIt).getWeights().end(), [&]( qreal & weight ) {
                        weight += trainigCoefficien * (*layerDeltaIt);
                    } );
                }
                deltaVector = layerDelta;
//                qDebug() << "deltaVector = " << deltaVector; // TODO remove debug output
            }
        }
        achievedAccuracy = errorList.last();
        ++ epochCounter;
    }
    // TODO remove debug output
    qDebug() << "Error list size = " << errorList.size();
    qDebug() << "Firt error = " << errorList.first();
    qDebug() << "Last error = " << errorList.last();
}

QVector<Layer> &Network::getLayers() {
    return layers;
}

const QVector<Layer> &Network::getLayers() const {
    return layers;
}

qreal Network::getBeta() const {
    return beta;
}

QPair < Result, QVector < QVector< qreal > > >  Network::process(const Data &data) {
    Q_ASSERT(data.size() != 0);

    Result result( data.size() );
    QVector < QVector< qreal > > intermidResultVector( layers.size() );
    {
        auto layerIt = layers.constBegin();
        for ( auto it = intermidResultVector.begin(); it != intermidResultVector.end(); ++ it, ++ layerIt ) {
            (*it).resize( (*layerIt).getNeurons().size() );
        }
    }

    auto resultIt = result.begin();
    for ( auto dataIt = data.constBegin(); dataIt != data.constEnd(); ++ dataIt, ++resultIt ) {
        const auto & currentSample = (*dataIt);
        QVector < qreal > tempResult = currentSample.getData();
        auto intermidIt = intermidResultVector.begin();
        for ( auto layerIt = layers.constBegin(); layerIt != layers.constEnd(); ++ layerIt, ++ intermidIt ) {
            const auto intermidResult = (*layerIt).process( tempResult );
            tempResult = intermidResult;
            // Sum intermid result and intermidResultVector
            auto itRes = intermidResult.begin();
            for ( auto it = (*intermidIt).begin(); it != (*intermidIt).end(); ++ it, ++ itRes ) {
                (*it) += (*itRes)/data.size();
            }
        }
        Result::value_type resultSample( tempResult );
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
            val =  10.0;
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
