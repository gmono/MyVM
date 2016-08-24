#-------------------------------------------------
#
# Project created by QtCreator 2016-06-27T19:44:09
#
#-------------------------------------------------

QT       -= core gui

TARGET = vmlib
TEMPLATE = lib

DEFINES += VMLIB_LIBRARY

SOURCES += mvm.cpp \
    mvm_dofuns.cpp

HEADERS += mvm.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
