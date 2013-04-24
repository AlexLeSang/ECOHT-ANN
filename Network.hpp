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


#ifdef TEST_NETWORK

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
        qDebug() << "layersDesciption: " << layersDesciption;
        network.initNetwork(layersDesciption);
        const auto layers = network.getLayers();
        std::for_each(layers.constBegin(), layers.constEnd(), [](const Layer & layer){
            static quint32 layerIndex = 0;
            const auto neurons = layer.getNeurons();
            qDebug() << "layerIndex: " << layerIndex++;
            std::for_each(neurons.constBegin(), neurons.constEnd(), [](const Neuron & neuron){
                qDebug() << neuron.getWeights();
            });
        });
    }
};

#endif

#endif // NETWORK_HPP
