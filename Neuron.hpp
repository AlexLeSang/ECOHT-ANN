#ifndef NEURON_H
#define NEURON_H

#include <QVector>

#include "Defines.hpp"
#include "Helpers.hpp"

/*!
 * \brief The Neuron class
 */
class Neuron
{
public:
    Neuron();
    quint32 getId() const;
    void initNeuron(const quint32 numberOfInputs, const qreal alpha = 1.0, const qreal beta = 0.0, const bool lastLayer = false);
    QVector< qreal > &getWeights();
    const QVector< qreal > &getWeights() const;
    qreal process(const QVector< qreal > & inputs) const;
    Neuron &operator =(const Neuron &rNeuron);

    //qreal getBias() const;
    //void setBias(const qreal &value);

private:
    static quint32 idCounter;
    const quint32 id;
    bool lastLayer;
    qreal alpha;
    qreal beta;
    QVector < qreal > weights;
   // qreal bias;
};

#ifdef TEST_MODE

#include <QtTest/QtTest>
#include <QObject>
class NeuronTest : public QObject {
    Q_OBJECT
private slots:
    void EmptyTest();
    void InitializationTest();
    void ProcessTest();
};
#endif

#endif // NEURON_H
