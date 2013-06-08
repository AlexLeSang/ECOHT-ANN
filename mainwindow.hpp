#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <qwt/qwt.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_zoomer.h>
//#include <Preprocessor.hpp>


/*!
 * \brief DataSample
 */
typedef QPair < QVector < qreal >, QVector < qreal > >  DataSample;

/*!
 * \brief Dataset
 */
typedef QVector< DataSample > Dataset;

/*!
 * \brief The LayerStruct struct
 */
struct LayerStruct{
    QLabel* label;
    QSpinBox* neuronsNumber;
    QSpinBox* inputsNumber;
    QSpacerItem* spacer;
};

/*!
 * \brief LayerDescription
 */
typedef QPair < quint32, quint32 > LayerDescription;

/*!
 * \brief LayersInfo
 */
typedef QVector < LayerDescription > LayersInfo;

/*!
 * \brief LayersGUI
 */
typedef QVector < LayerStruct > LayersGUI;

namespace Ui{
    class MainWindow;
}

/*!
 * \brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

signals:
    void setInputFileName( const QString fileName );
    void setOutputFileName( const QString fileName );
    void setLayerDescription( const QVector<QPair<quint32,quint32> > info );
    void setAlpha( double );
    void setBeta( double );

private slots:
    void start();
    void openInputFile();
    void openOutputFile();
    void displayResults();
    void saveImage();
    void changeLayers( int );
    void setInitialLayerInfo( const LayerDescription & );
    void sendAlpha();
    void sendBeta();

private:
    void showResults( const Dataset& data );
    LayersInfo getLayerInfo();

private:
    Ui::MainWindow *ui;
    QwtPlotZoomer *zoom;
    QwtPlotCurve curve;
    LayersGUI layers;
    qint32 currLayerNumber;
};

#endif // MAINWINDOW_H
