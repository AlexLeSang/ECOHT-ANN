#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
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
