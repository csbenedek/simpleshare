#include "settings.h"

#include <QSettings>
#include <QByteArray>
#include <QStringList>
#include <QVariant>

// Settings

Settings::Settings()
{
    resetSettings();
    readSettings();
}

Settings::~Settings()
{
    writeSettings();
}

void Settings::resetSettings()
{
    delay               = 250;
    copyUrlToClipboard  = true;
    delete_file         = true;
    delete_other_files  = false;
    startAtLogin        = true;
    automaticUpload     = true;
    https               = true;
    urlShortener        = false;
    editor              = false;
    invertedSelection   = false;
    captureCursor       = false;
    preview             = false;
    launchEditorOnClick = true;
    suffix              = "png";
    quality             = 70;
    mute                = true;
    retryCount          = 3;
}

void Settings::writeSettings()
{
    QSettings settings;
    settings.setValue("options/delay", delay);
  //settings.setValue("options/grab_mode", grab_mode);
  //settings.setValue("options/show_window", show_window);
    settings.setValue("options/delete_file", delete_file);
    settings.setValue("options/https", https);
    settings.setValue("options/delete_other_files", delete_other_files);
    settings.setValue("options/startAtLogin", startAtLogin);
    settings.setValue("options/copyUrlToClipboard", copyUrlToClipboard);
    settings.setValue("options/automaticUpload", automaticUpload);
    settings.setValue("options/urlShortener", urlShortener);
    settings.setValue("options/editor", editor);
    settings.setValue("options/invertedSelection", invertedSelection);
    settings.setValue("options/captureCursor", captureCursor);
    settings.setValue("options/preview", preview);
    settings.setValue("options/launchEditorOnClick", launchEditorOnClick);
    settings.setValue("options/suffix", suffix);
    settings.setValue("options/quality", quality);
    settings.setValue("options/mute", mute);
    settings.setValue("options/retryCount", retryCount);

    // shortcuts:
    settings.setValue("options/fullScreenKey", fullScreenKey);
    settings.setValue("options/regionKey", regionKey);
    settings.setValue("options/activeWindowKey", activeWindowKey);
    settings.setValue("options/screencastKey", screencastKey);
    settings.setValue("options/uploadKey", uploadKey);
    settings.setValue("options/repeatKey", repeatKey);
    settings.setValue("options/annotateKey", annotateKey);

    settings.setValue("options/proxy", proxy);


    settings.beginWriteArray("recent");
    for(int i=0; i < recentUploads.size(); ++i)
    {
        const QPair<QString,QString>& pair = recentUploads.at(i);
        settings.setArrayIndex(i);
        if (!pair.first.isEmpty() && !pair.second.isEmpty())
        {
            settings.setValue("name", pair.first);
            settings.setValue("link", pair.second);
        }
    }
    settings.endArray();
}

void Settings::readSettings()
{
    QSettings settings;

    delay               = settings.value("options/delay", 250).toInt();
    delete_file         = settings.value("options/delete_file", true).toBool();
    delete_other_files  = settings.value("options/delete_other_files", false).toBool();
    https               = settings.value("options/https", true).toBool();
    startAtLogin        = settings.value("options/startAtLogin", true).toBool();
    automaticUpload     = settings.value("options/automaticUpload", true).toBool();
    copyUrlToClipboard  = settings.value("options/copyUrlToClipboard", true).toBool();
    urlShortener        = settings.value("options/urlShortener", true).toBool();
    editor              = settings.value("options/editor", false).toBool();
    invertedSelection   = settings.value("options/invertedSelection", false).toBool();
    captureCursor       = settings.value("options/captureCursor", false).toBool();
    preview             = false; //settings.value("options/preview", true).toBool();
    launchEditorOnClick = settings.value("options/launchEditorOnClick", true).toBool();
    quality             = settings.value("options/quality", 70).toReal();
    suffix              = settings.value("options/suffix", "png").toString();
#ifdef Q_OS_LINUX // have some problems with mic
    mute                = settings.value("options/mute", true).toBool();
#else
    mute                = settings.value("options/mute", true).toBool();
#endif
    retryCount          = settings.value("options/retryCount", 3).toInt();

    // shortcuts:
    fullScreenKey       = settings.value("options/fullScreenKey", "").toString();
    regionKey           = settings.value("options/regionKey", "").toString();
    activeWindowKey     = settings.value("options/activeWindowKey", "").toString();
    screencastKey       = settings.value("options/screencastKey", "").toString();
    uploadKey           = settings.value("options/uploadKey", "").toString();
    repeatKey           = settings.value("options/repeatKey", "").toString();
    annotateKey         = settings.value("options/annotateKey", "").toString();

    proxy               = settings.value("options/proxy", "").toString();

    // DON'T force http for all except login
    //https               = false;

    // Load all recent-upload info


    convertRecentData();


    ///// end support old versions
    ///// saving recent items

    const int size = qMin(20, settings.beginReadArray("recent"));

    for(int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        const QString name = settings.value("name").toString();
        const QString link = settings.value("link").toString();
        if (!name.isEmpty() && !link.isEmpty())
        {
            recentUploads.push_back(QPair<QString,QString>(name, link));
        }
    }

    settings.endArray();
}

void Settings::convertRecentData()
{
    QSettings settings;

    // Read old data (if exists)
    QVariant variant;
    recentUploads.clear();
    variant = settings.value("data/recentUpload2");
    if(variant.isNull()||!variant.isValid())
    {
        // no data
    }
    else
    {
        QString szRecent = variant.toString();
        QStringList params = szRecent.split("|");
        recentUploads.push_front(QPair<QString,QString>(params[0],params[1]));
    }
    variant = settings.value("data/recentUpload1");
    if(variant.isNull()||!variant.isValid())
    {
        // no data
    }
    else
    {
        QString szRecent = variant.toString();
        QStringList params = szRecent.split("|");

        recentUploads.push_front(QPair<QString,QString>(params[0],params[1]));
    }
    variant = settings.value("data/recentUpload0");
    if(variant.isNull()||!variant.isValid())
    {
        // no data
    }
    else
    {
        QString szRecent = variant.toString();
        QStringList params = szRecent.split("|");
        recentUploads.push_front(QPair<QString,QString>(params[0],params[1]));
    }

    settings.remove("data"); // clear it
}

void Settings::addToRecent(const QString& fileName, const QString& link)
{
    recentUploads.push_front(QPair<QString,QString>(fileName, link));
    writeSettings(); // TODO: do this really need ?
}

