#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "Facade.hpp"

/*!
 * \brief MainWindow::MainWindow
 * \param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    currLayerNumber = 0;

    ui->setupUi(this);

    ui->qwtPlot->setTitle("Error");
    ui->qwtPlot->setAxisTitle(ui->qwtPlot->xBottom, "Epoch");
    ui->qwtPlot->setAxisTitle(ui->qwtPlot->yLeft,"Error");

    ui->stopButton->setVisible( false );

    QPen pen = QPen( Qt::red );
    curve.setRenderHint( QwtPlotItem::RenderAntialiased );
    curve.setPen( pen );
    curve.attach( ui->qwtPlot );

    // TODO connect ui to mainwindow
    {
        QObject::connect( ui->saveImageButton, SIGNAL( clicked() ),
                          this, SLOT( saveImage() ) );
        QObject::connect( ui->numberOfLayers, SIGNAL( valueChanged(int) ),
                          this, SLOT( changeLayers(int) ) );
        // TODO connect open file button to openInputFile();
        // TODO connect save result button to openOutputFile();
    }

    // TODO connection ui to facade
    {
        QObject::connect( ui->startButton, SIGNAL( clicked() ),
                          &Facade::getInstance(), SLOT( startProcess() ) );
        QObject::connect( ui->stopButton, SIGNAL( clicked() ),
                          &Facade::getInstance(), SLOT( stopProcess() ) );
        // TODO connect controls
    }

    // TODO connection facade to main window
    {
        QObject::connect( &Facade::getInstance(), SIGNAL( processEnd() ),
                          this, SLOT( displayResults() ) );
    }


    // TODO connection main window to facade
    {
        QObject::connect( this, SIGNAL( setInputFileName(QString) ),
                          &Facade::getInstance(), SLOT( setInputFileName(QString) ) );
        QObject::connect( this, SIGNAL( setOutputFileName(QString) ),
                          &Facade::getInstance(), SLOT( setOutputFileName(QString) ) );
    }
}

/*!
 * \brief MainWindow::DisplayResults
 */
void MainWindow::displayResults() {
    ui->qwtPlot->detachItems( QwtPlotItem::Rtti_PlotCurve, false );
    ui->qwtPlot->replot();

    const auto errorVector = Facade::getInstance().getErrors();
    QVector < QPointF > points( errorVector.size() );
    quint32 counter = 0;
    auto pointsIt = points.begin();
    // Create a
    for ( auto errorIt = errorVector.constBegin(); errorIt != errorVector.constEnd(); ++ errorIt, ++ pointsIt, ++ counter ) {
        (*pointsIt) = QPointF( counter, (*errorIt) );
    }
    curve.setSamples( QPolygonF ( points ) );
    curve.attach( ui->qwtPlot );
    ui->qwtPlot->replot();

    // TODO get testing error results from the facade
    // TODO display error results
}


/*!
 * \brief MainWindow::saveImage
 */
void MainWindow::saveImage(){
    QString filename = QFileDialog::getSaveFileName(this,tr("Save Image"), "image.png", tr("Image Files (*.png *.jpg *.bmp)"));
    QPixmap pixmap = QPixmap::grabWidget(ui->qwtPlot);

    pixmap.save(filename);
}

/*!
 * \brief MainWindow::changeLayers
 * \param layersNumber
 */
void MainWindow::changeLayers( int layersNumber ){
    if( layersNumber > currLayerNumber ){
        layers.resize( layersNumber );
    }
    for ( auto it = layers.begin(); it != layers.end(); ++it ){
        qint32 num = it - layers.begin() + 1;
        if( num > currLayerNumber && num <= layersNumber ){
            (*it).label = new QLabel( QString::number( num ) );
            (*it).neuronsNumber = new QSpinBox();
            (*it).spacer = new QSpacerItem( 20, 20 );
            (*it).inputsNumber = new QSpinBox();


            if ( 1 != num ){
                (*it).inputsNumber->setReadOnly( true );
                connect((*( it - 1 )).neuronsNumber,SIGNAL(valueChanged( int )),(*it).inputsNumber,SLOT(setValue( int )));
            }

            ui->layersGrid->addWidget( (*it).label, num, 0, 1, 1 );
            ui->layersGrid->addWidget( (*it).neuronsNumber, num, 1, 1, 1 );
            ui->layersGrid->addWidget( (*it).inputsNumber, num, 2, 1, 1 );
            ui->layersGrid->addItem( (*it).spacer, num, 3 );
        }

        if( num <= currLayerNumber && num > layersNumber ){
            ui->layersGrid->removeWidget( (*it).label );
            ui->layersGrid->removeWidget( (*it).neuronsNumber );
            ui->layersGrid->removeItem( (*it).spacer );
            ui->layersGrid->removeWidget( (*it).inputsNumber );

            delete (*it).label;
            delete (*it).neuronsNumber;
            delete (*it).spacer;
            delete (*it).inputsNumber;
        }
        ui->layersGrid->update();
    }

    if( layersNumber < currLayerNumber ){
        layers.resize( layersNumber );
    }

    currLayerNumber = layersNumber;
}

void MainWindow::showResults(const Dataset & data){
    quint32 tableWidth = data.first().first.size() + data.first().second.size();
    quint32 inputLength = data.first().first.size();
    ui->resultsTable->setColumnCount( tableWidth );
    ui->resultsTable->setRowCount( data.size() );

    QStringList labelsList;
    for ( qint32 i=1; i <= data.first().first.size(); ++i ){
        labelsList.append( QString( "x" ) + QString::number( i ) );
    }

    for ( qint32 i=1; i <= data.first().second.size(); ++i ){
        labelsList.append( QString( "y" ) + QString::number( i ) );
    }

    ui->resultsTable->setHorizontalHeaderLabels( labelsList );

    for ( auto it = data.constBegin(); it != data.constEnd(); ++it ){
        for ( auto itInputs = (*it).first.constBegin(); itInputs != (*it).first.constEnd(); ++itInputs ){
            QTableWidgetItem *item = new QTableWidgetItem( QString::number( *itInputs ));
            ui->resultsTable->setItem( it - data.constBegin(), itInputs - (*it).first.constBegin(), item );
        }

        for ( auto itOutputs = (*it).second.constBegin(); itOutputs != (*it).second.constEnd(); ++itOutputs ){
            QTableWidgetItem *item = new QTableWidgetItem( QString::number( *itOutputs ));
            ui->resultsTable->setItem( it - data.constBegin(), itOutputs - (*it).second.constBegin() + inputLength, item );
        }
    }
}

/*!
 * \brief MainWindow::getLayerInfo
 * \return
 */
LayersInfo MainWindow::getLayerInfo(){
    LayersInfo result;

    for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it ){
        result.append( LayerInfo((*it).neuronsNumber->value(), (*it).inputsNumber->value() ) );
    }

    return result;
}

/*!
 * \brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow() {
    Facade::getInstance().stopProcess();
    delete ui;
}

/*!
 * \brief MainWindow::openFile
 */
void MainWindow::openInputFile() {
    const QString fileName = QFileDialog::getOpenFileName( this, tr("Open data file"), "", tr("Data files (*.dat)"));
    emit setInputFileName( fileName );
}

/*!
 * \brief MainWindow::openOutputFile
 */
void MainWindow::openOutputFile() {
    const QString fileName = QFileDialog::getOpenFileName( this, tr("Open result file"), "", tr("Result files (*.res)"));
    emit setOutputFileName( fileName );
}
