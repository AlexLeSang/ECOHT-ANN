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
        return linLambda( inputs, weights, bias );
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
    weights = QVector < qreal > ( numberOfInputs );
    std::for_each( weights.begin(), weights.end(), randomLambda );
}

quint32 Neuron::getId() const {
    return id;
}


void NeuronTest::EmptyTest() {
    Neuron neuron;
    QCOMPARE(neuron.getWeights().size(), 0);
}

void NeuronTest::InitializationTest() {
    Neuron neuron;
    neuron.initNeuron(10);
    QCOMPARE(neuron.getWeights().size(), 10);
}

void NeuronTest::ProcessTest() {
    Neuron neuron;
    {
        QVector< qreal > inputVector( 30 );
        std::for_each( inputVector.begin(), inputVector.end(), randomLambda );
        neuron.initNeuron( inputVector.size() );
        const auto result = neuron.process( inputVector );
        //            qDebug() << "result = " << result;
        Q_UNUSED( result );
    }

    {
        QVector< qreal > inputVector( 80 );
        std::for_each( inputVector.begin(), inputVector.end(), randomLambda );
        neuron.initNeuron( inputVector.size() );
        const auto result = neuron.process( inputVector );
        //            qDebug() << "result = " << result;
        Q_UNUSED( result );
    }

    {
        QVector< qreal > inputVector( 2 );
        std::for_each( inputVector.begin(), inputVector.end(), randomLambda );
        neuron.initNeuron( inputVector.size() );
        const auto result = neuron.process( inputVector );
        //            qDebug() << "result = " << result;
        Q_UNUSED( result );
    }
}
