/*
 * box.com api class
 * - oauth2 authorization
 * - file uploads
 * - user info
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */


#include "boxcom.h"

#include "qupfile.h"

#include "WebLoginForm.h"
#include "webview.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

// BoxCom

BoxCom::BoxCom()
    : m_networkManager(NULL)
    , m_loadAvatar(true)
    , m_authentificated(false)
    , m_checkingAuthToken(false)
    , m_waitingForAuthToken(false)
    , m_maxUploadSize(0)
    , m_uploadProgress(-1)
    , m_upf(NULL)
    , m_webLoginForm(NULL)
    , m_isConnected(false)
    , m_isSSLErrors(false)
{
    qDebug() << Q_FUNC_INFO << "BoxCom API class initializing";

    m_fileTypeIcons.resize(FILE_MAX);
    for(int i = 0; i < FILE_MAX; ++i)
    {
        m_fileTypeIcons[i] = NULL;
    }

    m_networkManager = new QNetworkAccessManager();

    checkConnection();

    loadAuth();
    loadSettings();
}

BoxCom::~BoxCom()
{
    delete m_webLoginForm;

    stopCurrentUpload();
    saveSettings();

    closeFile();

    if (m_networkManager)
    {
        m_networkManager->disconnect();
        delete m_networkManager;
    }

    for(int i = 0; i < FILE_MAX; ++i)
    {
        QIcon* icon = m_fileTypeIcons.at(i);
        delete icon;
    }
    m_fileTypeIcons.resize(0);

    qDebug() << Q_FUNC_INFO << "BoxCom API class done";
}

void BoxCom::onSslError(const QList<QSslError> & errors)
{
    bool firstTime = true;
    for(int i=0; i<errors.count(); i++)
    {
        //qDebug() << "SSL error: " << errors.at(i).errorString();
        if(errors.at(i).errorString() == "No error")
        {
            QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
            Q_ASSERT(reply);
            if(reply)
            {
                reply->ignoreSslErrors();
            }
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "SSL error: " << errors.at(i).errorString();

            if (firstTime)
            {
                emit authFailed(ssl_error);
                firstTime = false;
            }
        }
    }
}

void BoxCom::onOAuth2Code(BxNet::RESPONSE_STATUS status, const QString& oauth2_code)
{
    qDebug() << Q_FUNC_INFO << "oauth2_code=" << oauth2_code;
    closeLoginForm();

    QByteArray postData("grant_type=authorization_code&code=");
    postData.append(oauth2_code);
    postData.append("&client_id=" OAUTH2_CLIENT_ID "&client_secret=" OAUTH2_CLIENT_SECRET);

    // For your "Content-Length" header
    QByteArray postDataSize = QByteArray::number(postData.size());

    // Time for building your request
    QUrl serviceURL(OAUTH2_AUTH_TOKEN_URL);
    QNetworkRequest request(serviceURL);

    // Add the headers specifying their names and their values with the following method : void QNetworkRequest::setRawHeader(const QByteArray & headerName, const QByteArray & headerValue);
    request.setRawHeader("User-Agent",     "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)");
    request.setRawHeader("Content-Type",   "application/x-www-form-urlencoded");
    request.setRawHeader("Content-Length", postDataSize);

    QNetworkReply* reply = m_networkManager->post(request, postData);
    connect(reply, SIGNAL(finished()), this, SLOT(onOAuth2TokenFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onOAuth2Error(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
}

void BoxCom::onOAuth2TokenFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }

    QString response(reply->readAll());

    // {"access_token":"MaWMWqkmWqd6aWI3YphkhjgDroGVvrA1","expires_in":3711,"restricted_to":[],"refresh_token":"4T8kn7Ou51QRTKCseK7lpjgMx92IWhIKG3xPKhXiivgkEAoH2X82sLMkT0frciEA","token_type":"bearer"}

    const QStringList items = response.mid(1, response.length()-2).split(',');

    foreach(const QString item, items)
    {
        const QStringList list = item.split(':');
        QString name = list[0];
        if (name.startsWith('"'))
        {
            name = name.mid(1, name.length()-2);
        }
            //if ( name == "access_token" )
        QString value = list[1];
        if (value.startsWith('"'))
        {
            value = value.mid(1, value.length()-2);
        }

        qDebug() << Q_FUNC_INFO << name << "=" << value;

        if ( name == "access_token" )
        {
            m_oauth2_token = value;
        }
        else if ( name == "refresh_token" )
        {
            m_oauth2_refresh_token = value;
        }
        else if ( name == "token_type" )
        {
            m_oauth2_token_type = value;
        }
    }


}

void BoxCom::onAuthError(BxNet::RESPONSE_STATUS status)
{
    qDebug() << Q_FUNC_INFO << "status=" << status;

    closeLoginForm();

    emit authFailed(status);
}


double BoxCom::spaceAmount() const
{
    return m_spaceAmount;
}

double BoxCom::spaceUsed() const
{
    return m_spaceUsed;
}

void BoxCom::readProxySettingsFromSystem()
{
#ifdef Q_WS_WIN
    qDebug() << Q_FUNC_INFO << "reading proxy settings from internet explorer settings";

    //load IE settings for proxying
    QSettings registry("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", QSettings::NativeFormat);
    const bool proxyEnabled = registry.value("ProxyEnable", false).toBool();
    const QString proxy = registry.value("ProxyServer", "").toString();

    if (proxyEnabled && !proxy.isEmpty())
    {
        QStringList parts  = proxy.split(":");
        enableProxying(
            parts.at(0),
            parts.size() > 1 ? parts.at(1) : "",
            QNetworkProxy::HttpProxy);
    }
#endif
}

QString BoxCom::proxySettings() const
{
    QNetworkProxy proxy = QNetworkProxy::applicationProxy();
    return !proxy.hostName().isEmpty() ? proxy.hostName() + ":" +QString::number(proxy.port()) : "";
}

void BoxCom::setProxySettings(const QString& proxy)
{
    QStringList strs = proxy.split(":");
    if (strs.count() == 1)
    {
        enableProxying(strs.at(0), "");
    }
    else if (strs.count() > 1)
    {
        enableProxying(strs.at(0), strs.at(1));
    }
}

void BoxCom::enableProxying(const QString& host, const QString& port,
                           QNetworkProxy::ProxyType type,
                           const QString& username, const QString& password)
{
    if (!host.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "proxy " << host << ":" << port << " type=" << type;

        QNetworkProxy proxy;
        proxy.setType(type);
        proxy.setHostName(host);
        if (!port.isEmpty())
        {
            proxy.setPort(port.toInt());
        }
        if (!username.isEmpty())
        {
            proxy.setUser(username);
        }
        if (!password.isEmpty())
        {
            proxy.setPassword(password);
        }
        QNetworkProxy::setApplicationProxy(proxy);
    }
}

void BoxCom::login()
{
    // Start OAuth2 login
    openLoginForm();
}

QIcon& BoxCom::getFileTypeIcon(int fileType)
{
    QIcon* result = m_fileTypeIcons.at(fileType);
    if (result == NULL)
    {
        QString resName;

        switch(fileType)
        {
        case FILE_IMAGE:
            resName = ":/res/icon_image.png";
            break;
        case FILE_VIDEO:
            resName = ":/res/icon_video.png";
            break;
        case FILE_AI:
            resName = ":/res/icon_ai.png";
            break;
        case FILE_PSD:
            resName = ":/res/icon_psd.png";
            break;
        case FILE_BOOKMARK:
            resName = ":/res/icon_bookmark.png";
            break;
        case FILE_DOC:
            resName = ":/res/icon_doc.png";
            break;
        case FILE_XLS:
            resName = ":/res/icon_xls.png";
            break;
        case FILE_PPT:
            resName = ":/res/icon_ppt.png";
            break;
        case FILE_FLA:
            resName = ":/res/icon_fla.png";
            break;
        case FILE_MUSIC:
            resName = ":/res/icon_music.png";
            break;
        case FILE_HTML:
            resName = ":/res/icon_html.png";
            break;
        case FILE_ZIP:
            resName = ":/res/icon_zip.png";
            break;
        default:
            resName = ":/res/icon_file.png";
        }

        result = new QIcon(resName);
        m_fileTypeIcons[fileType] = result;
    }

    Q_ASSERT(result);
    return *result;
}

void BoxCom::openLoginForm()
{
    emit beginOAuth2();

    if (m_webLoginForm == NULL)
    {
        m_webLoginForm = new WebLoginForm(NULL);
    }

    if (m_webLoginForm)
    {
        connect(m_webLoginForm, SIGNAL(onAuthSuccess(BxNet::RESPONSE_STATUS, QString)), this, SLOT(onOAuth2Code(BxNet::RESPONSE_STATUS, QString)));
        connect(m_webLoginForm, SIGNAL(onAuthError(BxNet::RESPONSE_STATUS)), this, SLOT(onAuthError(BxNet::RESPONSE_STATUS)));
        connect(m_webLoginForm, SIGNAL(beginSSO()), this, SIGNAL(beginSSO()));

        m_webLoginForm->goToOAuth2LoginUrl();
    }
    else
    {
        onAuthError(BxNet::no_memory);
    }
}

void BoxCom::closeLoginForm()
{
    if (m_webLoginForm)
    {
        m_webLoginForm->hide();
        m_webLoginForm->deleteLater();
        m_webLoginForm = NULL;
    }
}
