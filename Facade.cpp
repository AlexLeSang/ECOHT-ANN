#include "Facade.hpp"

Facade::Facade() {
    // TODO make connections between input-output, network and gui
}

Facade &Facade::getInstance() {
    static Facade instance; return instance;
}
