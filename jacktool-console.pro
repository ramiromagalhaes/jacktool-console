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
           extract_patch.cpp \
           ../jacktool-common/markings.cpp \
    patchextractor.cpp

HEADERS  += patchtool.h \
           extract_patch.h \
           ../jacktool-common/markings.h \
           ../jacktool-common/Rectangle.h \
    patchextractor.h



unix {
    LIBS += -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lboost_serialization \
            -lboost_filesystem \
            -lboost_system \
            -lboost_program_options
}
