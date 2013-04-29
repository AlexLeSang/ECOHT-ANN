#ifndef FACADE_HPP
#define FACADE_HPP

#include <QObject>

#include "Network.hpp"
#include "Preprocessor.hpp"

class Facade : public QObject {
    Q_OBJECT
public:
    ~Facade() {}
    static Facade & getInstance();
public slots:
    // Set network paramiters

    void setBeta( const qreal val );
    void setAlhpa( const qreal val );
    void setAccuracy( const qreal val );
    void setMaxNumberOfEpoh( const quint32 val );


    void startProcess();
    void stopProcess();

signals:
    // Get data from the network
    void processEnd();


private:
    Facade();

private:
    Network & networkRef;

    qreal beta;
    qreal alpha;
    qreal accuracy;
    quint32 maxNumberOfEpoch;
};

#endif // FACADE_HPP
