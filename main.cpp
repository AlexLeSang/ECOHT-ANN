#include <QApplication>

#include "mainwindow.hpp"

#include "Defines.hpp"

#ifdef USE_MAIN
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
#endif
