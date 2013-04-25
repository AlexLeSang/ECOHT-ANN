#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <QObject>

#include "Defines.hpp"
#include "Sample.hpp"

#include "Layer.hpp"

#include <QPair>

constexpr quint32 numberOfInputs = 2;
constexpr quint32 numberOfOutputs = 1;

typedef QVector< const Sample < qreal, numberOfInputs > > Data;
typedef QVector< const Sample < qreal, numberOfOutputs > > Result;

typedef quint32 neuronsNumber;
typedef quint32 inputsNumber;

typedef QPair< neuronsNumber, inputsNumber > LayerDescription;


class Network : public QObject {
    Q_OBJECT
public:
    static Network & getInstance();

    void initNetwork(const QVector< LayerDescription > & layersDescription);

    QVector<Layer> & getLayers();
    const QVector<Layer> & getLayers() const;

    void training(const Data & dataSet, const Result & result);
    const Result testing(const Data & data);


public slots:
    // TODO communication with interface

private:
    Network();
    Network(const Network & rNetwork) = delete;
    Network & operator = (const Network & rNetwork) = delete;

private:
    QVector< Layer > layers;
    quint32 maxNumberOfEpoch;
    quint32 numberOfLayers;

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
        layersDesciption.append(LayerDescription(2, 4));
        layersDesciption.append(LayerDescription(4, 5));
        layersDesciption.append(LayerDescription(4, 7));
        network.initNetwork(layersDesciption);
        const auto & constNetworkRef = network;
        QCOMPARE(constNetworkRef.getLayers().size(), layersDesciption.size());
        auto descIter = layersDesciption.begin();
        for ( auto iter = constNetworkRef.getLayers().constBegin(); iter != constNetworkRef.getLayers().constEnd(); ++iter ) {

            const auto requiredNeurons = (*descIter).first;
            const auto requiredWeights = (*descIter).second;
            const auto & neurons = (*iter).getNeurons();
            const auto achievedNeurons = static_cast< decltype(requiredNeurons) >( neurons.size() );

            const bool equal = achievedNeurons == requiredNeurons;
            QCOMPARE(true, equal);

            std::for_each( neurons.constBegin(), neurons.constEnd(), [&](const Neuron & neuron) {
                const auto achievedWeights = static_cast< decltype(requiredWeights) >( neuron.getWeights().size() );
                const bool equal = achievedWeights == requiredWeights;
                QCOMPARE( true, equal );
            } );

            ++ descIter;
        }
    }
};

#endif

#endif // NETWORK_HPP
