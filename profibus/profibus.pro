#-------------------------------------------------
#
# Project created by QtCreator 2018-06-21T14:26:50
#
#-------------------------------------------------

QT       += core gui
QT       +=xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = profibus
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dp_device.cpp \
    block.cpp \
    pb.cpp \
    tb.cpp \
    fb.cpp

HEADERS += \
        mainwindow.h \
    dp_device.h \
    block.h \
    pb.h \
    tb.h \
    fb.h

FORMS += \
        mainwindow.ui
        
QMAKE_CXXFLAGS += -std=c++11	# enables C++11
