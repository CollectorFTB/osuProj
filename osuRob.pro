TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -pthread

INCLUDEPATH += $$PWD/../robotarm

DEPENDPATH += $$PWD/../robotarm

SOURCES += main.cpp

DISTFILES +=

unix:!macx: LIBS += -lopencv_core

unix:!macx: LIBS += -lopencv_imgproc

unix:!macx: LIBS += -lopencv_highgui

unix:!macx: LIBS += -L/home/dlr/Documents/qt/robotarm/build/release/ -lrobotarm

INCLUDEPATH += $$PWD/../robotarm/build/release
DEPENDPATH += $$PWD/../robotarm
