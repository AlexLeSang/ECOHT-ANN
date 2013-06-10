#include "Layer.hpp"

/*!
 * \brief Layer::layerCounter
 */
quint32 Layer::layerCounter = 0;

/*!
 * \brief Layer::Layer
 */
Layer::Layer() : id( layerCounter++ ), neurons( QVector< Neuron > () ) {}

/*!
 * \brief Layer::initLayer
 * \param numberOfNeurons
 * \param numberOfInputs
 * \param beta
 * \param lastLayer
 */
void Layer::initLayer(const quint32 numberOfNeurons, const quint32 numberOfInputs, const qreal alpha, const qreal beta, const bool firstLayer )
{
    Q_ASSERT(numberOfNeurons > 0);
    Q_ASSERT(numberOfInputs > 0);
    neurons = QVector< Neuron > ( numberOfNeurons );
    std::for_each( neurons.begin(), neurons.end(),[&]( Neuron & neuron ) {
        neuron.initNeuron( numberOfInputs, alpha, beta, firstLayer );
    } );
}

/*!
 * \brief Layer::getBeta
 * \return
 */
/*qreal Layer::getBeta() const
{
    return beta;
}
*/
/*!
 * \brief Layer::getNeurons
 * \return
 */
QVector<Neuron> &Layer::getNeurons()
{
    return neurons;
}

/*!
 * \brief Layer::getNeurons
 * \return
 */
const QVector<Neuron> &Layer::getNeurons() const
{
    return neurons;
}

/*!
 * \brief Layer::process
 * \param inputs
 * \return
 */
const QVector<qreal> Layer::process(const QVector< qreal > & inputs) const
{
    QVector< qreal > result( neurons.size() );
    auto resultIt = result.begin();
    std::for_each( neurons.constBegin(), neurons.constEnd(), [&] ( const Neuron & neuron ) {
        (*resultIt) = neuron.process( inputs );
        ++ resultIt;
    } );

    /*
    qDebug() << "Layer:";
    std::for_each( neurons.constBegin(), neurons.constEnd(), [] ( const Neuron & neuron ) {
        qDebug() << neuron.getWeights();
        qDebug();
    } );

    qDebug() << "inputs: " << inputs;
    qDebug() << "result: " << result;
    */

    return result;
}

/*!
 * \brief Layer::getId
 * \return
 */
quint32 Layer::getId() const
{
    return id;
}


#ifdef TEST_MODE
void LayerTest::EmptyTest()
{
    Layer layer;
    QCOMPARE(layer.getNeurons().size(), 0);
}

void LayerTest::InitializationTest()
{
    Layer layer;
    layer.initLayer(20, 5);
    const auto neurons = layer.getNeurons();
    QCOMPARE(neurons.size(), 20);
    std::for_each(neurons.constBegin(), neurons.constEnd(), [](const Neuron & neuron){
        QCOMPARE(neuron.getWeights().size(), 5);
    });
}

void LayerTest::ProcessTest()
{
    Layer layer;
    {
        const quint32 numberOfInputs = 5;
        const quint32 numberOfNeurons = 20;

        QVector< qreal > data( numberOfInputs );
        std::for_each( data.begin(), data.end(), randomLambda );
        layer.initLayer( numberOfNeurons, data.size() );
        QVector < qreal > result( layer.getNeurons().size() );
        auto resultIt = result.begin();
        for( auto it = layer.getNeurons().constBegin(); it != layer.getNeurons().constEnd(); ++ it, ++ resultIt ) {
            (*resultIt) = (*it).process( data );
        }
        //            qDebug() << "result = " << result;
    }

    {
        const quint32 numberOfInputs = 1;
        const quint32 numberOfNeurons = 4;

        QVector< qreal > data( numberOfInputs );
        std::for_each( data.begin(), data.end(), randomLambda );
        layer.initLayer( numberOfNeurons, data.size() );
        QVector < qreal > result( layer.getNeurons().size() );
        auto resultIt = result.begin();
        for( auto it = layer.getNeurons().constBegin(); it != layer.getNeurons().constEnd(); ++ it, ++ resultIt ) {
            (*resultIt) = (*it).process( data );
        }
        //            qDebug() << "result = " << result;
    }
}
#endif
