#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <QtCore>
#include <Exceptions.hpp>

typedef QPair < QVector < qreal >, QVector < qreal > >  DataSample;
typedef QVector< DataSample > Dataset;

class Preprocessor {
public:
    static Preprocessor & getInstance();

    const Dataset readFile(const QString& fileName );
    void writeFile( const QString & fileName, const Dataset & data, const quint32 numb );
};

#endif // PREPROCESSOR_HPP
