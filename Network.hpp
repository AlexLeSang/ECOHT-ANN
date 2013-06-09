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
constexpr quint32 numberOfInputs = 2; // TODO read this from the facade

/*!
 * \brief numberOfOutputs
 */
constexpr quint32 numberOfOutputs = 1; // TODO read this from the facade

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
class Network : public QRunnable
{
public:
    static Network & getInstance();
    void run();

    QVector<Layer> & getLayers();
    const QVector<Layer> & getLayers() const;
    const QVector< qreal > getNetworkError() const;

    void stop();

    void setTrainigResult(const QVector< QVector< qreal > > &value);
    void setTrainingData(const QVector< QVector< qreal > > &value);
    void setTestingResult(const QVector< QVector< qreal > > &value);
    void setTestingData(const QVector< QVector< qreal > > &value);

    void setLayersDescription(const QVector<LayerDescription> &value);

    Result getObtainedTestingResult() const;
    QVector<qreal> getObtainedTestingError() const;

    qreal getAccuracy() const;
    void setAccuracy(const qreal value);

    qreal getAlpha() const;
    void setAlpha(const qreal value);

    qreal getBeta() const;
    void setBeta(const qreal value);

    quint32 getMaxNumberOfEpoch() const;
    void setMaxNumberOfEpoch(const quint32 value);

    QVector< QVector < qreal > > processInNetwork (Data::const_reference dataSample );

private:
    Network();
    Network(const Network & rNetwork) = delete;
    Network & operator = (const Network & rNetwork) = delete;

    void initLayers();
    void training(const Data & dataSet, const Result & desiredResult);
    void testing(const Data & data, const Result &desiredResult);
    qreal calculateNetworkError(const QVector<qreal> &desiredResult, const QVector<qreal> &obtainedResult) const;
    const QVector<qreal> calculateOutputGradient(const QVector<qreal> &desiredResult, const QVector<qreal> &obtainedResult) const;

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
    QVector < qreal > testingError;

    QVector< LayerDescription > layersDescription;
};


#ifdef TEST_MODE

#include <QtTest/QtTest>
#include <QObject>

class NetworkTest : public QObject {
    Q_OBJECT
public:
    void test();

private slots:
    void ProcessTest();
};

#endif

#endif // NETWORK_HPP
