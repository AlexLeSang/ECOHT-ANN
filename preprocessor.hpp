#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <QtCore>

typedef QVector< QPair < QVector < qreal >, QVector < qreal > > > qvpvrvr;

class Preprocessor
{
public:
    Preprocessor();
    qvpvrvr readFile(const QString& filename);
    bool writeFile(const QString& filename, qvpvrvr& data, quint32 numb);
};

#endif // PREPROCESSOR_HPP
