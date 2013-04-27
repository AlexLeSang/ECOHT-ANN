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
