#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <QObject>

#include "Defines.hpp"
#include "Sample.hpp"

#include "Layer.hpp"

#include <QPair>

constexpr quint32 numberOfInputs = 2;
constexpr quint32 numberOfOutputs = 1;

typedef QVector< Sample < qreal, numberOfInputs > > Data;
typedef QVector< Sample < qreal, numberOfOutputs > > Result;

typedef quint32 neuronsNumber;
typedef quint32 inputsNumber;

typedef QPair< neuronsNumber, inputsNumber > LayerDescription;


class Network : public QObject {
    Q_OBJECT
public:
    static Network & getInstance();

    void initNetwork(const QVector< LayerDescription > & layersDescription, const qreal accuracy, const quint32 maxNumberOfEpoch, const qreal trainingCoefficient);

    QVector<Layer> & getLayers();
    const QVector<Layer> & getLayers() const;

    void training(const Data & dataSet, const Result & desiredResult);
    const Result testing(const Data & data);


public slots:
    // TODO communication with interface

private:
    Network();
    Network(const Network & rNetwork) = delete;
    Network & operator = (const Network & rNetwork) = delete;

    Result process(const Data & data);

private:
    QVector< Layer > layers;
    quint32 maxNumberOfEpoch;
    qreal accuracy;
    qreal trainigCoefficien;
};


#ifdef TEST_MODE

#include <QtTest/QtTest>
#include <QObject>

class NetworkTest : public QObject {
    Q_OBJECT
private slots:

    void InitializationTest(){
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

    void ProcessTest() {
        constexpr quint32 numberOfDataSamples = 5;
        constexpr quint32 numberOfInputs = 2;
        constexpr quint32 numberOfOutputs = 1;

        Network & network = Network::getInstance();
        QVector< LayerDescription > layersDesciption;
        layersDesciption.append(LayerDescription(2, numberOfInputs));
        layersDesciption.append(LayerDescription(4, 2));
        layersDesciption.append(LayerDescription(numberOfOutputs, 4));
        network.initNetwork(layersDesciption, 1e-5, 1, 0.1);

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
};

#endif

#endif // NETWORK_HPP
