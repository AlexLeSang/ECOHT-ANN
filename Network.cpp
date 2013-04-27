#include "Network.hpp"

#include "Layer.hpp"

Network::Network() {}

Network &Network::getInstance() {
    static Network instance;
    return instance;
}

void Network::initNetwork(const QVector< LayerDescription > &layersDescription, const qreal accuracy, const quint32 maxNumberOfEpoch, const qreal trainingCoefficient) {
    Q_ASSERT(layersDescription.size() != 0);
//    Q_ASSERT(layersDescription.size() <= 3);
    Q_ASSERT(layersDescription.first().second == numberOfInputs); // "Number of inputs to the network"
    Q_ASSERT(layersDescription.last().first == numberOfOutputs); // "Number of outputs of the network")

    layers = QVector < Layer >( layersDescription.size() );
    auto descriptionIterator = layersDescription.begin();
    qint32 layerCount = 0;
    std::for_each( layers.begin(), layers.end(), [&](Layer & layer) {
        layer.initLayer( (*descriptionIterator).first, (*descriptionIterator).second, layerCount == (layers.size() - 1) );
        ++descriptionIterator;
        ++layerCount;
    } );

    this->accuracy = accuracy;
    this->maxNumberOfEpoch = maxNumberOfEpoch;
    this->trainigCoefficien = trainingCoefficient;
}

void Network::training(const Data &dataSet, const Result &desiredResult) {
    Q_ASSERT(dataSet.size() == desiredResult.size());

    quint32 epochCounter = 0;
    qreal achievedAccuracy = 1.0;

    QList< qreal > errorList;

    while ( true ){
        if ( epochCounter >= maxNumberOfEpoch ) break;
        if ( achievedAccuracy <= accuracy ) break;

        const auto obtainedResult = process( dataSet );
        Q_ASSERT( obtainedResult.size() == desiredResult.size() );

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
        error /= desiredResult.size();
        errorList.append( error );
        // Process all example and receive network output
        ++ epochCounter;
    }
    // TODO remove debug output
    qDebug() << "errorList = " << errorList;
}

QVector<Layer> &Network::getLayers() {
    return layers;
}

const QVector<Layer> &Network::getLayers() const {
    return layers;
}

Result Network::process(const Data &data) {
    Q_ASSERT(data.size() != 0);

    Result result(data.size());
    auto resultIt = result.begin();
    for ( auto dataIt = data.constBegin(); dataIt != data.constEnd(); ++ dataIt, ++resultIt ) {
        static int sampleCounter = 0;
//        qDebug() << "Sample # " << sampleCounter++; // TODO remove debug output
        Q_UNUSED( sampleCounter );

        const auto & currentSample = (*dataIt);
        QVector < qreal > tempResult = currentSample.getData();
        for ( auto it = layers.constBegin(); it != layers.constEnd(); ++ it ) {
            //            qDebug() << "tempResult = " << tempResult; // TODO remove debug otput
            const auto intermidResult = (*it).process(tempResult);
            //            qDebug() << "intermidResult = " << intermidResult; // TODO remove debug otput
            tempResult = intermidResult;
        }
        Result::value_type resultSample(tempResult);
        (*resultIt) = resultSample;
    }
    /* TODO remove debug output
    qDebug() << "Resutl = ";
    std::for_each( result.constBegin(), result.constEnd(), []( Result::const_reference constSample ) {
        static quint32 resultSampleCount = 0;
        qDebug() << "resultSampleCount #" << resultSampleCount++ << constSample.getData();
    } );
                */

    return result;
}


void NetworkTest::ProcessTest()  {
    constexpr quint32 numberOfDataSamples = 3;
    constexpr quint32 numberOfInputs = 2;
    constexpr quint32 numberOfOutputs = 1;

    Network & network = Network::getInstance();
    QVector< LayerDescription > layersDesciption;
    layersDesciption.append(LayerDescription(3, numberOfInputs)); // Input
    layersDesciption.append(LayerDescription(4, 3)); // Hidden
    layersDesciption.append(LayerDescription(7, 4)); // Hidden
    layersDesciption.append(LayerDescription(4, 7)); // Hidden
    layersDesciption.append(LayerDescription(numberOfOutputs, 4)); // Ouput
    network.initNetwork(layersDesciption, 1e-5, 10, 0.1);

    Data data( numberOfDataSamples );
    std::for_each( data.begin(), data.end(), []( Data::reference sample ) {
        sample.getData().resize( numberOfInputs );
        std::for_each( sample.getData().begin(), sample.getData().end(), randomLambda );
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
        std::for_each( sample.getData().begin(), sample.getData().end(), randomLambda );
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
