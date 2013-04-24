#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#include <QVector>

template < typename D, unsigned int n>
class Sample {
private:
    const QVector< D > data;

public:
    Sample() : data( QVector<D>(n) ) {}
    Sample( const decltype(data) & rData ) : data( rData ) {}

    const decltype(data) & getData() const {
        return data;
    }

    Sample( const Sample & rSample ) = delete;
    Sample & operator = ( const Sample & rSample ) = delete;
};

#endif // SAMPLE_HPP
