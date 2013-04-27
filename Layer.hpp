#ifndef LAYER_HPP
#define LAYER_HPP

#include <QVector>

#include "Defines.hpp"
#include "Neuron.hpp"


typedef QVector< qreal > LayerOutput;

class Layer {

public:
    Layer();
    quint32 getId() const;

    void initLayer(const quint32 numberOfNeurons, const quint32 numberOfInputs);

    QVector< Neuron > & getNeurons();
    const QVector< Neuron > & getNeurons() const;

    const QVector< qreal > process(const QVector<qreal> &inputs) const;

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

    void ProcessTest() {
        Layer layer;
        {
            const quint32 numberOfInputs = 5;
            const quint32 numberOfNeurons = 20;

            QVector< qreal > data( numberOfInputs );
            std::for_each( data.begin(), data.end(), randomLambda );
            layer.initLayer( numberOfNeurons, data.size() );
            QVector < qreal > result( layer.getNeurons().size() );
            auto resultIt = result.begin();
            for( auto it = layer.getNeurons().constBegin(); it != layer.getNeurons().constEnd(); ++ it, ++ resultIt ) {
                (*resultIt) = (*it).process( data );
            }
            qDebug() << "result = " << result;
        }

        {
            const quint32 numberOfInputs = 1;
            const quint32 numberOfNeurons = 4;

            QVector< qreal > data( numberOfInputs );
            std::for_each( data.begin(), data.end(), randomLambda );
            layer.initLayer( numberOfNeurons, data.size() );
            QVector < qreal > result( layer.getNeurons().size() );
            auto resultIt = result.begin();
            for( auto it = layer.getNeurons().constBegin(); it != layer.getNeurons().constEnd(); ++ it, ++ resultIt ) {
                (*resultIt) = (*it).process( data );
            }
            qDebug() << "result = " << result;
        }
    }
};

#endif

#endif // LAYER_HPP
