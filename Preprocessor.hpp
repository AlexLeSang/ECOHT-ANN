#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <QtCore>

#include "Exceptions.hpp"

#include "Defines.hpp"

typedef QPair < QVector < qreal >, QVector < qreal > >  DataSample;
typedef QVector< DataSample > Dataset;
typedef QVector < QVector < qreal > > SplittedDataSet;

class Preprocessor {
public:
    static Preprocessor & getInstance();
    const SplittedDataSet & getTrainingData() { return trainingData; }
    const SplittedDataSet & getTrainingResult() { return trainingResults; }
    const SplittedDataSet & getTestingData() { return testingData; }
    const SplittedDataSet & getTestingResult() { return testingResult; }

    void setInputFileName(const QString & s);
    void setOutputFileName(const QString & s) { fileNameOut = s; }
    void setPercentageOfTest( const quint32 p ) { percentageOfTest = p; }
    void saveFile( const Dataset & data) { writeFile(data); }
    void flush() { readFile(); splitData(); }
    const Dataset & getData(){ return cache; }
private:
    Preprocessor();
    Preprocessor( const Preprocessor &);
    void splitData();
    void readFile();
    void writeFile( const Dataset & data);
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
