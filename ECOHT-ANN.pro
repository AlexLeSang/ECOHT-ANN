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
    QMAKE_CXX = g++
#    QMAKE_CXX = ccache g++-4.7
}

TARGET = ECOHT-ANN
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x -Wno-unused-local-typedefs

SOURCES += main.cpp\
    mainwindow.cpp \
    Neuron.cpp \
    Layer.cpp \
    Network.cpp \
    Sample.cpp \
    Facade.cpp \
    Preprocessor.cpp

HEADERS  += \
    mainwindow.hpp \
    Neuron.hpp \
    Layer.hpp \
    Defines.hpp \
    Network.hpp \
    Sample.hpp \
    Helpers.hpp \
    Facade.hpp \
    Preprocessor.hpp \
    Exceptions.hpp

LIBS += -L/usr/lib -lqwt

INCLUDEPATH += /usr/include/qwt
DEPENDPATH += /usr/include/qwt


FORMS += \
    mainwindow.ui

OTHER_FILES += \
    data1.dat \
    data.dat \
    data2.dat \
    plotres.m \
    data4.dat \
    simple_data.dat \
    parabolic_data.dat
