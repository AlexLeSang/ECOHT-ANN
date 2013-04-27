#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cstdlib>
#include <ctime>

#include <QDebug>

constexpr auto randomLambda = [](qreal & val) {
    static bool seeded = false;
    if( !seeded ) {
        srand(time(NULL));
        seeded = true;
    }
    val = -1.0 + ((qreal)rand() / RAND_MAX) * (2.0);
};

#endif // HELPERS_HPP
