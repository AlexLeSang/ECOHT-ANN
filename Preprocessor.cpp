#include "Preprocessor.hpp"

Preprocessor::Preprocessor()
{
}

Dataset Preprocessor::readFile( const QString & fileName ) {
    Dataset result;
    QFile inputFile( fileName );
    if ( !inputFile.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        throw FileOpeningErrorException( "Preprocessor::readFile" );
    }
    QTextStream inputStream( &inputFile );
    qint32 numberOfInputs;
    inputStream >> numberOfInputs;
    inputStream.readLine();
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

        result.append( SampleData( inputParameters, outputParameters ) );
    }
    return result;
}

void Preprocessor::writeFile( const QString & fileName, const Dataset & data, const quint32 numberOfInputs ) {
    QFile outputFile( fileName );
    if ( !outputFile.open( QIODevice::ReadWrite | QIODevice::Text ) ) {
        throw FileOpeningErrorException( "Preprocessor::writeFile" );
    }
    QTextStream outputStream( &outputFile );
    outputStream << numberOfInputs << '\n';
    for ( auto it = data.begin(); it != data.end(); ++it ) {

        for ( auto itInputs = (*it).first.constBegin(); itInputs != (*it).first.constBegin() + numberOfInputs; ++itInputs ) {
            outputStream << (*itInputs) << ' ';
        }

        for ( auto itOutputs = (*it).second.constBegin(); itOutputs !=(*it).second.constEnd(); ++itOutputs ) {
            outputStream << (*itOutputs) << ' ';
        }

        outputStream << '\n';
    }
}
//TODO Add Qt newlines
