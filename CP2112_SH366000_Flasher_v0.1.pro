QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BatteryCommands.cpp \
    cp2112.cpp \
    main.cpp \
    mainwindow.cpp \
    readdialog.cpp

HEADERS += \
    BatteryCommands.h \
    GPIO_Config.h \
    Library/Windows/SLABCP2112.h \
    SMBusConfig.h \
    cp2112.h \
    cp2112_config.h \
    mainwindow.h \
    readdialog.h \
    sh366000_addresses.h \
    types.h

FORMS += \
    mainwindow.ui \
    readdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Library/Windows/x64/SLABHIDDevice.dll \
    Library/Windows/x64/SLABHIDtoSMBus.dll \
    Library/Windows/x64/SLABHIDtoSMBus.lib

win32: LIBS += -L$$PWD/Library/Windows/x64/ -lSLABHIDtoSMBus

INCLUDEPATH += $$PWD/Library/Windows
DEPENDPATH += $$PWD/Library/Windows

RESOURCES += \
    myResources.qrc \
    myResources.qrc

RC_ICONS = icon.ico
