#include "Preprocessor.hpp"
#include "Facade.hpp"
/*!
 * \brief Preprocessor::Preprocessor
 */
Preprocessor::Preprocessor() : percentageOfTest( defaultPercent ) {}

/*!
 * \brief Preprocessor::getInstance
 * \return
 */
Preprocessor &Preprocessor::getInstance()
{
    static Preprocessor instance;
    return instance;
}

/*!
 * \brief Preprocessor::getTrainingData
 * \return
 */
const SplittedDataSet &Preprocessor::getTrainingData() const
{
    return trainingData;
}

/*!
 * \brief Preprocessor::getTrainingResult
 * \return
 */
const SplittedDataSet &Preprocessor::getTrainingResult() const
{
    return trainingResults;
}

/*!
 * \brief Preprocessor::getTestingData
 * \return
 */
const SplittedDataSet &Preprocessor::getTestingData() const
{
    return testingData;
}

/*!
 * \brief Preprocessor::getTestingResult
 * \return
 */
const SplittedDataSet &Preprocessor::getTestingResult() const
{
    return testingResult;
}

/*!
 * \brief Preprocessor::readFile
 */
void Preprocessor::readFile()
{
    QFile inputFile( fileNameIn );
    if ( !inputFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        throw FileOpeningErrorException( "Preprocessor::readFile" );
    }

    QTextStream inputStream( &inputFile );
    qint32 numberOfInputs;
    qint32 numberOfOutputs;
    inputStream >> numberOfInputs;
    inputStream.readLine();

    cache.clear();
    while ( !inputStream.atEnd() ) {
        QString nextLine;
        nextLine = inputStream.readLine().trimmed().simplified();
        if ( nextLine.isEmpty() ){
            continue;
        }
        QTextStream line( &nextLine );
        QVector < qreal > inputParameters( numberOfInputs );

        for ( auto itInputs = inputParameters.begin(); itInputs != inputParameters.end(); ++itInputs ) {
            qreal tmp;
            if( line.atEnd() ) {
                throw UnexpectedFileEndException( "Preprocessor::readFile" );
            }
            line >> tmp;

            (*itInputs) = tmp;
        }

        QVector < qreal > outputParameters;
        while( !line.atEnd() ) {
            qreal tmp;
            line >> tmp;
            outputParameters.append( tmp );

        }

        cache.append( DataSample( inputParameters, outputParameters ) );
    }

    numberOfOutputs = cache.first().second.size();
    Facade::getInstance().setInitialLayerInfo( LayerDescription ( numberOfInputs, numberOfOutputs ) );

}

/*!
 * \brief Preprocessor::writeFile
 * \param data
 */
void Preprocessor::writeFile( const Dataset & data )
{
    QFile outputFile( fileNameOut );
    if ( !outputFile.open( QIODevice::ReadWrite | QIODevice::Text ) ) {
        throw FileOpeningErrorException( "Preprocessor::writeFile" );
    }

    QTextStream outputStream( &outputFile );
    quint32 numberOfInputs = data.first().first.size();
    outputStream << numberOfInputs << '\n';

    for ( auto it = data.constBegin(); it != data.constEnd(); ++it ) {

        for ( auto itInputs = (*it).first.constBegin(); itInputs != (*it).first.constBegin() + numberOfInputs; ++itInputs ) {
            outputStream << (*itInputs) << ' ';
        }

        for ( auto itOutputs = (*it).second.constBegin(); itOutputs !=(*it).second.constEnd(); ++itOutputs ) {
            outputStream << (*itOutputs) << ' ';
        }

        outputStream << '\n';
    }
}

/*!
 * \brief Preprocessor::setInputFileName
 * \param fileName
 */
void Preprocessor::setInputFileName(const QString & fileName )
{
    // TODO handle an exception
    // TODO parse and get number of inputs
    // TODO call method of the facade and set number of inputs
    // TODO call method of the facade and set number of outputs
    if ( fileName == fileNameIn ) {
        QFileInfo f( fileNameIn );
        if( !f.exists() ){
            throw FileNotExistsException( "Preprocessor::setFileName" );
        }
        if ( f.lastModified() != lastModified ){
            lastModified = f.lastModified();
            readFile();
            splitData();
        }
        return;
    }
    else {
        QFileInfo f( fileName );
        if( !f.exists() ){
            throw FileNotExistsException( "Preprocessor::setFileName" );
        }
        lastModified = f.lastModified();
        fileNameIn = fileName;
        readFile();
        splitData();
    }
}

/*!
 * \brief Preprocessor::setOutputFileName
 * \param fileName
 */
void Preprocessor::setOutputFileName(const QString &fileName)
{
    fileNameOut = fileName;
}

/*!
 * \brief Preprocessor::setPercentageOfTest
 * \param value
 */
void Preprocessor::setPercentageOfTest(const quint32 value)
{
    percentageOfTest = value;
}

/*!
 * \brief Preprocessor::saveFile
 * \param data
 */
void Preprocessor::saveFile(const Dataset &data)
{
    writeFile(data);
}

/*!
 * \brief Preprocessor::flush
 */
void Preprocessor::flush()
{
    readFile();
    splitData();
}

/*!
 * \brief Preprocessor::splitData
 */
void Preprocessor::splitData()
{
    qint32 trainingNumber = static_cast< qint32 >( cache.size() * ( ( 100 - percentageOfTest ) / 100.) );

    trainingData.clear();
    trainingResults.clear();
    testingData.clear();
    testingResult.clear();

    for ( auto it = cache.constBegin(); it != cache.constEnd(); ++it ){
        if( it - cache.constBegin() < trainingNumber ){
            trainingData.append( (*it).first );
            trainingResults.append( (*it).second );
        }
        else {
            testingData.append( (*it).first );
            testingResult.append( (*it).second );
        }
    }

    Q_ASSERT( trainingData.size() == trainingResults.size() );
    Q_ASSERT( testingData.size() == testingResult.size() );

}

#ifdef TEST_MODE
void PreprocessorTest::EmptyTest()
{
    QCOMPARE(Preprocessor::getInstance().getTestingData().size(), 0);
}

void PreprocessorTest::InitializationTest()
{
    Preprocessor::getInstance().setInputFileName( "/home/dan/1.dat" );
    QCOMPARE(Preprocessor::getInstance().getTestingData().size(), 3);
}

void PreprocessorTest::ProcessTest()
{
    QCOMPARE(Preprocessor::getInstance().getTrainingData().size(),7);
}
#endif
