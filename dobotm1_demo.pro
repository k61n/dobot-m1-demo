QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dobotm1_demo
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        mydobot.cpp \
        alertdialog.cpp

HEADERS += \
        mainwindow.h \
        mydobot.h \
        alertdialog.h \
        mytypes.h

FORMS += \
        mainwindow.ui \
        alertdialog.ui

INCLUDEPATH += $$PWD/dobotm1lib

win32 {
    LIBS += -L$$PWD/dobotm1lib/win64/ -lDobotDllM1
}
unix {
    LIBS += -L$$PWD/dobotm1lib/linux/ -lDobotDllM1
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resources.qrc
