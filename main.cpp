#include <QApplication>
#include "mainwindow.hpp"
#include "Defines.hpp"

#ifdef TEST_MODE
#include "Facade.hpp"

int main(int argc, char *argv[]) {
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    Facade::getInstance();

    /*
    NeuronTest neuronTest;
    QTest::qExec(&neuronTest);

    LayerTest layerTest;
    QTest::qExec(&layerTest);

    PreprocessorTest preprocessorTest;
    QTest::qExec(&preprocessorTest);
    */

    NetworkTest networkTest;
    QTest::qExec(&networkTest);

    return 0;
}

#else
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
#endif
