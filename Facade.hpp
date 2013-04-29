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

    void setBeta( const qreal val );
    void setAlhpa( const qreal val );
    void setAccuracy( const qreal val );
    void setMaxNumberOfEpoh( const quint32 val );

    void setTrainingDataPercent( const quint32 val );

    void setInputFileName( const QString fileName );
    void setOutputFileName( const QString fileName );

    void startProcess();
    void stopProcess();

signals:
    void processEnd();

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
