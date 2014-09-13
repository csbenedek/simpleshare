#ifndef SETTINGS_H
#define SETTINGS_H

#include <QList>
#include <QPair>
#include <QString>

class UserSession;

// TODO: rewrite using binding and Q_PROPERTY

class Settings
{
public:  
    typedef QPair<QString, QString> RecentItem;

    Settings();
    ~Settings();    

    QList<RecentItem> recentUploads; // recent upload links which is should on system tray menu

    void addToRecent(const QString &fileName, const QString &link);

    void resetSettings();
    void writeSettings();
    void readSettings();

private:
    void convertRecentData();

public:
    int     delay;
    bool    copyUrlToClipboard;
    bool    delete_file;
    bool    delete_other_files;
    bool    startAtLogin;
    bool    automaticUpload;
    bool    https;
    bool    urlShortener;
    bool    editor;
    bool    invertedSelection;
    bool    captureCursor;
    bool    preview;
    bool    launchEditorOnClick;
    QString fullScreenKey;
    QString regionKey;
    QString activeWindowKey;
    QString screencastKey;
    QString uploadKey;
    QString repeatKey;
    QString annotateKey;
    QString proxy;
    QString suffix;
    int     quality;
    bool    mute;
    int     retryCount;
    QString lastUserName;
};

#endif // SETTINGS_H
