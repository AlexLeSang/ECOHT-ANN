#include "Layer.hpp"

quint32 Layer::layerCounter = 0;

Layer::Layer() : id( layerCounter++ ), neurons( QVector< Neuron > () ) {}

void Layer::initLayer(const quint32 numberOfNeurons, const quint32 numberOfInputs) {
    Q_ASSERT(numberOfNeurons > 0);
    Q_ASSERT(numberOfInputs > 0);
    neurons = QVector< Neuron > ( numberOfNeurons );
    std::for_each(neurons.begin(), neurons.end(),[&](Neuron & neuron) {
        neuron.initNeuron(numberOfInputs);
    } );
}

QVector<Neuron> &Layer::getNeurons() {
    return neurons;
}

const QVector<Neuron> &Layer::getNeurons() const {
    return neurons;
}

quint32 Layer::getId() const {
    return id;
}
