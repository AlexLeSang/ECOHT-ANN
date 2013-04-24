#include "Neuron.hpp"

#include "Network.hpp"

quint32 Neuron::idCounter = 0;

Neuron::Neuron() : id(++idCounter), weights( QVector< qreal> () ) {}

QVector<qreal> &Neuron::getWeights() {
    return weights;
}

const QVector<qreal> &Neuron::getWeights() const {
    return weights;
}

void Neuron::initNeuron(const quint32 numberOfInputs) {
    weights = QVector < qreal > ( numberOfInputs );
    auto randomLabmda = [](qreal & val) {
        val = (qreal)std::rand()/RAND_MAX;
    };
    std::for_each(weights.begin(), weights.end(), randomLabmda);
}

quint32 Neuron::getId() const {
    return id;
}

#ifdef TEST_NEURON

QTEST_MAIN(NeuronTest)

#endif
