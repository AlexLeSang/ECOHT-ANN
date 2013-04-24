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
    QMAKE_CXX = ccache g++
}

TARGET = ECOHT-ANN
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

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
    Sample.hpp

