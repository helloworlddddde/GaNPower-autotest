QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Include"
LIBS        += "C:/Windows/System32/visa64.dll"
SOURCES += \
    dynamicwindow.cpp \
    main.cpp \
    serialbutton.cpp \
    serialthread.cpp \
    startwindow.cpp \
    staticwindow.cpp \
    testbutton.cpp \
    visabutton.cpp

HEADERS += \
    dynamicwindow.h \
    serialbutton.h \
    serialthread.h \
    startwindow.h \
    staticwindow.h \
    testbutton.h \
    visabutton.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
