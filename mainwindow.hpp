#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <qwt.h>
#include <qwt_plot_curve.h>
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
public slots:
    void plot(QVector<QPointF>&);
    void saveImage();
};

#endif // MAINWINDOW_H
