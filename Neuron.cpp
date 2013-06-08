#include "Neuron.hpp"

#include "Network.hpp"

/*!
 * \brief Neuron::idCounter
 */
quint32 Neuron::idCounter = 0;

/*!
 * \brief Neuron::Neuron
 */
Neuron::Neuron() : id(++idCounter), weights( QVector< qreal> () ) {}

/*!
 * \brief Neuron::getWeights
 * \return
 */
QVector<qreal> &Neuron::getWeights()
{
    return weights;
}

/*!
 * \brief Neuron::getWeights
 * \return
 */
const QVector<qreal> &Neuron::getWeights() const
{
    return weights;
}

/*!
 * \brief Neuron::process
 * \param inputs
 * \param bias
 * \return
 */
qreal Neuron::process(const QVector<qreal> &inputs, const qreal bias) const
{
    Q_ASSERT(inputs.size() == weights.size());
    // WARNING
//    if ( lastLayer ) {
//        return linLambda( inputs, weights, bias );
//    }
//    else {
//        return tanhLambda( inputs, weights, bias, beta );
//    }
    return linLambda( inputs, weights, bias );
//    return tanhLambda( inputs, weights, bias, beta );
}

/*!
 * \brief Neuron::operator =
 * \param rNeuron
 * \return
 */
Neuron & Neuron::operator =(const Neuron &rNeuron)
{
    if ( &rNeuron != this ) {
        weights = rNeuron.getWeights();
    }
    return *this;
}

/*!
 * \brief Neuron::initNeuron
 * \param numberOfInputs
 * \param beta
 * \param lastLayer
 */
void Neuron::initNeuron(const quint32 numberOfInputs, const qreal beta, const bool lastLayer)
{
    Q_ASSERT(numberOfInputs > 0);
    this->lastLayer = lastLayer;
    this->beta = beta;
    weights = QVector < qreal > ( numberOfInputs );
    std::for_each( weights.begin(), weights.end(), randomLambda );
}

/*!
 * \brief Neuron::getId
 * \return
 */
quint32 Neuron::getId() const
{
    return id;
}


#ifdef TEST_MODE
void NeuronTest::EmptyTest()
{
    Neuron neuron;
    QCOMPARE(neuron.getWeights().size(), 0);
}

void NeuronTest::InitializationTest()
{
    Neuron neuron;
    neuron.initNeuron(10);
    QCOMPARE(neuron.getWeights().size(), 10);
}

void NeuronTest::ProcessTest()
{
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
#endif
