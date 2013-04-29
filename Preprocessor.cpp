#include "Preprocessor.hpp"

/*!
 * \brief Preprocessor::Preprocessor
 */
Preprocessor::Preprocessor() : percentageOfTest(30) {}

/*!
 * \brief Preprocessor::getInstance
 * \return
 */
Preprocessor &Preprocessor::getInstance() {
    static Preprocessor instance;
    return instance;
}

/*!
 * \brief Preprocessor::readFile
 * \param fileName
 * \return
 */
void Preprocessor::readFile() {
    QFile inputFile( fileNameIn );
    if ( !inputFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        throw FileOpeningErrorException( "Preprocessor::readFile" );
    }

    QTextStream inputStream( &inputFile );
    qint32 numberOfInputs;
    inputStream >> numberOfInputs;
    inputStream.readLine();

    cache.clear();
    while ( !inputStream.atEnd() ) {
        QString nextLine;
        nextLine = inputStream.readLine();
        QTextStream line( &nextLine );
        QVector < qreal > inputParameters( numberOfInputs );

        for ( auto itInputs = inputParameters.begin(); itInputs != inputParameters.end(); ++itInputs ) {
            qreal tmp;
            line >> tmp;
            if( line.atEnd() ) {
                throw UnexpectedFileEndException( "Preprocessor::readFile" );
            }
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
}

/*!
 * \brief Preprocessor::writeFile
 * \param fileName
 * \param data
 * \param numberOfInputs
 */
void Preprocessor::writeFile( const Dataset & data ) {
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
 * \brief Preprocessor::setFilenameIn
 * \param s
 */
void Preprocessor::setFilenameIn( const QString & s ) {
    if ( s == fileNameIn ) {
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
        QFileInfo f( fileNameIn );
        if( !f.exists() ){
            throw FileNotExistsException( "Preprocessor::setFileName" );
        }
        lastModified = f.lastModified();
        fileNameIn = s;
        readFile();
        splitData();
    }
}

/*!
 * \brief Preprocessor::splitData
 */
void Preprocessor::splitData() {
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
}

#ifdef TEST_MODE
void PreprocessorTest::EmptyTest() {
    QCOMPARE(Preprocessor::getInstance().getTestingData().size(), 0);
}

void PreprocessorTest::InitializationTest() {
    Preprocessor::getInstance().setFilenameIn( "1.dat" );
    QCOMPARE(Preprocessor::getInstance().getTestingData().size(), 7);
}

void PreprocessorTest::ProcessTest() {
    QCOMPARE(Preprocessor::getInstance().getTrainingData().size(),3);

}
#endif
