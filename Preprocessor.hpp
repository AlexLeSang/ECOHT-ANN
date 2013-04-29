#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <QtCore>
#include <Exceptions.hpp>


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

    void setFilenameIn(const QString & s);
    void setFileNameOut(const QString & s) { fileNameOut = s; }
    void setPercentageOfTest( const quint32 p ) { percentageOfTest = p; }
    void saveFile() { writeFile(); }
    void flush() { readFile(); splitData(); }
private:
    void splitData();
    //const Dataset readFile(const QString& fileName );
    //void writeFile( const QString & fileName, const Dataset & data, const quint32 numb );
    void readFile();
    void writeFile();
    SplittedDataSet trainingData;
    SplittedDataSet trainingResults;
    SplittedDataSet testingData;
    SplittedDataSet testingResult;
    QString fileNameIn;
    QString fileNameOut;
    Dataset cache;
    quint32 percentageOfTest;
};

#endif // PREPROCESSOR_HPP
