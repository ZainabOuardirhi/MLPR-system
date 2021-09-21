#-------------------------------------------------
#
# Project created by QtCreator 2020-06-29T09:58:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mascir_LPR
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
    vehicledetector.cpp \
    charactersdetector.cpp \
    firstwindow.cpp \
    videowindow.cpp \
    imagewindow.cpp \
    Hungarian.cpp \
    KalmanTracker.cpp \
    track.cpp

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += opencv

HEADERS += \
        mainwindow.h \
    vehicledetector.h \
    charactersdetector.h \
    firstwindow.h \
    videowindow.h \
    imagewindow.h \
    KalmanTracker.h \
    Hungarian.h \
    track.h

FORMS += \
        mainwindow.ui \
    firstwindow.ui \
    videowindow.ui \
    imagewindow.ui
