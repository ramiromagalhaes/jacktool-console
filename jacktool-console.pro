#-------------------------------------------------
#
# Project created by QtCreator 2013-08-17T15:13:08
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = jacktool-console
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
           patchtool.cpp \
           patchextractor.cpp \
           ../jacktool-common/markings.cpp

HEADERS  += patchtool.h \
            patchextractor.h\
           ../jacktool-common/markings.h \
           ../jacktool-common/Rectangle.h

LIBS += -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lboost_serialization \
        -lboost_filesystem \
        -lboost_system \
        -lboost_program_options

