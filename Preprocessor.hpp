#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <QtCore>
#include <Exceptions.hpp>

typedef QPair < QVector < qreal >, QVector < qreal > >  SampleData;
typedef QVector< SampleData > Dataset;

class Preprocessor
{
public:
    Preprocessor();
    Dataset readFile(const QString& fileName );
    void writeFile( const QString & fileName, const Dataset & data, const quint32 numb );
};

#endif // PREPROCESSOR_HPP
