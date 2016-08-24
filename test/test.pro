
QT -= core gui

CONFIG += c++11

TARGET = test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../vmlib/release/ -lvmlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../vmlib/debug/ -lvmlib
else:unix: LIBS += -L$$OUT_PWD/../vmlib/ -lvmlib

INCLUDEPATH += $$PWD/../vmlib
DEPENDPATH += $$PWD/../vmlib
