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
    // Sigmoidal
    {
        Q_UNUSED(bias);
//        qDebug() << "weights: " << weights;
//        qDebug() << "inputs: " << inputs;
        qreal sum = 0.0;
        for ( auto iIt = inputs.constBegin(), wIt = weights.constBegin(); iIt != inputs.constEnd(); ++ iIt, ++ wIt ) {
            sum += (*iIt) * (*wIt);
        }
        sum = - sum;
//        qDebug() << "sum:" << sum;
        qreal value;
        value = 1.0/(1.0 - exp(sum));
//        if ( fabs(sum) < 1e-200 ) {
//            value = 1e10;
//        }
//        else {
//            value = 1.0/(1.0 - exp(sum));
//        }

        value /= weights.size();

//        qDebug() << "value:" << value;

//        exit(1);
        return value;
    }

//    if ( lastLayer ) {
//        return linLambda( inputs, weights, bias );
//    }
//    else {
//        return tanhLambda( inputs, weights, bias, beta );
//    }
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
