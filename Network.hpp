#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <QObject>

#include "Defines.hpp"
#include "Sample.hpp"

#include "Layer.hpp"

#include <QPair>
#include <QList>
#include <QVector>

#include <QRunnable>

/*!
 * \brief numberOfInputs
 */
constexpr quint32 numberOfInputs = 2;

/*!
 * \brief numberOfOutputs
 */
constexpr quint32 numberOfOutputs = 1;

/*!
 * \brief Data
 */
typedef QVector< Sample < qreal, numberOfInputs > > Data;

/*!
 * \brief Result
 */
typedef QVector< Sample < qreal, numberOfOutputs > > Result;

/*!
 * \brief neuronsNumber
 */
typedef quint32 neuronsNumber;

/*!
 * \brief inputsNumber
 */
typedef quint32 inputsNumber;

/*!
 * \brief LayerDescription
 */
typedef QPair< neuronsNumber, inputsNumber > LayerDescription;

/*!
 * \brief The Network class
 */
class Network : public QRunnable {

public:
    static Network & getInstance();
    void run();

    void initNetwork(const QVector< LayerDescription > & layersDescription, const qreal accuracy, const quint32 maxNumberOfEpoch, const qreal alpha, const qreal beta = 1.0);
    QVector<Layer> & getLayers();
    const QVector<Layer> & getLayers() const;
    const QVector< qreal > getNetworkError() const;
    void training(const Data & dataSet, const Result & desiredResult);
    void testing(const Data & data, const Result &desiredResult);
    qreal getBeta() const;

    void stop();

    void setTrainigResult(const Result &value);
    void setTrainingData(const Data &value);
    void setTestingResult(const Result &value);
    void setTestingData(const Data &value);

    void setLayersDescription(const QVector<LayerDescription> &value);

    Result getObtainedTestingResult() const;
    QVector<qreal> getObtainedTestingError() const;

private:
    Network();
    Network(const Network & rNetwork) = delete;
    Network & operator = (const Network & rNetwork) = delete;
    QPair<Result, QVector<QVector<qreal> > > process(const Data &data);

private:
    QVector< Layer > layers;
    QList< qreal > errorList;
    quint32 maxNumberOfEpoch;
    qreal accuracy;
    qreal alpha;
    qreal beta;

    volatile bool stopFlag;

    Result trainigResult;
    Data trainingData;

    Result testingResult;
    Data testingData;

    Result obtainedTestingResult;
    QVector < qreal > obtainedTestingError;

    QVector< LayerDescription > layersDescription;
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
