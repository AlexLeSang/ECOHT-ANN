#ifndef NEURON_H
#define NEURON_H

#include <QVector>

#include "Defines.hpp"

#include "Helpers.hpp"

class Neuron {

public:
    Neuron();
    quint32 getId() const;

    void initNeuron(const quint32 numberOfInputs, const bool lastLayer = false);

    QVector< qreal > &getWeights();
    const QVector< qreal > &getWeights() const;

    qreal process(const QVector< qreal > & inputs, const qreal bias = 0.0, const qreal beta = 1.0) const;

    Neuron &operator =(const Neuron &rNeuron);

private:
    static quint32 idCounter;

    const quint32 id;

    bool lastLayer;

    QVector < qreal > weights;
};

#ifdef TEST_MODE

#include <QtTest/QtTest>
#include <QObject>

class NeuronTest : public QObject {
    Q_OBJECT
private slots:

    void EmptyTest(){
        Neuron neuron;
        QCOMPARE(neuron.getWeights().size(), 0);
    }

    void InitializationTest(){
        Neuron neuron;
        neuron.initNeuron(10);
        QCOMPARE(neuron.getWeights().size(), 10);
    }

    void ProcessTest() {
        Neuron neuron;
        {
            QVector< qreal > inputVector( 30 );
            std::for_each( inputVector.begin(), inputVector.end(), randomLambda );
            neuron.initNeuron( inputVector.size() );
            const auto result = neuron.process( inputVector );
            //            qDebug() << "result = " << result;
            Q_UNUSED( result );
        }

        {
            QVector< qreal > inputVector( 80 );
            std::for_each( inputVector.begin(), inputVector.end(), randomLambda );
            neuron.initNeuron( inputVector.size() );
            const auto result = neuron.process( inputVector );
            //            qDebug() << "result = " << result;
            Q_UNUSED( result );
        }

        {
            QVector< qreal > inputVector( 2 );
            std::for_each( inputVector.begin(), inputVector.end(), randomLambda );
            neuron.initNeuron( inputVector.size() );
            const auto result = neuron.process( inputVector );
            //            qDebug() << "result = " << result;
            Q_UNUSED( result );
        }
    }
};
#endif

#endif // NEURON_H
