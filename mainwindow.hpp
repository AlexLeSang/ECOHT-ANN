#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtCore>
#include <qwt.h>
#include <qwt_plot_curve.h>
#include <Preprocessor.hpp>

struct LayerStruct{
    QLabel* label;
    QSpinBox* neuronsNumber;
    QSpinBox* inputsNumber;
    QSpacerItem* spacer;
};

typedef QPair < quint32, quint32 > LayerDescription;
typedef QVector < LayerDescription > LayersInfo;
typedef QVector < LayerStruct > LayersGUI;

namespace Ui{
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

signals:
    void setInputFileName( const QString fileName );
    void setOutputFileName( const QString fileName );
    void setLayerDescription( const QVector<QPair<quint32,quint32> > info );

private slots:
    void aboutSlot();
    void openInputFile();
    void openOutputFile();
    void displayResults();
    void saveImage();
    void changeLayers( int );
    void setInitialLayerInfo( const LayerDescription & );
private:
    void showResults( const Dataset& data );
    LayersInfo getLayerInfo();

private:
    Ui::MainWindow *ui;
    QwtPlotCurve curve;
    LayersGUI layers;
    qint32 currLayerNumber;
};

#endif // MAINWINDOW_H
