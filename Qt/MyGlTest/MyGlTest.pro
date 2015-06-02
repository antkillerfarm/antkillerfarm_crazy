#-------------------------------------------------
#
# Project created by QtCreator 2014-07-18T13:36:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyGlTest
TEMPLATE = app


SOURCES += main.cpp\
        window.cpp \
    threadgl.cpp \
    glwidget.cpp \
    threadtime.cpp \
    threaddraw.cpp

HEADERS  += window.h \
    threadgl.h \
    glwidget.h \
    threadtime.h \
    threaddraw.h
QT += opengl widgets
