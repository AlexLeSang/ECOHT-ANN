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
    qreal result = 0.0;
    // NOTE it can be done better
    auto weightIt = weights.constBegin();
    for ( auto inputIt = inputs.constBegin(); inputIt != inputs.constEnd(); ++ inputIt, ++ weightIt ) {
        result += (*inputIt) * (*weightIt);
    }
    result -= bias;
    result = std::tanh(beta*result);
    return result;
}

void Neuron::initNeuron(const quint32 numberOfInputs) {
    Q_ASSERT(numberOfInputs > 0);
    weights = QVector < qreal > ( numberOfInputs );
    std::for_each( weights.begin(), weights.end(), randomLambda );
}

quint32 Neuron::getId() const {
    return id;
}
