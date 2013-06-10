#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cstdlib>
#include <ctime>

#include <QVector>
#include <QDebug>
#include <functional>

constexpr auto randomLambda = [](qreal & val)
{
    static bool seeded = false;
    if( !seeded ) {
        srand(time(NULL));
        seeded = true;
    }
//    val = ((qreal)rand() / RAND_MAX);
    val = 0.3 * ((qreal)rand() / RAND_MAX);
//    qDebug() << "val:" << val;
};

//constexpr auto sigmLambda = []( const QVector<qreal> &inputs, const QVector<qreal> &weights )
//{
//    QVector< qreal > resultVector( inputs.size() );
//    std::transform( inputs.constBegin(), inputs.constEnd(), weights.constBegin(), resultVector.begin(), std::multiplies<qreal>() );
//    const qreal sum = - std::accumulate( resultVector.constBegin(), resultVector.constEnd(), 0 );
//    return 1.0/(1.0 - exp(sum));
//};

//constexpr auto derivSigmLambda = []( const qreal & x )
//{
//    return (exp(x))/( pow( 1.0 + exp(x), 2.0) );
//};


//constexpr auto linLambda = []( const QVector<qreal> &inputs, const QVector<qreal> &weights, const qreal bias )
//{
//    qreal result = 0.0;
//    // NOTE Oleksandr Halushko it can be done better
//    auto weightIt = weights.constBegin();
//    for ( auto inputIt = inputs.constBegin(); inputIt != inputs.constEnd(); ++ inputIt, ++ weightIt ) {
//        result += (*inputIt) * (*weightIt);
//    }
//    result -= bias;
//    return result;
//};

//constexpr auto derivLinLambda = []( const qreal & value )
//{
//    return value;
//};

//constexpr auto tanhLambda = []( const QVector<qreal> &inputs, const QVector<qreal> &weights, const qreal bias, const qreal beta )
//{
//    Q_ASSERT( beta != 0.0 );
//    qreal result = 0.0;
//    result = linLambda( inputs, weights, bias );
//    result = std::tanh( beta * result );
//    return result;
//};

//constexpr auto derivTanhLambda = []( const qreal & value, const qreal & beta )
//{
//    return beta * (1.0 * std::pow( std::tanh( value ), 2 ) );
//};


namespace std {

template < typename _InputIterator1, typename _InputIterator2, typename _InputIterator3, typename _OutputIterator, typename _Function >
/*!
 * \brief transform_3
 * \param _begin1
 * \param _end1
 * \param _begin2
 * \param _begin3
 * \param _result
 * \param function
 * \return
 */
_OutputIterator transform_3 ( _InputIterator1 _begin1, _InputIterator1 _end1, _InputIterator2 _begin2, _InputIterator3 _begin3, _OutputIterator _result, _Function function )
{
    while ( _begin1 != _end1 ) {
        *_result = function( *_begin1, *_begin2, *_begin3 );
        ++ _result;
        ++ _begin1;
        ++ _begin2;
        ++ _begin3;
    }
    return _result;
}

}

#endif // HELPERS_HPP
