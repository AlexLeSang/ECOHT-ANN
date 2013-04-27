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

qreal Neuron::process(const QVector<qreal> &inputs, const qreal bias, const qreal beta) const {
    Q_ASSERT(inputs.size() == weights.size());

    // TODO choose activation function
    if ( lastLayer ) {
        return linLambda( inputs, bias );
    }
    else {
        return tanhLambda( inputs, weights, bias, beta );
    }
}

Neuron & Neuron::operator =(const Neuron &rNeuron) {
    if ( &rNeuron != this ) {
        weights = rNeuron.getWeights();
    }
    return *this;
}

void Neuron::initNeuron(const quint32 numberOfInputs, const bool lastLayer) {
    Q_ASSERT(numberOfInputs > 0);
    this->lastLayer = lastLayer;
    if ( !lastLayer ) {
        weights = QVector < qreal > ( numberOfInputs );
        std::for_each( weights.begin(), weights.end(), randomLambda );
    }
}

quint32 Neuron::getId() const {
    return id;
}
