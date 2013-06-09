#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <QtCore>

#include "Exceptions.hpp"
#include "Defines.hpp"
#include <algorithm>

/*!
 * \brief DataSample
 */
typedef QPair < QVector < qreal >, QVector < qreal > >  DataSample;

/*!
 * \brief Dataset
 */
typedef QVector< DataSample > Dataset;


constexpr quint32 defaultPercent = 30;

/*!
 * \brief SplittedDataSet
 */
typedef QVector < QVector < qreal > > SplittedDataSet;

class Preprocessor
{
public:
    static Preprocessor & getInstance();

    const SplittedDataSet & getTrainingData() const;
    const SplittedDataSet & getTrainingResult() const;
    const SplittedDataSet & getTestingData() const;
    const SplittedDataSet & getTestingResult() const;

    void setInputFileName(const QString & fileName);
    void setOutputFileName(const QString & fileName);
    void setPercentageOfTest( const quint32 value );

    void saveFile( const Dataset & data);
    void flush();

private:
    Preprocessor();

    void splitData();
    void readFile();
    void writeFile( const Dataset & data);

private:
    SplittedDataSet trainingData;
    SplittedDataSet trainingResults;
    SplittedDataSet testingData;
    SplittedDataSet testingResult;
    QString fileNameIn;
    QString fileNameOut;
    Dataset cache;
    quint32 percentageOfTest;
    QDateTime lastModified;
};

#ifdef TEST_MODE

#include <QtTest/QtTest>
#include <QObject>
class PreprocessorTest : public QObject {
    Q_OBJECT
private slots:
    void EmptyTest();
    void InitializationTest();
    void ProcessTest();
};
#endif

#endif // PREPROCESSOR_HPP
