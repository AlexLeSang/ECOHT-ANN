#include "Network.hpp"

#include "Layer.hpp"

Network::Network() {}

Network &Network::getInstance() {
    static Network instance;
    return instance;
}

void Network::initNetwork(const QVector< LayerDescription > &layersDescription, const qreal accuracy, const quint32 maxNumberOfEpoch, const qreal trainingCoefficient) {
    Q_ASSERT(layersDescription.size() != 0);
    Q_ASSERT(layersDescription.size() <= 3);
    Q_ASSERT(layersDescription.first().second == numberOfInputs); // "Number of inputs to the network"
    Q_ASSERT(layersDescription.last().first == numberOfOutputs); // "Number of outputs of the network")

    layers = QVector < Layer >( layersDescription.size() );
    auto descriptionIterator = layersDescription.begin();
    std::for_each(layers.begin(), layers.end(), [&](Layer & layer){
        layer.initLayer( (*descriptionIterator).first, (*descriptionIterator).second );
        ++descriptionIterator;
    });

    this->accuracy = accuracy;
    this->maxNumberOfEpoch = maxNumberOfEpoch;
    this->trainigCoefficien = trainingCoefficient;
}

void Network::training(const Data &dataSet, const Result &desiredResult) {
    Q_ASSERT(dataSet.size() == desiredResult.size());
    quint32 epochConter = 0;
    qreal achievedAccuracy = 1.0;
    while ( true ){
        if ( epochConter >= maxNumberOfEpoch ) break;
        if ( achievedAccuracy <= accuracy ) break;

        auto obtainedResult = process( dataSet );
        // Process all example and receive network output
        ++ epochConter;
    }
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
        qDebug() << "Sample # " << sampleCounter++;

        const auto & currentSample = (*dataIt);
        QVector < qreal > tempResult = currentSample.getData();
        for ( auto it = layers.constBegin(); it != layers.constEnd(); ++ it ) {
            // TODO remove debug otput
            qDebug() << "tempResult = " << tempResult;
            tempResult = (*it).process(tempResult);
        }
        Result::value_type resultSample(tempResult);
        (*resultIt) = resultSample;
    }
    qDebug() << "Resutl = ";
    std::for_each( result.constBegin(), result.constEnd(), []( Result::const_reference constSample ) {
        static quint32 resultSampleCount = 0;
        qDebug() << "resultSampleCount #" << resultSampleCount++ << constSample.getData();
    } );

    return result;
}
