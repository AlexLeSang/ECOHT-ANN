#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    currLayerNumber = 0;

    ui->setupUi(this);

    ui->qwtPlot->setTitle("Error");
    ui->qwtPlot->setAxisTitle(ui->qwtPlot->xBottom, "Epoch");
    ui->qwtPlot->setAxisTitle(ui->qwtPlot->yLeft,"Error");

    QPen pen = QPen(Qt::red);
    curve = new QwtPlotCurve;
    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setPen(pen);
    curve->attach(ui->qwtPlot);

    connect(ui->saveImageButton,SIGNAL(clicked()),this,SLOT(saveImage()));
    connect(ui->numberOfLayers,SIGNAL(valueChanged(int)),this,SLOT(changeLayers(int)));
}

MainWindow::~MainWindow()
{

}

void MainWindow::plot(QVector<QPointF>& vec){
    QwtPointSeriesData * data = new QwtPointSeriesData(vec);
    curve->setData(data);
    ui->qwtPlot->replot();
}


void MainWindow::saveImage(){
    QString filename = QFileDialog::getSaveFileName(this,tr("Save Image"), "image.png", tr("Image Files (*.png *.jpg *.bmp)"));
    QPixmap pixmap = QPixmap::grabWidget(ui->qwtPlot);

    pixmap.save(filename);
}

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
            (*it).activationFunction = new QComboBox();

            //(*it).activationFunction->addItem( "Linear" );
            //(*it).activationFunction->addItem( "Sigma" );

            ui->layersGrid->addWidget( (*it).label, num, 0, 1, 1 );
            ui->layersGrid->addWidget( (*it).neuronsNumber, num, 1, 1, 1 );
            ui->layersGrid->addWidget( (*it).activationFunction, num, 2, 1, 1 );
            ui->layersGrid->addItem( (*it).spacer, num, 3 );
        }

        if( num <= currLayerNumber && num > layersNumber ){
            ui->layersGrid->removeWidget( (*it).label );
            ui->layersGrid->removeWidget( (*it).neuronsNumber );
            ui->layersGrid->removeItem( (*it).spacer );
            ui->layersGrid->removeWidget( (*it).activationFunction );

            delete (*it).label;
            delete (*it).neuronsNumber;
            delete (*it).spacer;
            delete (*it).activationFunction;
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

LayersInfo MainWindow::getLayerInfo(){
    LayersInfo result;

    for ( auto it = layers.constBegin(); it != layers.constEnd(); ++it ){
        result.append( LayerInfo((*it).neuronsNumber->value(), (*it).activationFunction->currentIndex() ) );
    }

    return result;
}
