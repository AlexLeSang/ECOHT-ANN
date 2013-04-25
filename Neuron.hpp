#ifndef NEURON_H
#define NEURON_H

#include <QVector>
#include "Defines.hpp"

class Neuron {

public:
    Neuron();
    quint32 getId() const;

    void initNeuron(const quint32 numberOfInputs);

    QVector<qreal> &getWeights();
    const QVector<qreal> &getWeights() const;

private:
    static quint32 idCounter;

    const quint32 id;
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
};
#endif

#endif // NEURON_H
