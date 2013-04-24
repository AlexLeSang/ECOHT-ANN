#ifndef NEURON_H
#define NEURON_H

#include <QVector>
#include "Defines.hpp"

class Neuron {

public:
    Neuron();

    void initNeuron(const quint32 numberOfInputs);

    QVector<qreal> &getWeights();
    const QVector<qreal> &getWeights() const;

    quint32 getId() const;
private:
    static quint32 idCounter;

    const quint32 id;
    QVector < qreal > weights;
};

#ifdef TEST_NEURON

#include <QtTest/QtTest>
#include <QObject>

class NeuronTest : public QObject {
    Q_OBJECT
private slots:
    void InitializationTest(){
        Neuron neuron;
        qDebug() << "neuron.getWeights() = " << neuron.getWeights();
        neuron.initNeuron(10);
        qDebug() << "neuron.getWeights() = " << neuron.getWeights();
    }
};
#endif

#endif // NEURON_H
