#include "Layer.hpp"

quint32 Layer::layerCounter = 0;

Layer::Layer() : id( layerCounter++ ), neurons( QVector< Neuron > () ) {}

void Layer::initLayer( const quint32 numberOfNeurons, const quint32 numberOfInputs, const bool lastLayer ) {
    Q_ASSERT(numberOfNeurons > 0);
    Q_ASSERT(numberOfInputs > 0);
    neurons = QVector< Neuron > ( numberOfNeurons );
    std::for_each( neurons.begin(), neurons.end(),[&]( Neuron & neuron ) {
        neuron.initNeuron( numberOfInputs, lastLayer );
    } );
}

QVector<Neuron> &Layer::getNeurons() {
    return neurons;
}

const QVector<Neuron> &Layer::getNeurons() const {
    return neurons;
}

const QVector<qreal> Layer::process(const QVector< qreal > & inputs) const {
    QVector< qreal > result( neurons.size() );
    auto resultIt = result.begin();
    for ( auto neuronIt = neurons.constBegin(); neuronIt != neurons.constEnd(); ++ neuronIt, ++resultIt ) {
        (*resultIt) = (*neuronIt).process(inputs);
    }
    return result;
}

quint32 Layer::getId() const {
    return id;
}
