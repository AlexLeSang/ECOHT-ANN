#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cstdlib>
#include <ctime>

#include <QVector>

constexpr auto randomLambda = [](qreal & val) {
    static bool seeded = false;
    if( !seeded ) {
        srand(time(NULL));
        seeded = true;
    }
    val = ((qreal)rand() / RAND_MAX);
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

constexpr auto derivLinLambda = []( const qreal & value ) {
    return value;
};

constexpr auto tanhLambda = []( const QVector<qreal> &inputs, const QVector<qreal> &weights, const qreal bias, const qreal beta ) {
    Q_ASSERT( beta != 0.0 );
    qreal result = 0.0;
    result = linLambda( inputs, weights, bias );
    result = std::tanh( beta * result );
    return result;
};

constexpr auto derivTanhLambda = []( const qreal & value, const qreal & beta ) {
    return beta * (1.0 * std::pow( std::tanh( value ), 2 ) );
};

#endif // HELPERS_HPP
