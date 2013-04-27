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

INCLUDEPATH += /usr/local/qwt-6.0.3-svn/include

LIBS += /usr/local/qwt-6.0.3-svn/lib/libqwt.so.6.0.3

FORMS += \
    mainwindow.ui
