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
    QSpacerItem* fSpacer;
    QSpinBox* neuronsNumber;
    QSpacerItem* sSpacer;
    QComboBox* activationFunction;
};

typedef QPair < qint32, quint32 > LayerInfo;
typedef QVector < LayerInfo > LayersInfo;
typedef QVector < LayerStruct > LayersGUI;

namespace Ui{

class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QwtPlotCurve *curve;
    LayersGUI layers;
    qint32 currLayerNumber;
    void showResults(const Dataset& data);
    LayersInfo getLayerInfo();
public slots:
    void plot(QVector<QPointF>&);
    void saveImage();
    void changeLayers(int);
};

#endif // MAINWINDOW_H
