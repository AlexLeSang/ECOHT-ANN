#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <QObject>

#include "Defines.hpp"
#include "Sample.hpp"

#include "Layer.hpp"

#include <QPair>
#include <QList>
#include <QVector>

constexpr quint32 numberOfInputs = 2;
constexpr quint32 numberOfOutputs = 1;

typedef QVector< Sample < qreal, numberOfInputs > > Data;
typedef QVector< Sample < qreal, numberOfOutputs > > Result;

typedef quint32 neuronsNumber;
typedef quint32 inputsNumber;

typedef QPair< neuronsNumber, inputsNumber > LayerDescription;


class Network {
public:
    static Network & getInstance();

    void initNetwork(const QVector< LayerDescription > & layersDescription, const qreal accuracy, const quint32 maxNumberOfEpoch, const qreal trainingCoefficient);

    QVector<Layer> & getLayers();
    const QVector<Layer> & getLayers() const;

    void training(const Data & dataSet, const Result & desiredResult);
    const Result testing(const Data & data);

private:
    Network();
    Network(const Network & rNetwork) = delete;
    Network & operator = (const Network & rNetwork) = delete;

    Result process(const Data & data);

private:
    QVector< Layer > layers;
    quint32 maxNumberOfEpoch;
    qreal accuracy;
    qreal trainigCoefficien;
};


#ifdef TEST_MODE

#include <QtTest/QtTest>
#include <QObject>

class NetworkTest : public QObject {
    Q_OBJECT
private slots:
    void InitializationTest();
    void ProcessTest();
};

#endif

#endif // NETWORK_HPP
