# The application version http://goo.gl/68V62
#
# hieu@mycompany.com
# BoxNet2864!
#
VERSION = 1.0.42

RC_FILE = icon.rc
ICON = icon.ico

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG   += qxt
QT       += core gui network xml script webkit
TARGET   = simpleshare
TEMPLATE = app
TRANSLATIONS += simpleshare_ru.ts

include($$PWD/3rdparty/qtsingleapplication/src/qtsingleapplication.pri)

INCLUDEPATH += $$PWD/3rdparty/windowsystem
INCLUDEPATH += $$PWD/3rdparty/globalshortcut

INCLUDEPATH += $$PWD/3rdparty/qtsrc
INCLUDEPATH += $$PWD/3rdparty/qzip
INCLUDEPATH += $$PWD/3rdparty/zlib

INCLUDEPATH += $$PWD/imageeditor
INCLUDEPATH += $$PWD/videograb

INCLUDEPATH += $$PWD/../BoxApi


SOURCES += \
    applicationex.cpp \
    main.cpp \
    preferenceswidget.cpp \
    graber.cpp \
    regionselect.cpp \
    settings.cpp \
    $$PWD/../BoxApi/utils.cpp \
    $$PWD/../BoxApi/bxnet.cpp \
    $$PWD/../BoxApi/qupfile.cpp \
    recordingframe.cpp \
    basicupdater.cpp \
    ziphelper.cpp \
    abstractdroptargetwidget.cpp \
    urlshortener.cpp \
    keysequencewidget.cpp \
    $$PWD/imageeditor/imageeditor.cpp \
    $$PWD/imageeditor/imageeditorscene.cpp \
    $$PWD/imageeditor/imageeditorview.cpp \
    $$PWD/imageeditor/itemutils.cpp \
    $$PWD/imageeditor/arrowitem.cpp \
    $$PWD/imageeditor/textitem.cpp \
    $$PWD/imageeditor/rectitem.cpp \
    $$PWD/imageeditor/ellipseitem.cpp \
    $$PWD/imageeditor/cropitem.cpp \
    $$PWD/imageeditor/bluritem.cpp \
    $$PWD/imageeditor/pixmapitem.cpp \
    $$PWD/imageeditor/staritem.cpp \
    $$PWD/imageeditor/penitem.cpp \
    $$PWD/imageeditor/editablelabel.cpp \
    $$PWD/imageeditor/simplemenu.cpp \
    $$PWD/imageeditor/simplebutton.cpp \
    3rdparty/globalshortcut/qxtglobalshortcut.cpp \
    3rdparty/globalshortcut/qxtglobal.cpp \
    3rdparty/windowsystem/qxtwindowsystem.cpp \
    videograb/ffmpegrunner.cpp \
    3rdparty/qzip/qzip_static.cpp \
    3rdparty/qzip/qzip.cpp \
    3rdparty/systray/systemtrayicon_win.cpp \
    3rdparty/systray/systemtrayicon.cpp \
    ../BoxApi/crypto/blowfish.cpp \
    ../BoxApi/webloginform.cpp \
    ../BoxApi/webview.cpp

HEADERS += \
    applicationex.h \
    preferenceswidget.h \
    graber.h \
    regionselect.h \
    settings.h \
    $$PWD/../BoxApi/utils.h \
    $$PWD/../BoxApi/bxnet.h \
    $$PWD/../BoxApi/qupfile.h \
    recordingframe.h \
    basicupdater.h \
    ziphelper.h \
    abstractdroptargetwidget.h \
    urlshortener.h \
    keysequencewidget.h \
    $$PWD/imageeditor/imageeditor.h \
    $$PWD/imageeditor/imageeditorscene.h \
    $$PWD/imageeditor/imageeditorview.h \
    $$PWD/imageeditor/itemutils.h \
    $$PWD/imageeditor/arrowitem.h \
    $$PWD/imageeditor/textitem.h \
    $$PWD/imageeditor/rectitem.h \
    $$PWD/imageeditor/ellipseitem.h \
    $$PWD/imageeditor/cropitem.h \
    $$PWD/imageeditor/bluritem.h \
    $$PWD/imageeditor/pixmapitem.h \
    $$PWD/imageeditor/staritem.h \
    $$PWD/imageeditor/penitem.h \
    $$PWD/imageeditor/editablelabel.h \
    $$PWD/imageeditor/simplemenu.h \
    $$PWD/imageeditor/simplebutton.h \
    3rdparty/globalshortcut/qxtglobalshortcut_p.h \
    3rdparty/globalshortcut/qxtglobalshortcut.h \
    3rdparty/globalshortcut/qxtglobal.h \
    3rdparty/windowsystem/qxtwindowsystem.h \
    videograb/ffmpegrunner.h \
    3rdparty/qzip/qzipwriter_p.h \
    3rdparty/qzip/qzipreader_p.h \
    3rdparty/systray/systemtrayicon.h \
    ../BoxApi/crypto/blowfish.h \
    ../BoxApi/webloginform.h \
    ../BoxApi/webview.h \
    ui_config.h

FORMS += \
    preferenceswidget.ui \
    recordingframe.ui \
    $$PWD/imageeditor/imageeditor.ui \
    ../BoxApi/webloginform.ui

RESOURCES += \
    resources.qrc

##################################################################################


win32 {
LIBS += \
    -luser32 \
    -lgdi32 \
    -lwinmm

SOURCES += \
    droptargetsystray.cpp \
    oleutils.cpp \
    $$PWD/videograb/audiograber_win.cpp \
    $$PWD/videograb/videograber_win.cpp \
    3rdparty/globalshortcut/qxtglobalshortcut_win.cpp \
    3rdparty/windowsystem/qxtwindowsystem_win.cpp

HEADERS += \
    droptargetsystray.h \
    oleutils.h \
    $$PWD/videograb/audiograber_win.h \
    $$PWD/videograb/videograber_win.h \
    $$PWD/../shellutils/shellutils.h

# OLE and Shell

INCLUDEPATH += $$PWD/../shellutils
CONFIG += qaxcontainer

# FFMPEG:

SOURCES += $$PWD/videograb/videoencoder.cpp
HEADERS += $$PWD/videograb/videoencoder.h

# Set list of required FFmpeg libraries
LIBS += -lavutil \
    -lavcodec \
    -lavformat \
    -lswscale

# Add the path
LIBS += -L$$PWD/videograb/lib_win32

# apt-get install ffmpeg
# apt-get install libavutil-dev libavcodec-dev libavformat-dev libswscale-dev

}

mac {
LIBS += -framework Carbon -framework CoreFoundation -framework CoreServices -framework ApplicationServices
QMAKE_INFO_PLIST = ./Info.plist
SOURCES += 3rdparty/globalshortcut/qxtglobalshortcut_mac.cpp \
    3rdparty/windowsystem/qxtwindowsystem_mac.cpp
}

linux {
LIBS += \
    -lX11

SOURCES += \
    3rdparty/globalshortcut/qxtglobalshortcut_x11.cpp \
    3rdparty/windowsystem/qxtwindowsystem_x11.cpp
}

unix {
LIBS += \
    -lz

HEADERS += \
    $$PWD/videograb/videograber_x11.h

SOURCES += \
    $$PWD/videograb/videograber_x11.cpp \
}

OTHER_FILES += \
    todo.txt

