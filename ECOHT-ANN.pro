#-------------------------------------------------
#
# Project created by QtCreator 2013-04-23T00:45:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

debug {
    QT       += testlib
    CONFIG   += console
    QMAKE_CXX = ccache g++-4.7
#    QMAKE_CXX = g++-4.7
}

TARGET = ECOHT-ANN
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x -Wno-unused-local-typedefs

SOURCES += main.cpp\
        mainwindow.cpp \
    Neuron.cpp \
    Layer.cpp \
    Network.cpp \
    Sample.cpp

HEADERS  += \
    mainwindow.hpp \
    Neuron.hpp \
    Layer.hpp \
    Defines.hpp \
    Network.hpp \
    Sample.hpp \
    Helpers.hpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/qwt-6.0.2/lib/release/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/qwt-6.0.2/lib/debug/ -lqwt
else:unix:!macx: LIBS += -L$$PWD/../../../../usr/local/qwt-6.1.0-rc3/lib/ -lqwt

INCLUDEPATH += $$PWD/../../../../usr/local/qwt-6.1.0-rc3/include
DEPENDPATH += $$PWD/../../../../usr/local/qwt-6.1.0-rc3/include

FORMS += \
    mainwindow.ui
