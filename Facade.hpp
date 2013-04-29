#ifndef FACADE_HPP
#define FACADE_HPP

#include <QObject>

#include "Network.hpp"
#include "Preprocessor.hpp"

class Facade : public QObject {
    Q_OBJECT
public:
    static Facade & getInstance();
    void processFinished();

    QVector< qreal > getErrors() const;

public slots:
    // Set network paramiters

    void setBeta( double val );
    void setAlhpa( double val );
    void setAccuracy( double val );
    void setMaxNumberOfEpoh( int val );
    void setInitialLayerInfo( const LayerDescription & val );
    void setTrainingDataPercent(int val);

    void setInputFileName( const QString fileName );
    void setOutputFileName( const QString fileName );

    void setLayersDescription( const QVector< QPair < quint32, quint32 > > & layersDescription );

    void startProcess();
    void stopProcess();

signals:
    void processEnd();
    void sendInitialLayerInfo( const LayerDescription & val );
private:
    Facade();

private:
    Network & networkRef;
    Preprocessor & preprocessorRef;

    qreal beta;
    qreal alpha;
    qreal accuracy;
    quint32 maxNumberOfEpoch;
};

#endif // FACADE_HPP
