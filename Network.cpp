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
        const auto numberOfNeurons = (*descriptionIterator).first;
        const auto numberOfInputs = (*descriptionIterator).second;
        ++descriptionIterator;
        layer.initLayer( numberOfNeurons, numberOfInputs );
    });
}

void Network::training(const Data &dataSet, const Result &result) {
    Q_ASSERT(dataSet.size() == result.size());

    layers = QVector< Layer > ( numberOfLayers );
}

QVector<Layer> &Network::getLayers() {
    return layers;
}

const QVector<Layer> &Network::getLayers() const {
    return layers;
}

#ifdef TEST_NETWORK
QTEST_MAIN(NetworkTest)
#endif
