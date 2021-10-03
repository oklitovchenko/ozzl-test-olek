TEMPLATE = app
TARGET = ozzl-test-olek-client

DESTDIR = ../../bin
MOC_DIR = ../../build/client
OBJECTS_DIR = ../../build/client


CONFIG += console
CONFIG -= app_bundle

QT += core
QT += network
QT -= gui

INCLUDEPATH += ./

# Input
SOURCES += main.cpp \
           ozzltcppeer.cpp

HEADERS += ozzltcppeer.h\
           ../common/ozzlprocommon.h