#include "basicupdater.h"
#include <QNetworkAccessManager>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QSettings>
#include <QDebug>

// BasicUpdater

// version file format: build number->32bit installer->64bit installer->full version info
// for example: 15->http://sites.box.net/apps/web/simpleshare/SimpleShare32.msi->http://sites.box.net/apps/web/simpleshare/SimpleShare64.msi->1.0.15

BasicUpdater::BasicUpdater(QObject *parent) :
        QObject(parent)
{
    downloader = new QNetworkAccessManager(this);
}

void BasicUpdater::checkForUpdates()
{
    //we download the version file
    //after the download will be completed we check if
    //a nre verson is available

    downloadVersionFile();

    // for DEBUG purposes only:
    //handleUpdateVersionInformation("15->http://sites.box.net/apps/web/simpleshare/SimpleShare32.msi->http://sites.box.net/apps/web/simpleshare/SimpleShare64.msi->1.0.15");
}

void BasicUpdater::downloadVersionFile()
{
    QString fileUrl="http://sites.box.net/apps/web/simpleshare/version";

    QUrl url(fileUrl);
    QNetworkReply *reply =downloader->get(QNetworkRequest(url));

    connect(reply, SIGNAL(finished()), this, SLOT(onfFinishedDownloading()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
}

void BasicUpdater::onfFinishedDownloading()
{
    QNetworkReply *reply=(QNetworkReply *)(sender());
    QString data=QString(reply->readAll());
    qDebug()<<"The response recived is:\n"<<data;
    handleUpdateVersionInformation(data);
    reply->deleteLater();
}

void BasicUpdater::onError(QNetworkReply::NetworkError)
{
    qDebug("error downloading version file");
}

void BasicUpdater::handleUpdateVersionInformation(const QString &updateInfo)
{
    try
    {
        QStringList list=updateInfo.split("->");
        if (list.size() < 2)
        {
            qDebug("after split the list size is less then 2");
            return;
        }
        int build               = list.at(0).split(".").last().toInt();
        int curentBuildNumber   = QApplication::applicationVersion().split(".").last().toInt();

        if (curentBuildNumber >= build)
        {
            qDebug("no need to upgrade");
            return;
        }

        bool is64bit = false;
        if (list.size() >= 3)
        {
#ifdef Q_OS_WIN
            // build->x86.msi->i64.msi
            is64bit = false;

            // Microsoft KB556009 HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment = x86 == 32 bit

            QSettings settings("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", QSettings::NativeFormat);
            QString arch = settings.value("PROCESSOR_ARCHITECTURE").toString();
            if (arch == "x86")
            {
                is64bit = false;
            }
            else if (arch == "AMD64")
            {
                is64bit = true;
            }
#endif
        }

        emit updateAvailable(list.at(list.size() >=4 ? 3 : 0), list.at(is64bit ? 2 : 1).trimmed());
    }
    catch(...)
    {
        qDebug("can't upgrade");
    }
}
