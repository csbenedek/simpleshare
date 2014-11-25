/* urlshortener.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef URLSHORTENER_H
#define URLSHORTENER_H

#include <QObject>

class QNetworkReply;
class QNetworkAccessManager;

class UrlShortener : public QObject
{
  Q_OBJECT

public:
    explicit UrlShortener(QObject *parent = 0);

    void shorten(const QString& url, const QString& originalFileName = "");
    void setApiKey(const QString& apiKey);
    QString apiKey();

signals:
    void shortened(const QString& url, const QString& originalFileName);
    void errorMessage(const QString &message);

private slots:
    void replyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager*  m_networkManager;
    QString                 m_apiKey;
};


#endif // URLSHORTENER_H
