#include "Network.hpp"

#include "Layer.hpp"

#include <QDebug>

#include "Facade.hpp"

void Network::run() {
    initLayers();
    training( trainingData, trainigResult );
    testing( testingData, testingResult );
    Facade::getInstance().processFinished();
}

void Network::training(const Data &dataSet, const Result &desiredResult)
{
    Q_ASSERT( dataSet.size() == desiredResult.size() );
    Q_ASSERT( maxNumberOfEpoch >= 1 );
    quint32 epochCounter = 0;
    qreal achievedAccuracy = 1.0;

    errorList.clear(); // List contains all errors
    stopFlag = false;


    /*
    while ( true ) {

        if ( epochCounter >= maxNumberOfEpoch ) break;
        if ( achievedAccuracy <= accuracy ) break;
        if ( stopFlag ) break;

        qreal networkError = 0.0;

        qDebug() << "Network:";
        std::for_each( layers.constBegin(), layers.constEnd(), []( const Layer & layer ) {
            qDebug() << "=============================";
            std::for_each( layer.getNeurons().constBegin(), layer.getNeurons().constEnd(), [] ( const Neuron & neuron ) {
                qDebug() << neuron.getWeights();
                qDebug() << "+++++++++++++++++++++++++";
            }  );
        } );

        // Process all dataset
        QList< IntermidResults > intermidResultsList;
        std::for_each( dataSet.constBegin(), dataSet.constEnd(), [&]( Data::const_reference & dataSample ) {
            intermidResultsList.append( processInNetwork( dataSample ) );
        } );

        // Calculate network error
        for ( auto index = 0; index < desiredResult.size(); ++ index ) {
            // Obtain results for current sample
            const QVector < qreal > & networkOutput = intermidResultsList.at( index ).last();
            // Obtain desiredResult for current sample
            const QVector < qreal > & currentDesiredResult = desiredResult.at( index ).getData();

            Q_ASSERT( networkOutput.size() == currentDesiredResult.size() );
            networkError = networkError + calculateNetworkError( currentDesiredResult, networkOutput ) / desiredResult.size() ;
        }

        // Calculate gradient for last layer
        QList< QVector < qreal > > gradientList;
        for ( auto index = 0; index < desiredResult.size(); ++ index ) {
            // Obtain results for current sample
            const QVector < qreal > & networkOutput = intermidResultsList.at( index ).last();
            // Obtain desiredResult for current sample
            const QVector < qreal > & currentDesiredResult = desiredResult.at( index ).getData();

            Q_ASSERT( networkOutput.size() == currentDesiredResult.size() );
            const auto currentGradient = calculateOutputGradient( currentDesiredResult, networkOutput );
            gradientList.append( currentGradient );
        }

        // Calculate average gradient
        QVector < qreal > averageGradient = gradientList.first();
        std::for_each( averageGradient.begin(), averageGradient.end(), [&] ( qreal & val ) { val /= gradientList.size(); } );
        std::for_each( gradientList.constBegin() + 1, gradientList.constEnd(), [&] ( const QVector< qreal > & currentGradient ) {
            Q_ASSERT( averageGradient.size() == currentGradient.size() );
            for ( auto gradientIndex = 0; gradientIndex < averageGradient.size(); ++ gradientIndex ) {
                averageGradient[ gradientIndex ] = averageGradient[ gradientIndex ] + currentGradient[ gradientIndex ] / gradientList.size();
            }
        } );
        qDebug() << "averageGradient: " << averageGradient;

        qDebug() << "intermidResultsList:";
        std::for_each( intermidResultsList.constBegin(), intermidResultsList.constEnd(), []( const IntermidResults & intRes ) {
            std::for_each( intRes.constBegin(), intRes.constEnd(), []( const QVector < qreal > & layerOutput ) {
                qDebug() << layerOutput;
            } );
            qDebug();
        } );


        // Calculate average intermid output
        IntermidResults averageIntermidResults = intermidResultsList.first();
        std::for_each( averageIntermidResults.begin(), averageIntermidResults.end(), [&] ( QVector < qreal > & layerOut ) {
            std::for_each( layerOut.begin(), layerOut.end(), [&] ( qreal & val ) { val /= gradientList.size(); } );
        } );
        std::for_each( intermidResultsList.constBegin() + 1, intermidResultsList.constEnd(), [&] ( const IntermidResults & currResult ) {
            // Intermid result
            {
                Q_ASSERT( averageIntermidResults.size() == currResult.size() );
                for ( auto intermidIndex = 0; intermidIndex < averageIntermidResults.size(); ++ intermidIndex ) {
                    // Layer
                    {
                        QVector < qreal > & layerOutput = averageIntermidResults[ intermidIndex ];
                        const QVector < qreal > & currLayerOutput = currResult[ intermidIndex ];
                        Q_ASSERT( layerOutput.size() == currLayerOutput.size() );

                        for ( auto neuronIndex = 0; neuronIndex < currLayerOutput.size(); ++ neuronIndex ) {
                            // Output
                            layerOutput[ neuronIndex ] = layerOutput[ neuronIndex ] + currLayerOutput[ neuronIndex ] / intermidResultsList.size();
                        }
                    }
                }
            }

        } );

        qDebug() << "averageIntermidResults";
        std::for_each( averageIntermidResults.constBegin(), averageIntermidResults.constEnd(), []( const QVector < qreal > & layerOutput ) {
            qDebug() << layerOutput;
        } );


        // Calculate delta
//        QVector Layers < QVector Neurons < qreal delta > > deltaVector( layers.size() );
        QVector < QVector < qreal > > deltaVector( layers.size() );
        // Init last layer's delta
        deltaVector.last().resize( averageGradient.size() );
        std::copy( averageGradient.constBegin(), averageGradient.constEnd(), deltaVector.last().begin() );

        for ( auto layerIndex = deltaVector.size() - 2; layerIndex >= 0; -- layerIndex ) {

            QVector < qreal > & currentLayerDelta = deltaVector[ layerIndex ];
            const QVector < qreal > & prevLayerDelta = deltaVector[ layerIndex + 1 ];

            currentLayerDelta.resize( layers.at( layerIndex ).getNeurons().size() );

            for ( auto neuronIndex = 0; neuronIndex < currentLayerDelta.size(); ++ neuronIndex ) {

                qreal & delta = currentLayerDelta[ neuronIndex ];

                const qreal & output = averageIntermidResults.at( layerIndex ).at( neuronIndex );

                qreal sum = 0.0;
                // Calculate sum of connected delta and weights
                {
                    for ( auto prevLayerNeuronIndex = 0; prevLayerNeuronIndex < layers.at( layerIndex + 1 ).getNeurons().size(); ++ prevLayerNeuronIndex ) {

                        const Neuron & prevLayerNeuron = layers.at( layerIndex + 1 ).getNeurons()[ prevLayerNeuronIndex ];
                        const qreal & prevLayerNeuronDelta = prevLayerDelta[ prevLayerNeuronIndex ];

                        const qreal & correspondingWeight = prevLayerNeuron.getWeights().at( neuronIndex );

                        sum += prevLayerNeuronDelta * correspondingWeight;
                    }
                }
                delta = output * ( 1 - output ) * sum;
            }

        }

        qDebug() << "\ndeltaVector:";
        std::for_each( deltaVector.constBegin(), deltaVector.constEnd(), []( const QVector < qreal > & layerDelta ) {
            qDebug() << "------------------------------------";
            qDebug() << layerDelta;
        } );
        qDebug() << "------------------------------------";


        // Update weights
        for ( auto layerIndex = 0; layerIndex < deltaVector.size(); ++ layerIndex ) {

            QVector < Neuron > & currentLayersNeurons = layers[ layerIndex ].getNeurons();

            const QVector < qreal > & currentLayerDeltas = deltaVector[ layerIndex ];

            for ( auto neuronIndex = 0; neuronIndex < currentLayersNeurons.size(); ++ neuronIndex ) {

                Neuron & currentNeuron = currentLayersNeurons[ neuronIndex ];

                const qreal & currentDelta = currentLayerDeltas[ neuronIndex ];

                const qreal & currentIntermidResult = averageIntermidResults.at( layerIndex ).at( neuronIndex );

                for ( auto weightIndex = 0; weightIndex < currentNeuron.getWeights().size(); ++ weightIndex ) {
                    qreal & currentWeight = currentNeuron.getWeights()[ weightIndex ];
                    currentWeight = currentWeight + currentDelta * currentIntermidResult;
                }

            }
        }


        qDebug() << "Epoch# " << epochCounter << " error:" << networkError << '\n';

        if ( epochCounter > 4 ) {
            if ( errorList.last() <= networkError ) break;
        }

        errorList.append( networkError ); // Save current error
        achievedAccuracy = networkError; // Get last error
        ++ epochCounter;

    }

    */


    //    /*
    while ( true ) {

        if ( epochCounter >= maxNumberOfEpoch ) break;
        if ( achievedAccuracy <= accuracy ) break;
        if ( stopFlag ) break;

        qreal networkError = 0.0;

        //        QVector< QVector < QVector < qreal > > > delta( desiredResult.size() ); // Result_size < Layer < Neuron > >
        //        std::for_each( delta.begin(), delta.end(), [&]( QVector < QVector < QVector < qreal > > > & layersDelta ) {
        //            layersDelta.resize( layers.size() );
        //            auto index = 0;
        //            std::for_each( layersDelta.begin(), layersDelta.end(), [&]( QVector < QVector < qreal > > & neuronDelta ) {
        //                neuronDelta.resize( layers.at( index ).getNeurons().size() );
        //            } );
        //        } );


        /*
        qDebug() << "Network:";
        std::for_each( layers.constBegin(), layers.constEnd(), []( const Layer & layer ) {
            qDebug() << "=============================";
            std::for_each( layer.getNeurons().constBegin(), layer.getNeurons().constEnd(), [] ( const Neuron & neuron ) {
                qDebug() << neuron.getWeights();
                qDebug() << "+++++++++++++++++++++++++";
            }  );
        } );
        */

        // For each part of data
        auto desiredResultIt = desiredResult.begin();
        std::for_each( dataSet.constBegin(), dataSet.constEnd(), [&]( Data::const_reference dataSample ) {
            //            qDebug() << "==========================================================================";
            //            qDebug() << "dataSample: " << dataSample.getData();
            //            qDebug() << "desiredResult: " << (*desiredResultIt).getData();

            const QVector < QVector < qreal > > intermidResult = processInNetwork( dataSample );
            //            qDebug() << "obtainedResult: " << intermidResult.last();

            // Process data through the network
            /*
            qDebug() << "Process in network";
            for ( auto layerIndex = 0; layerIndex < intermidResult.size(); ++ layerIndex ) {
                qDebug() << intermidResult.at( layerIndex );
            }
            */

            // Calculate network error for this data and add it to error in current epoch
            const QVector< qreal > & obtainedResult = intermidResult.last(); // Get network result
            const QVector< qreal > & currentDesiredResult = (*desiredResultIt).getData();

            networkError += calculateNetworkError( currentDesiredResult, obtainedResult );
            //            qDebug() << "networkError: " << networkError;


            QVector < QVector < qreal > > deltaForCurrentSample( layers.size() ); // Layer < Neuron < delta > >
            for ( auto i = 0; i < deltaForCurrentSample.size(); ++ i ) {
                deltaForCurrentSample[ i ].resize( layers.at( i ).getNeurons().size() );
            }
            deltaForCurrentSample.last() = calculateOutputGradient( currentDesiredResult, obtainedResult );

            for ( auto gradIndex = 0; gradIndex < deltaForCurrentSample.last().size(); ++gradIndex ){
                QVector < qreal > & lastLayerNeuronWeights = layers.last().getNeurons()[gradIndex].getWeights();
                for ( auto weightIndex = 0; weightIndex < lastLayerNeuronWeights.size(); ++weightIndex){
                    qreal & currWeight = lastLayerNeuronWeights[ weightIndex ];
                    const qreal & currentGrad = deltaForCurrentSample.last()[ gradIndex ];
                    currWeight = currWeight + etha * currentGrad * obtainedResult[ gradIndex ];
                }
            }
            // Not last layers calculation
            for ( auto layerIndex = layers.size() - 2; layerIndex >= 0; -- layerIndex ) {

                QVector < qreal > & currDeltas = deltaForCurrentSample[ layerIndex ];

                const QVector < qreal > & prevDeltas = deltaForCurrentSample.at( layerIndex + 1 );

                QVector < Neuron > & currLayerNeurons = layers[ layerIndex ].getNeurons();
                const QVector < Neuron > & prevLayerNeurons = layers[ layerIndex + 1 ].getNeurons();

                Q_ASSERT( prevDeltas.size() == prevLayerNeurons.size() );

                for ( auto neuronIndex = 0; neuronIndex < currLayerNeurons.size(); ++ neuronIndex ) {
                    qreal sum = 0.0;

                    for ( auto prevLayerIndex = 0; prevLayerIndex < prevLayerNeurons.size(); ++ prevLayerIndex ) {
                        const Neuron & prevLevNeuron = prevLayerNeurons.at( prevLayerIndex );
                        const qreal & corrWeight = prevLevNeuron.getWeights().at( neuronIndex );
                        const qreal & prevNeuronDelta = prevDeltas.at( prevLayerIndex );
                        sum += corrWeight * prevNeuronDelta;
                    }

                    const qreal & currentNeuronRes = intermidResult.at( layerIndex ).at( neuronIndex );

                    qreal & currentNeuronDelta = currDeltas[ neuronIndex ];
                    currentNeuronDelta = currentNeuronRes * ( 1 - currentNeuronRes ) * sum;

                    QVector < qreal > & currNeuronWeights = currLayerNeurons[ neuronIndex ].getWeights();

                    const qreal & currResult = intermidResult.at( layerIndex ).at( neuronIndex );

                    // For each weight
                    for ( auto weightIndex = 0; weightIndex < currNeuronWeights.size(); ++ weightIndex ) {
                        currNeuronWeights[ weightIndex ] = currNeuronWeights[ weightIndex ] + etha * ( currentNeuronDelta * currResult );
                    }

                }
            }

            /*
            qDebug() << "deltaForCurrentSample:";
            for ( auto layerIndex = 0; layerIndex < deltaForCurrentSample.size(); ++ layerIndex ) {
                qDebug() << deltaForCurrentSample.at( layerIndex );
            }
            */

            // Update weights
            /*
            for ( auto layerIndex = 0; layerIndex < layers.size(); ++ layerIndex ) {
                const QVector < qreal > & currLevelDeltas = deltaForCurrentSample.at( layerIndex );
                QVector < Neuron > & currentLevelNeurons = layers[ layerIndex ].getNeurons();
                Q_ASSERT( currLevelDeltas.size() == currentLevelNeurons.size() );

                // For each neuron in layer
                for ( auto neuronIndex = 0; neuronIndex < currentLevelNeurons.size(); ++ neuronIndex ) {
                    const qreal & currDelta = currLevelDeltas.at( neuronIndex );
                    const qreal & currResult = intermidResult.at( layerIndex ).at( neuronIndex );
                    QVector < qreal > & currNeuronWeights = currentLevelNeurons[ neuronIndex ].getWeights();

                    // For each weight
                    for ( auto weightIndex = 0; weightIndex < currNeuronWeights.size(); ++ weightIndex ) {
//                        const qreal finalDelta = (1 - alpha) * etha * currDelta * currResult;
//                        currNeuronWeights[ weightIndex ] = currNeuronWeights[ weightIndex ] + finalDelta;
                        currNeuronWeights[ weightIndex ] = currNeuronWeights[ weightIndex ] + etha * (currDelta*currResult);
                    }
                }
            }
            */
            //            qDebug() << "==========================================================================";

            // Go to the next part of trainign data
            ++ desiredResultIt;
        } );

        //        qDebug() << "Epoch# " << epochCounter << " error:" << networkError << '\n';

        //        if ( epochCounter > 0 ) {
        //            if ( errorList.last() < networkError ) break;
        //        }

        errorList.append( networkError ); // Save current error
        achievedAccuracy = networkError; // Get last error
        ++ epochCounter;
    }
    //    */
    qDebug() << "epochCounter: " << epochCounter;

}



#ifdef TEST_MODE
void NetworkTest::ProcessTest()
{

    constexpr quint32 numberOfDataSamples = 20;
    //    constexpr quint32 numberOfInputs = 2;
    constexpr quint32 numberOfOutputs = 1;

    Network & network = Network::getInstance();
    QVector< LayerDescription > layersDesciption;
    //        layersDesciption.append(LayerDescription(numberOfOutputs, numberOfInputs));
    layersDesciption.append( LayerDescription( 12, numberOfInputs ) ); // Input
    layersDesciption.append( LayerDescription( numberOfOutputs, 12 ) ); // Ouput

    QVector < QVector < qreal > > data( numberOfDataSamples );
    auto index = 0;
        auto step = ( 1.0 / (numberOfDataSamples * 2) );
//    auto step = ( 1.0 / (numberOfDataSamples) );
    std::for_each( data.begin(), data.end(), [&]( QVector< qreal > & sample ) {
        sample.resize( numberOfInputs );
        std::for_each( sample.begin(), sample.end(), [&] ( qreal & val ) {
            val = (index)*step;
        } );
        index++;
    } );


    index = 0;
    step = ( 1.0 / (numberOfDataSamples ) );
    QVector < QVector < qreal > > result( data.size() );
    for ( auto dataIndex = 0; dataIndex < data.size(); ++ dataIndex ) {
        //        result[ dataIndex ].append( std::accumulate( data.at( dataIndex ).constBegin(), data.at( dataIndex ).constEnd(), 1.0, std::multiplies<qreal>() ) );
        result[ dataIndex ].append( std::accumulate( data.at( dataIndex ).constBegin(), data.at( dataIndex ).constEnd(), 0.0, std::plus<qreal>() ) );
    }

    //        std::random_shuffle( data.begin(), data.end() );
    //    std::random_shuffle( result.begin(), result.end() );

    //                qDebug() << "data: " << data;
    //                qDebug() << "result: " << result;
    //    return;

    network.setTestingData( data );
    network.setTestingResult( result );

    network.setTrainingData( data );
    network.setTrainigResult( result );

    network.setAccuracy( 1e-13 );

    network.setAlpha( 1e-3 );
    //    network.setBeta( 1e-15 );
    network.setEtha( 1.0 );
    network.setMaxNumberOfEpoch( 10000 );

    network.setLayersDescription( layersDesciption );
    network.run();
    //    /*
    auto testringRes = network.getObtainedTestingResult();
    qDebug() << "result: ";
    std::for_each( result.constBegin(), result.constEnd(), []( const QVector < qreal > & vect ) {
        qDebug() << vect;
    } );
    qDebug() << "testringRes: ";
    std::for_each( testringRes.constBegin(), testringRes.constEnd(), [] ( const Result::value_type & val ) {
        qDebug() << val.getData();
    } );
    //    */

    //    qDebug() << "Network error: " << network.getNetworkError();

}




#endif

