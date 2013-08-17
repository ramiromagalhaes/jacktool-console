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
           ../jacktool/markings.cpp \
           ../jacktool/extract_patch.cpp

HEADERS  += ../jacktool/markings.h \
           patchtool.h \
           ../jacktool/PatchExtractorConfiguration.h \
           ../jacktool/extract_patch.h



#INCLUDEPATH += /home/ramiro/workspace/jacktool/


unix {
    LIBS += -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lboost_serialization \
            -lboost_filesystem \
            -lboost_system
}
