#include "Network.hpp"

#include "Layer.hpp"

Network::Network() {}

Network &Network::getInstance() {
    static Network instance;
    return instance;
}

void Network::initNetwork(const QVector< LayerDescription > &layersDescription) {
    Q_ASSERT(layersDescription.size() != 0);
    layers = QVector < Layer >( layersDescription.size() );
    auto descriptionIterator = layersDescription.begin();
    std::for_each(layers.begin(), layers.end(), [&](Layer & layer){
        layer.initLayer( (*descriptionIterator).first, (*descriptionIterator).second );
        ++descriptionIterator;
    });
}

void Network::training(const Data &dataSet, const Result &result) {
    Q_ASSERT(dataSet.size() == result.size());
    // TODO training
}

QVector<Layer> &Network::getLayers() {
    return layers;
}

const QVector<Layer> &Network::getLayers() const {
    return layers;
}
