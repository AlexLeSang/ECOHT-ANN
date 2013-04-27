#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cstdlib>
#include <ctime>

constexpr auto randomLambda = [](qreal & val) {
    static bool seeded = false;
    if( !seeded ) {
        srand(time(NULL));
        seeded = true;
    }
    val = -1.0 + ((qreal)rand() / RAND_MAX) * (2.0);
};

constexpr auto linLambda = []( const QVector<qreal> &inputs, const QVector<qreal> &weights, const qreal bias ) {
    qreal result = 0.0;
    // NOTE it can be done better
    auto weightIt = weights.constBegin();
    for ( auto inputIt = inputs.constBegin(); inputIt != inputs.constEnd(); ++ inputIt, ++ weightIt ) {
        result += (*inputIt) * (*weightIt);
    }
    result -= bias;
    return result;
};

constexpr auto tanhLambda = []( const QVector<qreal> &inputs, const QVector<qreal> &weights, const qreal bias, const qreal beta ) {
    qreal result = 0.0;
    result = linLambda( inputs, weights, bias );
    result = std::tanh( beta * result );
    return result;
};

#endif // HELPERS_HPP
