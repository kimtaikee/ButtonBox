CONFIG += c++11
contains(QT, core): QT += concurrent
contains(QT, gui): QT += widgets

include(../src/buttonbox.pri)

DESTDIR = ../build
TARGET = testButtonBox
TEMPLATE = app

FORMS += \
    testbuttonbox.ui

HEADERS += \
    testbuttonbox.h

SOURCES += \
    testbuttonbox.cpp \
    main.cpp
