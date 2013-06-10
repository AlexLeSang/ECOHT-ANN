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


typedef QVector< QVector < qreal > > IntermidResults;

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
    static Network & getInstance() {
        static Network instance;
        return instance;
    }
    void run();

    QVector<Layer> & getLayers() {
        return layers;
    }
    const QVector<Layer> & getLayers() const {
        return layers;
    }
    const QVector< qreal > getNetworkError() const {
        // const QVector< qreal > vector( errorList.toVector() );
        return testingError;
    }

    void stop() {
        stopFlag = true;
    }

    void setTrainigResult(const QVector< QVector< qreal > > &value) {
        trainigResult.resize( value.size() );
        auto trainigResultIt = trainigResult.begin();
        for ( auto valueIt = value.begin(); valueIt != value.end(); ++ valueIt, ++ trainigResultIt ) {
            Q_ASSERT( static_cast< quint32 >( (*valueIt).size() ) == numberOfOutputs );
            (*trainigResultIt) = Result::value_type( (*valueIt) );
        }
        Q_ASSERT( trainigResult.size() > 0 );
    }
    void setTrainingData(const QVector< QVector< qreal > > &value) {
        trainingData.resize( value.size() );
        auto trainigDatatIt = trainingData.begin();
        for ( auto valueIt = value.begin(); valueIt != value.end(); ++ valueIt, ++ trainigDatatIt ) {
            Q_ASSERT( static_cast< quint32 >( (*valueIt).size() ) == numberOfInputs );
            (*trainigDatatIt) = Data::value_type( (*valueIt) );
        }
        Q_ASSERT( trainingData.size() > 0 );
    }
    void setTestingResult(const QVector< QVector< qreal > > &value) {
        testingResult.resize( value.size() );
        auto testingResultIt = testingResult.begin();
        for ( auto valueIt = value.begin(); valueIt != value.end(); ++ valueIt, ++ testingResultIt ) {
            Q_ASSERT( static_cast< quint32 >( (*valueIt).size() ) == numberOfOutputs );
            (*testingResultIt) = Result::value_type( (*valueIt) );
        }
        Q_ASSERT( testingResult.size() > 0 );
    }
    void setTestingData(const QVector< QVector< qreal > > &value) {
        testingData.resize( value.size() );
        auto testingDatatIt = testingData.begin();
        for ( auto valueIt = value.begin(); valueIt != value.end(); ++ valueIt, ++ testingDatatIt ) {
            Q_ASSERT( static_cast< quint32 >( (*valueIt).size() ) == numberOfInputs );
            (*testingDatatIt) = Data::value_type( (*valueIt) );
        }

        Q_ASSERT( testingData.size() > 0 );
    }

    void setLayersDescription(const QVector<LayerDescription> &value) {
        layersDescription = value;
    }

    Result getObtainedTestingResult() const {
        return obtainedTestingResult;
    }
    QVector<qreal> getObtainedTestingError() const {
        return testingError;
    }

    qreal getAccuracy() const {
        return accuracy;
    }
    void setAccuracy(const qreal value) {
        accuracy = value;
        Q_ASSERT( accuracy < 1.0 );
    }

    qreal getAlpha() const {
        return alpha;
    }
    void setAlpha(const qreal value) {
        alpha = value;
    }

    qreal getBeta() const {
        return beta;
    }
    void setBeta(const qreal value) {
        beta = value;
    }

    quint32 getMaxNumberOfEpoch() const {
        return maxNumberOfEpoch;
    }
    void setMaxNumberOfEpoch(const quint32 value) {
        maxNumberOfEpoch = value;
    }

    IntermidResults processInNetwork (Data::const_reference dataSample ) {
        QList < QVector < qreal > > intermidResult;
        {
            intermidResult.append( dataSample.getData() );
            std::for_each( layers.constBegin(), layers.constEnd(), [&]( const Layer & layer ) {
                auto result = layer.process( intermidResult.last() );
                intermidResult.append( result );
            } );
            intermidResult.removeFirst();
        }
        return intermidResult.toVector();
    }

    qreal getEtha() const {
        return etha;
    }
    void setEtha(const qreal &value) {
        etha = value;
    }

private:
    Network() : maxNumberOfEpoch( 50 ), accuracy( 1e-4 ), alpha( 1.0 ), beta( 1.0 ), etha( 0.99 ) {
        setAutoDelete( false );
    }
    Network(const Network & rNetwork) = delete;
    Network & operator = (const Network & rNetwork) = delete;

    void initLayers() {
        Q_ASSERT( beta != 0.0 );
        Q_ASSERT( alpha != 0.0 );
        Q_ASSERT( layersDescription.size() != 0 );
        Q_ASSERT( layersDescription.first().second == numberOfInputs ); // "Number of inputs to the network"
        Q_ASSERT( layersDescription.last().first == numberOfOutputs ); // "Number of outputs of the network")

        // Init new layer structure
        layers = QVector < Layer >( layersDescription.size() );

        // Init layers
        auto descriptionIt = layersDescription.begin();
        qint32 layerCount = 0;
        for ( auto layerIt = layers.begin(); layerIt != layers.end(); ++ layerIt,  ++ descriptionIt, ++ layerCount ) {
            (*layerIt).initLayer( (*descriptionIt).first, (*descriptionIt).second, alpha, beta, ( std::distance( layerIt, layers.end() ) == 1 ) );
        }
    }
    void training(const Data & dataSet, const Result & desiredResult);
    void testing(const Data & data, const Result &desiredResult) {
        Q_ASSERT( data.size() == desiredResult.size() );
        // Process all data
        {
            obtainedTestingResult.resize( data.size() );
            auto obtainedResultIt = obtainedTestingResult.begin();
            std::for_each( data.constBegin(), data.constEnd(), [&, this] ( Data::const_reference dataSample ) {
                const auto result = ( processInNetwork( dataSample ) ).last();
                (*obtainedResultIt).getData() = result;
                ++ obtainedResultIt;
            } );
        }
        // TODO remove debug output
        /*
    {
        qDebug() << "obtainedTestingResult = ";
        std::for_each( obtainedTestingResult.constBegin(), obtainedTestingResult.constEnd(), [] ( Result::const_reference result ) {
            qDebug() << result.getData();
        } );
    }
    */
        // TODO calculate difference between result obtained from the network and desired result
        {
            Q_ASSERT( obtainedTestingResult.size() == desiredResult.size() );
            testingError.resize( obtainedTestingResult.size() );
            std::transform ( obtainedTestingResult.constBegin(), obtainedTestingResult.constEnd(), desiredResult.constBegin(), testingError.begin(), []( Result::const_reference obtained, Result::const_reference desired ) {
                Q_ASSERT( obtained.getData().size() == desired.getData().size() );
                qreal diff = 0.0;
                auto obtainedDataIt = obtained.getData().constBegin();
                std::for_each ( desired.getData().constBegin(), desired.getData().constEnd(), [&]( const Result::value_type::value_type value ) {
                    diff += std::sqrt( std::pow( (*obtainedDataIt) - value, 2 ) );
                    ++ obtainedDataIt;
                } );
                return diff;
            } );
        }
        //     TODO remove debug output
        {
            qDebug() << "Max testing error" << ( *(std::max_element( testingError.constBegin(), testingError.constEnd() )) );
            qDebug() << "Min testing error" << ( *(std::min_element( testingError.constBegin(), testingError.constEnd() )) );
            qDebug() << "Average error = " << ( std::accumulate( testingError.constBegin(), testingError.constEnd(), 0.0 ) / testingError.size() );
        }

    }
    qreal calculateNetworkError(const QVector<qreal> &desiredResult, const QVector<qreal> &obtainedResult) const {
        Q_ASSERT( desiredResult.size() == obtainedResult.size() );
        qreal networkError = 0.0;
        auto obtainedIt = obtainedResult.constBegin();
        std::for_each( desiredResult.constBegin(), desiredResult.constEnd(), [&]( const qreal & desired ) {
            const qreal obtained = (*obtainedIt);
            ++ obtainedIt;
            networkError += std::pow( (desired - obtained), 2.0 );
        } );
        return networkError;
    }
    const QVector<qreal> calculateOutputGradient(const QVector<qreal> &desiredResult, const QVector<qreal> &obtainedResult) const {
        Q_ASSERT( desiredResult.size() == obtainedResult.size() );
        QVector< qreal > gradient( desiredResult.size() );
        auto obtainedIt = obtainedResult.constBegin();
        auto gradientIt = gradient.begin();
        std::for_each( desiredResult.constBegin(), desiredResult.constEnd(), [&]( const qreal & desired ) {
            const qreal & obtained = (*obtainedIt);
            // qDebug() << "obtained : " << obtained;
            // qDebug() << "desired : " << desired;
             auto grad = obtained * ( 1 - obtained ) * ( desired - obtained ); // Sigm
//            auto grad = ( desired - obtained ) * ( 1 - obtained ) * ( 1 + obtained ); // Tanh
            // qDebug() << "grad: " << grad;
            (*gradientIt) = grad;
            ++ obtainedIt;
            ++ gradientIt;
        } );
        return gradient;
    }

private:
    QVector< Layer > layers;
    QList< qreal > errorList;

    quint32 maxNumberOfEpoch;
    qreal accuracy;
    qreal alpha;
    qreal beta;
    qreal etha;

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
    void test() {
        ProcessTest();
    }

private slots:
    void ProcessTest();
};

#endif

#endif // NETWORK_HPP
