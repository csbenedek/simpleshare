#ifndef BASICUPDATER_H
#define BASICUPDATER_H

#include <QNetworkReply>
#include <QObject>

class QNetworkAccessManager;

class BasicUpdater : public QObject
{
    Q_OBJECT
public:
    explicit BasicUpdater(QObject *parent = 0);

signals:
    void updateAvailable(const QString& version, const QString& link);

public slots:
    void checkForUpdates();
private slots:
    void onfFinishedDownloading();
    void onError(QNetworkReply::NetworkError);
private:
    QNetworkAccessManager *downloader;
    void downloadVersionFile();
    void handleUpdateVersionInformation(const QString &updateInfo);
};

#endif // BASICUPDATER_H
