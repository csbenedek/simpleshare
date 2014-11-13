QT       += core gui
TARGET   = systemtraydemo
TEMPLATE = app

INCLUDEPATH += $$PWD/systemtray_ext/qt_internals/qt4_systray

SOURCES += \
    main.cpp \
    widget.cpp \
    systemtray_ext/qt_internals/qt4_systray/qsystemtrayicon_win.cpp \
    systemtray_ext/systemtrayicon_ext.cpp

HEADERS += \
    widget.h \
    systemtray_ext/systemtrayicon_ext.h

FORMS += \
    widget.ui

RESOURCES += \
    resources.qrc
              
