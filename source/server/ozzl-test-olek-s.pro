TEMPLATE = app
TARGET = ozzl-test-olekd

DESTDIR = ../../bin
MOC_DIR = ../../build/server
OBJECTS_DIR = ../../build/server

CONFIG += console
CONFIG -= app_bundle

QT += core
QT += network
QT -= gui

INCLUDEPATH += ./

# Input
SOURCES += main.cpp \
           ozzltcpserver.cpp \
           ozzltcpworker.cpp

HEADERS += ozzltcpserver.h \
           ozzltcpworker.h \
           ../common/ozzlprocommon.h