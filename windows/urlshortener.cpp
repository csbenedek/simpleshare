/* urlshortener.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include <QRegExp>
#include <QScriptEngine>
#include "urlshortener.h"

UrlShortener::UrlShortener(QObject* parent)
    : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)) );
}

void UrlShortener::shorten(const QString &url, const QString &originalFileName)
{
    QString newUrl = url.contains("://") ? url : "http://" + url;

    QString queryStr = "https://www.googleapis.com/urlshortener/v1/url";
    if (!m_apiKey.isEmpty())
    {
        queryStr += "?key=" + m_apiKey;
    }
    QUrl queryUrl(queryStr);
    QNetworkRequest request(queryUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    const QString content = "{\"longUrl\": \"" + newUrl +"\"}";
    QNetworkReply* reply = m_networkManager->post(request, content.toLatin1());
    reply->setProperty("fileName", originalFileName);
}

void UrlShortener::replyFinished(QNetworkReply* reply)
{
    QByteArray data = reply->readAll();

    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate("(" + QString(data) + ")");

    if (sc.property("error").isValid())
    {
        QString message = sc.property("message").toString();
        qDebug() << "can't shorten link: " << message;
        emit errorMessage(message);
    }
    else
    {
        QString shortLink = sc.property("id").toString();
        qDebug() << "shortened link: " << shortLink;
        const QString originalFileName = reply->property("fileName").toString();
        emit shortened(shortLink, originalFileName);
    }

    reply->deleteLater();
}

void UrlShortener::setApiKey(const QString& apiKey)
{
    m_apiKey = apiKey;
}

QString UrlShortener::apiKey()
{
    return m_apiKey;
}
