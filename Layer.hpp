#ifndef LAYER_HPP
#define LAYER_HPP

#include <QVector>

#include "Defines.hpp"
#include "Neuron.hpp"

/*!
 * \brief LayerOutput
 */
typedef QVector< qreal > LayerOutput;

/*!
 * \brief The Layer class
 */
class Layer
{
public:
    Layer();
    quint32 getId() const;
    void initLayer( const quint32 numberOfNeurons, const quint32 numberOfInputs, const qreal beta = 1.0, const bool lastLayer = false );
    qreal getBeta() const;
    QVector< Neuron > & getNeurons();
    const QVector< Neuron > & getNeurons() const;
    const QVector< qreal > process( const QVector<qreal> &inputs ) const;

private:
    static quint32 layerCounter;
    const quint32 id;
    qreal beta;
    QVector< Neuron > neurons;
};

#ifdef TEST_MODE

#include <QtTest/QtTest>
#include <QObject>

class LayerTest : public QObject {
    Q_OBJECT
private slots:
    void EmptyTest();
    void InitializationTest();
    void ProcessTest();
};

#endif

#endif // LAYER_HPP
