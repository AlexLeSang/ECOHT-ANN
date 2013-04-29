#include "Facade.hpp"

Facade::Facade() : networkRef( Network::getInstance() ) {
}

Facade &Facade::getInstance() {
    static Facade instance; return instance;
}

void Facade::setBeta(const qreal val) {
    beta = val;
}

void Facade::setAlhpa(const qreal val) {
    alpha = val;
}

void Facade::setAccuracy(const qreal val) {
    accuracy = val;
}

void Facade::setMaxNumberOfEpoh(const quint32 val) {
    maxNumberOfEpoch = val;
}

void Facade::startProcess() {
    // TODO implement me
}

void Facade::stopProcess() {
    // TODO implement me
}
