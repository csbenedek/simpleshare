VERSION = 1.1.50

RC_FILE = icon.rc
ICON = icon.ico

# Define the preprocessor macro to get the application version in application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG   += qxt
QT       += core gui network xml script webkit
TARGET   = simpleshare
TEMPLATE = app
TRANSLATIONS += simpleshare_ru.ts

include(./3rdparty/qtsingleapplication/src/qtsingleapplication.pri)

INCLUDEPATH += ./3rdparty/windowsystem
INCLUDEPATH += ./3rdparty/globalshortcut

INCLUDEPATH += ./3rdparty/qtsrc
INCLUDEPATH += ./3rdparty/qzip
INCLUDEPATH += ./3rdparty/zlib

INCLUDEPATH += ./imageeditor
INCLUDEPATH += ./videograb

INCLUDEPATH += ./BoxApi

SOURCES += \
    applicationex.cpp \
    main.cpp \
    preferenceswidget.cpp \
    graber.cpp \
    regionselect.cpp \
    settings.cpp \
    recordingframe.cpp \
    basicupdater.cpp \
    ziphelper.cpp \
    abstractdroptargetwidget.cpp \
    urlshortener.cpp \
    keysequencewidget.cpp \
    ./BoxApi/utils.cpp \
    ./BoxApi/bxnet.cpp \
    ./BoxApi/qupfile.cpp \
    ./BoxApi/crypto/blowfish.cpp \
    ./BoxApi/webloginform.cpp \
    ./BoxApi/webview.cpp \
    ./BoxApi/synchttp.cpp \
    ./imageeditor/imageeditor.cpp \
    ./imageeditor/imageeditorscene.cpp \
    ./imageeditor/imageeditorview.cpp \
    ./imageeditor/itemutils.cpp \
    ./imageeditor/arrowitem.cpp \
    ./imageeditor/textitem.cpp \
    ./imageeditor/rectitem.cpp \
    ./imageeditor/ellipseitem.cpp \
    ./imageeditor/cropitem.cpp \
    ./imageeditor/bluritem.cpp \
    ./imageeditor/pixmapitem.cpp \
    ./imageeditor/staritem.cpp \
    ./imageeditor/penitem.cpp \
    ./imageeditor/editablelabel.cpp \
    ./imageeditor/simplemenu.cpp \
    ./imageeditor/simplebutton.cpp \
    ./videograb/ffmpegrunner.cpp \
    ./3rdparty/globalshortcut/qxtglobalshortcut.cpp \
    ./3rdparty/globalshortcut/qxtglobal.cpp \
    ./3rdparty/windowsystem/qxtwindowsystem.cpp \
    ./3rdparty/qzip/qzip_static.cpp \
    ./3rdparty/qzip/qzip.cpp \
    ./3rdparty/systray/systemtrayicon_win.cpp \
    ./3rdparty/systray/systemtrayicon.cpp \
    textinputwidget.cpp \
    tooltipwidget.cpp \
    messagedialog.cpp \
    folderdialog.cpp


HEADERS += \
    applicationex.h \
    preferenceswidget.h \
    graber.h \
    regionselect.h \
    settings.h \
    recordingframe.h \
    basicupdater.h \
    ziphelper.h \
    abstractdroptargetwidget.h \
    urlshortener.h \
    keysequencewidget.h \
    ./BoxApi/utils.h \
    ./BoxApi/bxnet.h \
    ./BoxApi/qupfile.h \
    ./BoxApi/crypto/blowfish.h \
    ./BoxApi/webloginform.h \
    ./BoxApi/webview.h \
    ./BoxApi/synchttp.h \
    ./imageeditor/imageeditor.h \
    ./imageeditor/imageeditorscene.h \
    ./imageeditor/imageeditorview.h \
    ./imageeditor/itemutils.h \
    ./imageeditor/arrowitem.h \
    ./imageeditor/textitem.h \
    ./imageeditor/rectitem.h \
    ./imageeditor/ellipseitem.h \
    ./imageeditor/cropitem.h \
    ./imageeditor/bluritem.h \
    ./imageeditor/pixmapitem.h \
    ./imageeditor/staritem.h \
    ./imageeditor/penitem.h \
    ./imageeditor/editablelabel.h \
    ./imageeditor/simplemenu.h \
    ./imageeditor/simplebutton.h \
    ./videograb/ffmpegrunner.h \
    ./3rdparty/globalshortcut/qxtglobalshortcut_p.h \
    ./3rdparty/globalshortcut/qxtglobalshortcut.h \
    ./3rdparty/globalshortcut/qxtglobal.h \
    ./3rdparty/windowsystem/qxtwindowsystem.h \
    ./3rdparty/qzip/qzipwriter_p.h \
    ./3rdparty/qzip/qzipreader_p.h \
    ./3rdparty/systray/systemtrayicon.h \
    textinputwidget.h \
    tooltipwidget.h \
    messagedialog.h \
    folderdialog.h

FORMS += \
    ./imageeditor/imageeditor.ui \
    ./BoxApi/webloginform.ui \
    preferenceswidget.ui \
    recordingframe.ui \
    messagedialog.ui \
    folderdialog.ui

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
    ./videograb/audiograber_win.cpp \
    ./videograb/videograber_win.cpp \
    ./3rdparty/globalshortcut/qxtglobalshortcut_win.cpp \
    ./3rdparty/windowsystem/qxtwindowsystem_win.cpp

HEADERS += \
    droptargetsystray.h \
    oleutils.h \
    ./videograb/audiograber_win.h \
    ./videograb/videograber_win.h \
    ../shellutils/shellutils.h

# OLE and Shell

INCLUDEPATH += ../shellutils

# FFMPEG:

SOURCES += ./videograb/videoencoder.cpp
HEADERS += ./videograb/videoencoder.h


# Add the path
LIBS += -L$$PWD/3rdparty/ffmpeg/lib_win32
INCLUDEPATH += ./3rdparty/ffmpeg

# Set list of required FFmpeg libraries
LIBS += -lavutil \
    -lavcodec \
    -lavformat \
    -lswscale


# apt-get install ffmpeg
# apt-get install libavutil-dev libavcodec-dev libavformat-dev libswscale-dev

}

mac {
LIBS += -framework Carbon -framework CoreFoundation -framework CoreServices -framework ApplicationServices
QMAKE_INFO_PLIST = ./Info.plist
SOURCES += \
    ./3rdparty/globalshortcut/qxtglobalshortcut_mac.cpp \
    ./3rdparty/windowsystem/qxtwindowsystem_mac.cpp
}

linux {
LIBS += \
    -lX11

SOURCES += \
    ./3rdparty/globalshortcut/qxtglobalshortcut_x11.cpp \
    ./3rdparty/windowsystem/qxtwindowsystem_x11.cpp
}

unix {
LIBS += \
    -lz

HEADERS += \
    ./videograb/videograber_x11.h

SOURCES += \
    ./videograb/videograber_x11.cpp \
}

OTHER_FILES += \
    todo.txt




