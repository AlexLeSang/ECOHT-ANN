#include <QApplication>
#include "mainwindow.hpp"
#include "Defines.hpp"


// TODO All reports should be added to the Report Directory
// TODO The reports content should be placed on the main page of the site

#ifdef TEST_MODE
#include "Facade.hpp"

int main()
{
    Facade::getInstance();

//    NeuronTest neuronTest;
//    QTest::qExec(&neuronTest);

//    LayerTest layerTest;
//    QTest::qExec(&layerTest);

    NetworkTest networkTest;
    networkTest.test();
//    QTest::qExec(&networkTest);

//    PreprocessorTest preprocessorTest;
//    QTest::qExec(&preprocessorTest);

    return 0;
}

#else
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
#endif
