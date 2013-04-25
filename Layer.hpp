#ifndef LAYER_HPP
#define LAYER_HPP

#include "Defines.hpp"

#include <QVector>
#include "Neuron.hpp"

class Layer {

public:
    Layer();
    quint32 getId() const;

    void initLayer(const quint32 numberOfNeurons, const quint32 numberOfInputs);

    QVector< Neuron > & getNeurons();
    const QVector< Neuron > & getNeurons() const;

private:
    static quint32 layerCounter;

    const quint32 id;
    QVector< Neuron > neurons;
};

#ifdef TEST_MODE

#include <QtTest/QtTest>
#include <QObject>

class LayerTest : public QObject {
    Q_OBJECT
private slots:
    void EmptyTest() {
        Layer layer;
        QCOMPARE(layer.getNeurons().size(), 0);
    }

    void InitializationTest(){
        Layer layer;
        layer.initLayer(20, 5);
        const auto neurons = layer.getNeurons();
        QCOMPARE(neurons.size(), 20);
        std::for_each(neurons.constBegin(), neurons.constEnd(), [](const Neuron & neuron){
            QCOMPARE(neuron.getWeights().size(), 5);
        });
    }
};

#endif

#endif // LAYER_HPP
