/* boxcom.h
 * box.com api class
 * - oauth2 authorization
 * - file uploads
 * - user info
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */


#ifndef BOXCOM_H
#define BOXCOM_H

#include <QObject>
#include <QNetworkReply>
#include <QString>

class QUpFile;
class WebLoginForm;


#define DEFAULT_UPLOAD_FOLDER       "SimpleShare Uploads"

// OAuth2

#define OAUTH2_CLIENT_ID      "eo0cww5szjn2wywnkcje927zdk0opxxp"
#define OAUTH2_CLIENT_SECRET  "YrtmNDYRVSPuchqX6yv52c4AkIhNydh9"

#define OAUTH2_AUTHORIZE_URL       "https://api.box.com/oauth2/authorize?"
#define OAUTH2_AUTHORIZE_CODE_URL  OAUTH2_AUTH_AUTHORIZE_URL "response_type=code&client_id=%s&redirect_uri=%s"

#define OAUTH2_REDIRECT_URL        "https://app.box.com/oauth2/logged_in"
#define OAUTH2_AUTH_TOKEN_URL      "https://api.box.com/oauth2/token"


class BoxCom: public QObject
{
    Q_OBJECT
public:
    BoxCom();
    ~BoxCom();

public:
    // TODO: bool isConnected() const; // TODO: refactor this
    
    // Proxy settings
    void readProxySettingsFromSystem();
    void enableProxying(const QString& host, const QString& port = "",
                        QNetworkProxy::ProxyType type = QNetworkProxy::HttpProxy,
                        const QString& username = "", const QString& password = "");

    QString proxySettings() const;
    void setProxySettings(const QString& proxy);

    // Login
    void login();
    // TODO: void logout();

    QIcon& getFileTypeIcon(int fileType);

protected:
    void initLog();
    void doneLog();    

private:
    void openLoginForm();
    void closeLoginForm();


signals:
    void beginOAuth2();
    void networkError(QNetworkReply::NetworkError error, const QString& errorString);
    void authFailed(const QString& error);
    void authCompleted();

    void notLoggedIn();
    void errorOpenBrowser(QString url);

    //void fileSizeLimit(const QString& fileName);

private slots:
    void onOAuth2Code(BxNet::RESPONSE_STATUS status, const QString& oauth2_code);
    void onOAuth2Error(BxNet::RESPONSE_STATUS status);
    void onOAuth2TokenFinished();

    void onError(QNetworkReply::NetworkError);
    void onSilentError(QNetworkReply::NetworkError);
    void onSslError(const QList<QSslError>& errors);

private:
    QPointer<QNetworkAccessManager> m_networkManager;
    QPointer<QNetworkReply>         m_uploadsReply;

    QString     m_oauth2_token;
    QString     m_oauth2_refresh_token;
    QString     m_oauth2_token_type;
    //quint64     m_oauth2_refresh_interval;

    double      m_spaceAmount;
    double      m_spaceUsed;
    double      m_maxUploadSize;

    QVector<QIcon*>     m_fileTypeIcons;

    QPointer<WebLoginForm>      m_webLoginForm;
};

#endif // BOXCOM_H
