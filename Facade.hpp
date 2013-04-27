#ifndef FACADE_HPP
#define FACADE_HPP

#include <QObject>

class Facade : public QObject {
    Q_OBJECT
public:
    ~Facade() {}
    static Facade & getInstance();
    // TODO network interface
public slots:
    // Set network paramiters
signals:
    // Get data from the network

private:
    Facade();
};

#endif // FACADE_HPP
