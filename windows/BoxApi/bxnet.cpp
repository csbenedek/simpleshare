#include "bxnet.h"
#include <QtNetwork>
#include <QtCore>
#include <QtXml>
#include <QDebug>
#include <QMessageBox>
#include <QDataStream>
#include <QStringList>
#include <QSslError>
#include <QIcon>
#include <QDesktopServices>
#include <fstream>
#include "utils.h"
#include "qupfile.h"
#include "applicationex.h"

#include "qzipreader_p.h"

#include "WebLoginForm.h"
#include "webview.h"

using namespace std;

#ifdef Q_OS_WIN
#include <windows.h>
#endif


// BxNet

BxNet::BxNet()
    : m_networkManager(NULL)
    , m_uploadsReply(NULL)
    , m_https(true)
    , m_loadAvatar(true)
    , m_authentificated(false)
    , m_checkingAuthToken(false)
    , m_waitingForAuthToken(false)
    , m_maxUploadSize(0)
    , m_uploadProgress(-1)
    , m_upf(NULL)
    , m_disconnected(false)
    , m_webLoginForm(NULL)
{
    qDebug() << Q_FUNC_INFO << "BxNet initializing";

    m_fileTypeIcons.resize(FILE_MAX);
    for(int i = 0; i < FILE_MAX; ++i)
    {
        m_fileTypeIcons[i] = NULL;
    }

    initializeStatusMessagesHash();
    m_networkManager = new QNetworkAccessManager();

    loadAuth();
    loadSettings();
}

BxNet::~BxNet()
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
        if (icon != NULL)
        {
            delete icon;
        }
    }
    m_fileTypeIcons.resize(0);

    qDebug() << Q_FUNC_INFO << "BxNet done";
}

int BxNet::warningWithLog(
    QWidget* parent,
    const QString& title,
    const QString& text,
    bool logButton)
{
    if (ApplicationEx::isLoggingEnabled() && logButton)
    {
        qDebug() << Q_FUNC_INFO << "error message: " << text;

        QMessageBox messageBox(parent);

        messageBox.setWindowTitle(title);
        messageBox.setText(text);
        messageBox.setInformativeText(tr("for more information, click the button \"Show log\""));
        messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Save);
        messageBox.setDefaultButton(QMessageBox::Ok);
        messageBox.setButtonText(QMessageBox::Save, tr("Show log"));
        messageBox.setIcon(QMessageBox::Warning);

        messageBox.setModal(true);
        messageBox.show();
        messageBox.raise();

        centrateWidget(&messageBox);

        int result = messageBox.exec();

        switch(result)
        {
        case QMessageBox::Save:
            QDesktopServices::openUrl(QUrl(ApplicationEx::logFileName()));
            break;
        case QMessageBox::Ok:
            // do nothing for now
            break;
        }

        return result;
    }

    return QMessageBox::warning(parent, title, text, QMessageBox::Ok, QMessageBox::Ok);
}


void BxNet::initializeStatusMessagesHash()
{
    //TODO: make this static

    m_statusMessages.insert("wrong auth token",         wrong_auth_token);
    m_statusMessages.insert("application_restricted",   application_restricted);
    m_statusMessages.insert("upload_some_files_failed", upload_some_files_failed);
    m_statusMessages.insert("not_enough_free_space",    not_enough_free_space);
    m_statusMessages.insert("filesize_limit_exceeded",  filesize_limit_exceeded);
    m_statusMessages.insert("access_denied",            access_denied);
    m_statusMessages.insert("upload_wrong_folder_id",   upload_wrong_folder_id);
    m_statusMessages.insert("upload_invalid_file_name", upload_invalid_file_name);
    m_statusMessages.insert("not_logged_in",            not_logged_in);

    m_statusMessages.insert("get_auth_token_error",     get_auth_token_error);

    m_statusMessages.insert("e_input_params",           e_input_params);
    m_statusMessages.insert("folder_name_too_bid",      folder_name_too_bid);
    m_statusMessages.insert("e_no_folder_name",         e_no_folder_name);
    m_statusMessages.insert("e_no_access",              e_no_access);
    m_statusMessages.insert("invalid_folder_name",      invalid_folder_name);
    m_statusMessages.insert("s_folder_exists",          s_folder_exists);
    m_statusMessages.insert("no_parent",                no_parent);
    m_statusMessages.insert("create_ok",                create_ok);

    m_statusMessages.insert("share_ok",                 share_ok);
    m_statusMessages.insert("invalid_login",            invalid_login);
    m_statusMessages.insert("login_limit",              login_limit);

    m_statusMessages.insert("get_ticket_ok",            get_ticket_ok);
    m_statusMessages.insert("logged",                   logged);
    m_statusMessages.insert("get_account_info_ok",      get_account_info_ok);
    m_statusMessages.insert("s_get_user_info",          s_get_user_info);
    m_statusMessages.insert("get_auth_token_ok",        get_auth_token_ok);
    m_statusMessages.insert("upload_ok",                upload_ok);

    m_statusMessages.insert("listing_ok",               listing_ok);
    m_statusMessages.insert("e_folder_id",              e_folder_id);
}

BxNet::RESPONSE_STATUS BxNet::responseStatusFromString(const QString& status) const
{
    if (!m_statusMessages.contains(status))
    {
        return unknown_status;
    }
    return m_statusMessages.value(status);
}

BxNet::RESPONSE_STATUS BxNet::responseStatus(QNetworkReply* reply, QDomElement* root)
{
    Q_ASSERT(reply);
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        return network_error;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    QString response(reply->readAll());

    RESPONSE_STATUS plainStatus = responseStatusFromString(response);
    if (plainStatus != unknown_status)
    {
        qDebug() << Q_FUNC_INFO << "Not a XML response!: "  << response;

        return plainStatus;
    }

    QDomDocument doc;
    if (!doc.setContent(response, false, &errorStr, &errorLine, &errorColumn))
    {
        qDebug() << Q_FUNC_INFO << "XML Error: Parse error at line " << errorLine << ", " << "column " << errorColumn << ": " << qPrintable(errorStr);
        qDebug() << Q_FUNC_INFO << "XML Was: "  << response;

        return replay_parssing_error;
    }

    QDomElement tmpRoot;
    if (root == NULL)
    {
        root = &tmpRoot;
    }

    *root = doc.documentElement();
    if (root->tagName() != "response")
    {
        qDebug() << Q_FUNC_INFO << "XML Error: Not a response file" << "\n";
        qDebug() << Q_FUNC_INFO << "XML Was: "  << response;

        return replay_parssing_error;
    }

    const QDomNode child = root->firstChild();
    if (child.isNull())
    {
        qDebug() << Q_FUNC_INFO << "XML Error: child is NULL";
        qDebug() << Q_FUNC_INFO << "XML Was: "  << response;

        return replay_parssing_error;
    }

    const QDomElement element = child.toElement();
    if (element.tagName() != "status")
    {
        qDebug() << Q_FUNC_INFO << "XML Error: node name not a \"status\"\n";
        qDebug() << Q_FUNC_INFO << "XML Was: "  << response;

        return replay_parssing_error;
    }

    const QString status = element.text();

    BxNet::RESPONSE_STATUS result = responseStatusFromString(status);

    securelyErase(response);

    return result;
}

void BxNet::closeFile()
{
    if (isUploading())
    {
        qDebug() << Q_FUNC_INFO << "WARNING closing file while upload in progress.";
        return;
    }
    m_currentUploadingName.clear();
    if (m_upf)
    {
        qDebug() << Q_FUNC_INFO << "Uploading file closed";

        m_upf->close();
        delete m_upf;
        m_upf = NULL;
    }
    emit uploadQueueChanged();
}

void BxNet::onUploadProgress(qint64 uploaded, qint64 total)
{
    m_total = total;
    m_uploaded = uploaded;

    if (total <= 0)
    {
        m_uploadProgress = -1;
        return;
    }

    int uploadProgress = qFloor(uploaded*1.0 / total * 100.0);

    if (m_uploadProgress != uploadProgress)
    {
        m_uploadProgress = uploadProgress;
        qDebug() << "Upload progress " << m_uploadProgress << "%" << "uploaded" << uploaded << "/" << total;
    }

    emit uploadProgressSignal(m_uploadProgress, uploaded, total);
}

void BxNet::addFileToUploadQueue(const QString& fileName)
{   
    if (m_uploadingQueue.contains(fileName))
    {
        qDebug() << Q_FUNC_INFO << "file "<< fileName << " already in upload queue";
        return;
    }

    QFileInfo fileInfo(fileName);

    if (m_authentificated && m_maxUploadSize > 0 && fileInfo.size() > m_maxUploadSize)
    {
        qDebug() << Q_FUNC_INFO << "file "<< fileName << " size " << fileInfo.size() << " limit " << m_maxUploadSize << " exceeded.";
        emit fileSizeLimit(fileName);
        return;
    }

    m_uploadingQueue.push_back(fileName);
    m_uploadingQueue.removeDuplicates(); // second check. does it need ?

    qDebug() << Q_FUNC_INFO << "file "<< fileName << " added to queue";
    emit uploadQueueChanged();

    if (!isUploading())
    {
        startUpload(); // delayed start
    }
}

void BxNet::startUpload()
{
    qDebug() << "start upload called";

    if (m_uploadingQueue.size() == 0)
    {
        emit queueEmpty();
        return;
    }

    //Q_ASSERT(m_authentificated);
    if (!m_authentificated)
    {
        if (!m_checkingAuthToken)
        {
            qDebug() << Q_FUNC_INFO << "emit notLoggedIn()";
            emit notLoggedIn();
        }
        else
        {
            checkAuth();
            QTimer::singleShot(2000, this, SLOT(startUpload()));
        }
        return;
    }

    if (!isConnectedToNetwork())
    {

        if (!m_disconnected)
        {
            qDebug() << Q_FUNC_INFO << "disconnected from network";
            emit disconnected();
        }
        else
        {
            qDebug() << "still no connection";
        }
        m_disconnected = true;

        QTimer::singleShot(15000, this, SLOT(startUpload())); // 15 sec waiting
        return;
    }
    else
    {
        if (m_disconnected)
        {
            qDebug() << "connected after disconnect. waiting a little...";
            m_disconnected = false;
            QTimer::singleShot(15000, this, SLOT(startUpload())); // wait some time
            return;
        }
        m_disconnected = false;
    }

    if (m_upf || isUploading())
    {
        QTimer::singleShot(15000, this, SLOT(startUpload()));
        return;
    }

    emit startUploadSignal();

    QString fileName;

    do
    {

        if (m_uploadingQueue.size() <= 0)
        {
            qDebug() << Q_FUNC_INFO << "trying to startUpload with an empty list";
            return;
        }

        fileName = m_uploadingQueue.at(0);
        m_uploadingQueue.pop_front();

    }
    while (fileName.isEmpty());

    QFileInfo fileInfo(fileName);

    if (m_authentificated && m_maxUploadSize > 0 && fileInfo.size() > m_maxUploadSize)
    {
        m_uploadingQueue.pop_front();
        qDebug() << Q_FUNC_INFO << "file "<< fileName << " size " << fileInfo.size() << " limit " << m_maxUploadSize << " exceeded.";
        emit fileSizeLimit(fileName);
        return;
    }

    //https://upload.box.net/api/1.0/upload/"+auth_token+"/"+user->uploadFolderId;

    //qDebug() << "start upload with auth = " << m_authToken;

    QString uploadUrl = m_https ? "https://" : "http://";
    uploadUrl += "upload.box.net/api/1.0/upload/" + m_authToken + "/" + m_uploadFolderId;

    QByteArray boundaryRegular(QString("--" + QString::number(qrand(), 10)).toAscii());
    QByteArray boundary("\r\n--" + boundaryRegular + "\r\n");
    QByteArray boundaryLast("\r\n--" + boundaryRegular + "--\r\n");

    QUrl url(uploadUrl);

    QNetworkRequest request(url);
    request.setRawHeader("Host",            url.encodedHost());
    request.setRawHeader("User-Agent",      "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)");
    request.setRawHeader("Accept",          "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    request.setRawHeader("Accept-Language", "en-us;q=0.7,en;q=0.3");
    request.setRawHeader("Accept-Encoding", "gzip,deflate");
    request.setRawHeader("Accept-Charset",  "windows-1251,utf-8;q=0.7,*;q=0.7");
    request.setRawHeader("Keep-Alive",      "300");
    request.setRawHeader("Connection",      "keep-alive");
    request.setRawHeader("Referer",         "https://box.net");
    request.setRawHeader("Content-Type",    QByteArray("multipart/form-data; boundary=").append(boundaryRegular));

    QByteArray mimedata1("--"+boundaryRegular+"\r\n");
    mimedata1.append("Content-Disposition: form-data; name=\"action\"\r\n\r\n");
    mimedata1.append("file_upload");
    mimedata1.append(boundary);
    mimedata1.append("Content-Disposition: form-data; name=\"sfile\"; filename=\""+fileName.toUtf8()+"\"\r\n");
    mimedata1.append("Content-Type: application/octet-stream\r\n\r\n");

    QString passw = "";
    QString descr = "";
    QByteArray mimedata2(boundary);
    //mimedata2.append("Content-Disposition: form-data; name=\"password\"\r\n\r\n");
    //mimedata2.append(passw.toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"description\"\r\n\r\n");
    mimedata2.append(descr.toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"agree\"\r\n\r\n");
    mimedata2.append("1");
    mimedata2.append(boundaryLast);

    m_currentUploadingName = fileName;

    m_upf = new QUpFile(fileName, mimedata1, mimedata2, this);
    if (!m_upf->openFile())
    {
        delete m_upf;
        m_upf = NULL;
        qDebug() << Q_FUNC_INFO << "cannot open file " << fileName;
        emit uploadFailed(fileName, error_open_file);
        m_currentUploadingName.clear();
        QTimer::singleShot(2000, this, SLOT(startUpload())); // retry other uploads.
        return;
    }
    emit uploadQueueChanged();

    qDebug() << Q_FUNC_INFO << "starting uploading "<< fileName;

    m_uploadsReply = m_networkManager->post(request, m_upf);
    connect(m_uploadsReply, SIGNAL(finished()), this, SLOT(uploadFinished()));
    connect(m_uploadsReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onUploadError(QNetworkReply::NetworkError)));
    connect(m_uploadsReply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
    connect(m_uploadsReply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(onUploadProgress(qint64, qint64)));
}

void BxNet::uploadFinished()
{
    m_uploadProgress = -1; //if it faield or not we stop counting the upload progress
    m_uploadsReply = NULL; //the upload is finished succesfull or not

    m_currentUploadingName.clear();
    closeFile();

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        QTimer::singleShot(1000, this, SLOT(startUpload())); // retry upload later
        return;
    }
    if (reply == NULL
            || !((reply->error() == QNetworkReply::NoError)
                 || (reply->error() == 2 && m_uploaded == m_total)))
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        QTimer::singleShot(1000, this, SLOT(startUpload())); // retry upload later
        return;
    }

    //a safe check to be sure nothing bad could happen
    if (!m_authentificated)
    {
        //the code can reach this branch if the user logs out imediatly afeter startup
        qDebug() << Q_FUNC_INFO << "emit notLoggedIn()";
        emit notLoggedIn();
        return;
    }

    QDomElement root;
    RESPONSE_STATUS status = responseStatus(reply, &root);
    qDebug() << Q_FUNC_INFO << "upload finished with status=" << status;

    if (status == wrong_auth_token)
    {
        m_authentificated = false; // something wrong with auth_token
    }

    if (reply->error() == 5)
    {
        status = upload_canceled;
    }
    if (reply->error() == 2 && m_uploaded == m_total)
    {
        qDebug() << Q_FUNC_INFO << "Server closed connection (error #2), but file was uploaded";

        status = upload_ok;
    }

    if (status == upload_ok)
    {
        QDomElement element = root.firstChildElement("files");
        element = element.firstChildElement("file");
        QString fileName = element.attribute("file_name");
        QString id = element.attribute("id");

        //request sharing
        if (!id.isEmpty() && !fileName.isEmpty())
        {
            shareFile(id, fileName);
            emit uploadComplete(fileName);
        }

        return;
    }

    emit uploadFailed(currentUpload(), status);
    qDebug() << Q_FUNC_INFO << "trying to start uploading again";

    QTimer::singleShot(500, this, SLOT(startUpload())); // upload next
}

void BxNet::shareFile(const QString& fileId, const QString& fileName)
{
    qDebug() << Q_FUNC_INFO << "shareFile " << fileName;

    Q_ASSERT(m_authentificated);
    if (!m_authentificated)
    {
        qDebug() << Q_FUNC_INFO << "emit notLoggedIn()";
        emit notLoggedIn();
        return;
    }

    QString request;

    if (m_https)
    {
        request = "https://www.box.net/api/1.0/rest?action=public_share&api_key="+m_apiKey+"&auth_token="+m_authToken+"&target=file&target_id=" + fileId + "&password=&message=&emails=";
    }
    else
    {
        request = "http://www.box.net/api/1.0/rest?action=public_share&api_key="+m_apiKey+"&auth_token="+m_authToken+"&target=file&target_id=" + fileId + "&password=&message=&emails=";
    }

    qDebug() << Q_FUNC_INFO << "sharing file " << fileName << " id=" << fileId;

    QNetworkReply* reply = m_networkManager->get(QNetworkRequest(QUrl(request)));
    securelyErase(request);

    reply->setProperty("file_name", fileName);
    connect(reply, SIGNAL(finished()), this, SLOT(shareFileFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onShareError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));   
}

void BxNet::shareFileFinished()
{
    qDebug() << Q_FUNC_INFO << "share File finished";

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

    QDomElement root;
    RESPONSE_STATUS status = responseStatus(reply, &root);
    qDebug() << Q_FUNC_INFO  << "sharing finished with status=" << status;

    switch(status)
    {
    case share_ok:
        {
            const QDomElement el = root.firstChildElement("public_name");
            const QString public_name = el.text();
            QString link;
            if(m_https)
            {
                link = "https://www.box.net/shared/";
            }
            else
            {
                link = "http://www.box.net/shared/";
            }

            link += public_name;

            const QVariant variant = reply->property("file_name");
            if (!variant.isValid())
            {
                qDebug() << Q_FUNC_INFO << "Invalid file_name in reply share";
                return;
            }
            const QString fileName=variant.toString();

            emit addUploadToLog(fileName, link);
            emit shareSucceded(fileName, link);
            break;
        }

    case not_logged_in:
        {
            logout();
            break;
        }
    default:
        {
            emit shareFailed(status);
            break;
        }
    }

    qDebug() << Q_FUNC_INFO << "trying to start uploading again";

    QTimer::singleShot(500, this, SLOT(startUpload())); // upload next
}

void BxNet::onAuthError(QNetworkReply::NetworkError error)
{
    m_ticket.clear();
    m_maxUploadSize = 0;

    onError(error);

    emit authError(network_error);
}

void BxNet::onUploadError(QNetworkReply::NetworkError error)
{
    if (error != 5 && !(error == 2 && m_uploaded == m_total)) // TODO: why sometimes I get this error ?
    {
        onError(error);
        emit uploadFailed(currentUpload(), network_error);
    }

    emit uploadQueueChanged();
}

void BxNet::onShareError(QNetworkReply::NetworkError error)
{
    qDebug() << Q_FUNC_INFO << "shareError " << error;

    onError(error);

    emit shareFailed(network_error);
}

void BxNet::onCreateFolderError(QNetworkReply::NetworkError error)
{
    m_createFolderHelper.clear();

    onError(error);

    emit createFolderError(network_error);
}

void BxNet::onError(QNetworkReply::NetworkError error)
{
    if (m_checkingAuthToken)
    {
        m_checkingAuthToken = false;
    }

    // if error == 5 then upload was stopped. this is not an error
    if (error != 5)
    {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        Q_ASSERT(reply);
        const QString errorString = reply ? reply->errorString() : "empty reply";

        qDebug() << Q_FUNC_INFO << "QNetworkReply error: " << error << " " << errorString;

       // if (error != QNetworkReply::NoError)
       // {
       //     m_authentificated = false;
       // }

        emit networkError(error, errorString);
    }    
}

void BxNet::onSilentError(QNetworkReply::NetworkError error)
{
    if (m_checkingAuthToken)
    {
        m_checkingAuthToken = false;
    }

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    const QString errorString = reply ? reply->errorString() : "empty reply";

    qDebug() << Q_FUNC_INFO << "QNetworkReply error: " << error << " " << errorString;
}


void BxNet::createFolder(QString folderName, const QString& parentId, const QString additionalPart)
{   
    Q_ASSERT(m_authentificated);
    if (!m_authentificated)
    {
        qDebug() << Q_FUNC_INFO << "emit notLoggedIn()";
        emit notLoggedIn();
        return;
    }

    // special handling of empty folderName

    if ((folderName == "") && (parentId == "0"))
    {
        m_uploadFolder = "";
        m_uploadFolderId = "0";
        saveSettings();
        emit folderCreated(m_uploadFolder, m_uploadFolderId);
        return;
    }


    Q_ASSERT(m_createFolderHelper.isEmpty());
    if (!m_createFolderHelper.isEmpty())
    {
        return;
    }

    folderName.replace("/", "\\");

    QStringList parts = folderName.split("\\");
    if (parts.size() > 1)
    {
        // bleat' nogi.
        createFolder(parts.at(0), parentId, folderName.right(folderName.length() - parts.at(0).length() - 1));
        return;
    }
    m_createFolderHelper = additionalPart;

    //"https://www.box.net/api/1.0/rest?action=create_folder&api_key="+appKey+"&auth_token="+auth_token+"&parent_id="+parentID+"&name="+folderName+"&share=0"
    QString request = command(QStringList()
                                    << apiKeyParam()
                                    << "action=create_folder"
                                    << "auth_token=" + m_authToken
                                    << "parent_id=" + parentId
                                    << "name=" + folderName
                                    << "share=0");
    const QString requestUrl = apiUrl(m_https, RequestRest);

    qDebug() << Q_FUNC_INFO << "creating folder: " << folderName << " parentId=" << parentId;

    QNetworkReply* reply = m_networkManager->get(QNetworkRequest(QUrl(requestUrl + "?" + request)));
    securelyErase(request);

    connect(reply, SIGNAL(finished()),this,SLOT(createFolderFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onCreateFolderError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
}

void BxNet::createFolderFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

    //a safe check to be sure nothing bad could happen
    if (!m_authentificated)
    {
        //the code can reach this branch if the user logs out imediatly afeter startup
        qDebug() << Q_FUNC_INFO << "emit notLoggedIn()";
        emit notLoggedIn();
        return;
    }

    QDomElement root;
    RESPONSE_STATUS status = responseStatus(reply, &root);
    qDebug() << Q_FUNC_INFO << "create folder finished with status=" << status;

    switch(status)
    {
    case create_ok:
    case s_folder_exists:
        {
            //we need the folder id
            QDomElement el = root.firstChildElement("folder");
            el = el.firstChildElement("folder_id");

            const QString createdId = el.text();
            if (m_createFolderHelper.isEmpty())
            {
                m_uploadFolderId = createdId;
                saveSettings();
                emit folderCreated(m_uploadFolder, m_uploadFolderId);
            }
            else
            {
                const QString wantedFolder = m_createFolderHelper;
                m_createFolderHelper.clear();
                createFolder(wantedFolder, createdId);
            }
            break;
        }
    case wrong_auth_token:
    case not_logged_in:
        {
            logout();
            break;
        }
    default:
        {
            emit createFolderError(status);
            break;
        }
    }

}

QString BxNet::apiKeyParam() const
{
    if (m_apiKey.isEmpty())
    {
        return QString();
    }

    return "api_key=" + m_apiKey;
}

QString BxNet::apiUrl(bool https, RequestType type, const QString& path)
{
    QString command = https ? "https://" : "http://";
    command += "www.box.net/api/1.0";
    switch(type)
    {
    case RequestXML:
        command += "/xml";
        break;
    case RequestRest:
        command += "/rest";
        break;
    case RequestEmpty:
        break;
    }
    if (!path.isEmpty())
    {
        command += "/" + path;
    }
    return command;
}

QString BxNet::command(QString& param, RequestType type) const
{
    QStringList list;
    list << apiKeyParam()
         << param;
    return command(list, type);
}

QString BxNet::command(const QString& param, RequestType type) const
{
    QStringList list;
    list << apiKeyParam()
         << param;
    return command(list, type);
}

QString BxNet::command(QStringList& params, RequestType type) const
{
    if (type == RequestRest)
    {
        QString command;
        for(int i = 0; i < params.size(); ++i)
        {
            QString& param = params[i];
            if (!param.isEmpty())
            {
                if (!command.isEmpty())
                {
                    command += "&";
                }
                command += param;
                securelyErase(param);
            }
        }
        return command;
    }
    else if (type == RequestXML)
    {
        QDomDocument doc;
        QDomElement root = doc.createElement("request");
        doc.appendChild(root);

        foreach(const QString& param, params)
        {
            if (!param.isEmpty())
            {
                QStringList parts = param.split("=");
                if (parts.count() >= 2)
                {
                    QDomElement arg = doc.createElement(parts.at(0));
                    root.appendChild(arg);
                    QDomText argValue = doc.createTextNode(parts.at(1));
                    arg.appendChild(argValue);
                }
            }
        }
        return doc.toString();
    }
    return QString();
}

bool BxNet::isLoadAvatarEnabled() const
{
    return m_loadAvatar;
}

void BxNet::enableLoadAvatar(bool value)
{
    m_loadAvatar = value;
}

void BxNet::setApiKey(const QString& apiKey)
{
    m_apiKey = apiKey;
}

QString BxNet::apiKey()
{
    return m_apiKey;
}

void BxNet::onSslError(const QList<QSslError> & errors)
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
                emit authError(ssl_error);
                firstTime = false;
            }
        }
    }
}

void BxNet::loadAvatar(const QUrl& url)
{
    m_avatarUrl = url.toString();

    qDebug() << Q_FUNC_INFO << "loading avatar " << m_avatarUrl;

    QNetworkReply* reply = m_networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, SIGNAL(finished()), this, SLOT(avatarReplyFinished()));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));    
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onSilentError(QNetworkReply::NetworkError)));
}

void BxNet::avatarReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

    QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (redirect.isValid())
    {
        qDebug() << Q_FUNC_INFO << "loading avatar redirected";

        loadAvatar(redirect);
    }
    else
    {
        const QByteArray data(reply->readAll());
        if (data.size() > 0)
        {
            qDebug() << Q_FUNC_INFO << "avatar loaded";

            QPixmap pixmap;
            pixmap.loadFromData(data);
            changeUserIcon(pixmap);
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "received pixmap looks like nothing";
        }
    }
}

void BxNet::requestTicket()
{
    if (!m_ticket.isEmpty())
    {
        useTicketToLogin(m_ticket);
        return;
    }

    // QString request="https://www.box.net/api/1.0/rest?action=get_ticket&api_key="+this->appKey;

    QString request = command("action=get_ticket");
    const QString requestUrl = apiUrl(true);

    qDebug() << Q_FUNC_INFO << "requesting ticket";

    QNetworkReply* reply = m_networkManager->get(QNetworkRequest(QUrl(requestUrl + "?" + request)));

    connect(reply, SIGNAL(finished()), this, SLOT(getTicketFinished()));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onAuthError(QNetworkReply::NetworkError)));
}

void BxNet::getTicketFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

    //    if(waitingForAutentification)
    //    {
    //        qDebug("manageTicketReplay skiped because waitingForAuthentification is set to true");
    //        return;
    //    }

    QDomElement root;
    RESPONSE_STATUS status = responseStatus(reply, &root);

    qDebug() << Q_FUNC_INFO << "get ticket with status=" << status;

    if(status != get_ticket_ok)
    {
        emit authError(replay_parssing_error);
        return;
    }

    const QDomNode child = root.firstChild().nextSibling();
    const QDomElement element = child.toElement();
    QString ticket = element.text();

    if (!m_ticket.isEmpty())
    {
        // already have ticket but open browser again
        ticket = m_ticket;
    }

    //open browser here
    useTicketToLogin(ticket);
}


void BxNet::useTicketToLogin(const QString &ticket)
{
    Q_ASSERT(!ticket.isEmpty());
    if (ticket.isEmpty())
    {
        //TODO: ???
        return;
    }

    m_ticket = ticket;
    QString url = "https://www.box.net/api/1.0/auth/" + m_ticket;
    if (!m_email.isEmpty())
    {
        // SSO login
        url = url + "?sso_user_login=" + m_email;
        openLoginForm(url);
    }
    else
    {
        //waitingForAutentification = true;

        bool openUrl = QDesktopServices::openUrl(url);
        if (!openUrl)
        {
            emit errorOpenBrowser(url);
        }
        //waitingForAutentification = true;

        waitingForAuthToken();
    }
}

void BxNet::openLoginForm(const QString& url)
{
    emit beginSSO();

    if (m_webLoginForm == NULL)
    {
        m_webLoginForm = new WebLoginForm(NULL);
        connect(m_webLoginForm, SIGNAL(onTicketId(QString)), this, SLOT(onTicketId(QString)));
    }

    centrateWidget(m_webLoginForm);
    m_webLoginForm->goToUrl(url);
}

void BxNet::onTicketId(const QString& ticketId)
{
    Q_ASSERT(m_webLoginForm);
    if (m_webLoginForm)
    {
        m_webLoginForm->close();
    }

    m_ticket = ticketId;
    requestAuthToken();
}


void BxNet::waitingForAuthToken()
{
    qDebug() << "waiting for auth token...";

    m_waitingForAuthToken = true;
    requestAuthToken();
}

void BxNet::takeUsrToAuthorizePage()
{
	//QString url="https://www.box.com/api/oauth2/authorize"
	QNetworkReply* reply = NULL;

	const QString request = "https://www.box.com/api/oauth2/authorize?response_type=code&client_id=" + m_apiKey + "&redirect_uri=http://127.0.0.1&state=authenticated";
    
	reply = m_networkManager->get(QNetworkRequest(QUrl(request)));

    Q_ASSERT(reply);
    if (reply != NULL)
    {
        connect(reply, SIGNAL(finished()), this, SLOT(takeUsrToAuthorizePageDone()));
        connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));        
    }

}

void BxNet::takeUsrToAuthorizePageDone()
{

	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);

    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

	QString resp = reply->readAll();

	ofstream fos("pauthorize.html");
	if(fos.is_open())
	{
		fos<<static_cast<std::string>(resp);
	}
	fos.close();


	QWebView* view = new QWebView();	
	view->load(QUrl("pauthorize.html"));
	view->show();	
	m_AuthorizationPage = view;

	connect(view, SIGNAL(urlChanged(const QUrl&)), this, SLOT(onFiniAllowDeny(const QUrl&)));
    
}

void BxNet::onFiniAllowDeny(const QUrl& url)
{
	qDebug() << Q_FUNC_INFO << QString(url);	
	QWebView* existingView = qobject_cast<QWebView*>(sender());
	
	connect(existingView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFini(bool)));
}


void BxNet::onLoadFini(bool ok)
{
	QWebView* existingView = qobject_cast<QWebView*>(sender());	

	qDebug() << Q_FUNC_INFO << QString(ok);	
	
	qDebug() << Q_FUNC_INFO << QString(existingView->url());	
	bool foundAllow = existingView->findText(QString("Allow"));
	if(foundAllow == true)
	{
		qDebug() << Q_FUNC_INFO << "Allow Access Page has loaded";
			

		//connect(existingView, SIGNAL(loadStarted()), this, SLOT(onLinkClicked()));
		QNetworkAccessManager* qnet = existingView->page()->networkAccessManager();

		connect(qnet, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLinkClicked(QNetworkReply*)) );

		//connect(existingView, SIGNAL(linkClicked(const QUrl&)), this, SLOT(onLinkClicked(const QUrl&)));
		//qDebug() << Q_FUNC_INFO << "Allow Access Page has loaded";
		//existingView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
		
	}	
}

void BxNet::onLinkClicked(QNetworkReply* rep)
{	

	QVariant possibleRedirectUrl =
	         rep->attribute(QNetworkRequest::RedirectionTargetAttribute);
	
	QString allUrls = 	QString(possibleRedirectUrl.toUrl().toString());
	std::string theAuthCode;

	if(allUrls.contains(QString("state=authenticated&code=")))
	{
		theAuthCode = static_cast<std::string>(allUrls);
		theAuthCode = theAuthCode.substr(theAuthCode.find_last_of("=")+1,theAuthCode.length()-1);
		qDebug() << Q_FUNC_INFO << QString(possibleRedirectUrl.toUrl().toString());	
		m_AuthenticationCode = theAuthCode;
		getOAuth2AccessToken();
	}	

}

std::string BxNet::getOAuth2AuthenticationCode()
{
	return m_AuthenticationCode;
}

void BxNet::getOAuth2AccessToken()
{
	//webview to be closed here
	if(m_AuthorizationPage!=0)
	{
		m_AuthorizationPage->close();
		//delete m_AuthorizationPage;
	}
		
	QString requrl="https://www.box.com/api/oauth2/token";
	QNetworkReply* reply = NULL;
	
	QString AuthCode = QString(m_AuthenticationCode.c_str());
	QString cid = QString(BOXNET_API_KEY);
	QString csecret = QString(BOXNET_CLIENT_SECRET);

	const QString request = command(QStringList()
                                    << "grant_type=authorization_code"
                                    << "code="+AuthCode
                                    << "client_id=" + cid
                                    << "client_secret=" + csecret, RequestRest);     	

		QNetworkRequest qnr = QNetworkRequest(QUrl(requrl));
		qnr.setRawHeader("Accept","application/json");
		reply = m_networkManager->post(qnr, request.toUtf8());     

		Q_ASSERT(reply);
		if (reply != NULL)
		{
			connect(reply, SIGNAL(finished()), this, SLOT(getOAuth2AccessTokenDone()));			
			connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onAuthError(QNetworkReply::NetworkError)));
		}

}

void BxNet::getOAuth2AccessTokenDone()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);

    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }

	QString response(reply->readAll());
	qDebug() << Q_FUNC_INFO << "OAuth2 response: " << response;

	
		std::string acc_token = std::string(static_cast<const char*>(response));
		if(acc_token.find("error") == std::string::npos)
		{
		std::string ref_token;
		int pos1 = acc_token.find("token")+8;
		int pos2 = acc_token.find("expires")-4;
		int pos3 = acc_token.find("refresh")+16;
		int pos4 = acc_token.find("token_type")-4;

		m_acc_token = acc_token.substr(pos1,pos2-pos1+1);
		m_ref_token = acc_token.substr(pos3,pos4-pos3+1);

		if (reply == NULL || reply->error() != QNetworkReply::NoError)
		{
			qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
			return;
		}
	}
	else
		return;

	//get user's folder

	/*curl https://www.box.com/api/2.0/folders/0 \
	 -H "Authorization: Bearer T9cE5asGnuyYCCqIZFoWjFHvNbvVqHjl"*/


	QUrl requrl = QUrl("https://www.box.com/api/2.0/folders/0");
	
	std::string token = m_acc_token;
	QString hdrval = "Bearer "+QString(token.c_str());

	QNetworkRequest qnr(requrl);
	qnr.setRawHeader("Authorization",hdrval.toUtf8());

	
/*	QByteArray postData;
	

	std::string token = m_acc_token;
	QString hdr = "Authorization: Bearer "+QString(token.c_str());

	postData.append(hdr);	
	QNetworkRequest qnr(requrl);*/

	QNetworkReply* areply = NULL;    
	areply = m_networkManager->get(qnr);


	Q_ASSERT(areply);
	
	if (areply != NULL)
	{
		connect(areply, SIGNAL(finished()), this, SLOT(getUserFolderDone()));			
		connect(areply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onAuthError(QNetworkReply::NetworkError)));
	}

}

void BxNet::getUserFolderDone()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);	

    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
	else
	{
		QString response(reply->readAll());
		qDebug() << Q_FUNC_INFO << "Folder req response: " << response;
	}

	// no error received?
    if (reply->error() == QNetworkReply::NoError)
    {        
 
        // Example 2: Reading bytes form the reply
		QByteArray bytes = reply->readAll();  // bytes
        QString stng(bytes); // string
    }
	QString resp = reply->readAll();
	doCreateSmvFolder();

}

void BxNet::doCreateSmvFolder()
{
	
	QUrl requrl = QUrl("https://api.box.com/2.0/files/content");
	
	std::string token = m_acc_token;

	QString hdrval = "Bearer "+QString(token.c_str());

	QNetworkRequest qnr(requrl);
	qnr.setRawHeader("Authorization",hdrval.toUtf8());	

	QString boundary;
	boundary = "---------7d935033608e2";

	QByteArray data;    
    data.append("file=@btest.txt");
	data.append(boundary);
	data.append("folder_id=0");
	data.append(boundary);


	qnr.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/form-data; boundary=---------7d935033608e2");
	qnr.setHeader(QNetworkRequest::ContentLengthHeader,data.size());

	
    //No idea what this does
    //data.remove(0,1);

	QNetworkReply* areply = NULL; 
    areply = m_networkManager->post(qnr,data);

	Q_ASSERT(areply);
	
	if (areply != NULL)
	{
		connect(areply, SIGNAL(finished()), this, SLOT(SmvFileCreatedDone()));			
		connect(areply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onAuthError(QNetworkReply::NetworkError)));
	}

}

void BxNet::SmvFileCreatedDone()
{

	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);

	if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
	else
	{
		QString response(reply->readAll());
		qDebug() << Q_FUNC_INFO << "Folder req response: " << response;
	}

}

void BxNet::directLogin(const QString& name, const QString& password)
{
    //do not redirect the user browser to the authentification page
    //QString url="https://www.box.net/api/1.0/auth/"+ticket;

    RequestType type = RequestXML; //Using XML, becoause REST can't process emails with "+" character
    QNetworkReply* reply = NULL;

    if (type == RequestRest)
    {
        const QString encodedName = QString(QUrl::toPercentEncoding(name, "@", "+").constData());
        const QString encodedPass = QString(QCryptographicHash::hash(
                    (name + password).toLatin1()
                    , QCryptographicHash::Md5).toHex().constData());

        const QString request = command(QStringList()
                                    << apiKeyParam()
                                    << "action=authorization"
                                    << "login=" + encodedName
                                    << "password=" + encodedPass
                                    << "method=md5", type);
        const QString requestUrl = apiUrl(true, type);

        reply = m_networkManager->get(QNetworkRequest(QUrl(requestUrl + "?" + request)));
    }
    else if (type == RequestXML)
    {
        const QString encodedPass = QString(QCryptographicHash::hash(
                    (name + password).toLatin1()
                    , QCryptographicHash::Md5).toHex().constData());

        const QString request = command(QStringList()
                                    << apiKeyParam()
                                    << "action=authorization"
                                    << "login=" + name
                                    << "password=" + encodedPass
                                    << "method=md5", type);
        const QString requestUrl = apiUrl(true, type);
        reply = m_networkManager->post(QNetworkRequest(QUrl(requestUrl)), request.toUtf8());
    }

    qDebug() << Q_FUNC_INFO << "direct login " << name;

    Q_ASSERT(reply);
    if (reply != NULL)
    {
        connect(reply, SIGNAL(finished()), this, SLOT(directLoginFinished()));
        connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onAuthError(QNetworkReply::NetworkError)));
    }
}

void BxNet::directLoginFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);

    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

    QDomElement root;
    RESPONSE_STATUS status = responseStatus(reply, &root);

    qDebug() << Q_FUNC_INFO << "direct login finished with status=" << status;

    if(status != logged)
    {
        emit authError(status);
        return;
    }

    QDomNode child = root.firstChild().nextSibling();
    QDomElement element = child.toElement();
    m_authToken = element.text();

    child   = child.nextSibling();
    element = child.toElement();
    //this is the user element, we get it,s elements
    QDomElement elemUser;
    elemUser = element.firstChildElement("login");
    m_login  = elemUser.text();

    elemUser = element.firstChildElement("email");
    m_email  = elemUser.text();

    //elemUser   = element.firstChildElement("access_id");
    //m_accessId = elemUser.text();

    elemUser = element.firstChildElement("user_id");
    m_userId = elemUser.text();

    elemUser      = element.firstChildElement("space_amount");
    m_spaceAmount = elemUser.text().toDouble();

    elemUser    = element.firstChildElement("space_used");
    m_spaceUsed = elemUser.text().toDouble();

    elemUser = element.firstChildElement("max_upload_size");
    m_maxUploadSize = elemUser.text().toDouble();

    m_authentificated = true;

    emit accountInfoCompleted();
    onFinishAuth();

    // get more user info
    getUserInfo();
}

void BxNet::getAccountInfo(bool silent)
{
    if (authentificated() && isUploading() && silent) // don't allow get account info by timer if uploading active
    {
        return;
    }

    QString request = command(QStringList()
                                        << apiKeyParam()
                                        << "action=get_account_info"
                                        << "auth_token=" + m_authToken
                                        << "user_id=" + m_userId);
    const QString requestUrl = apiUrl(m_https, RequestRest);
    request = requestUrl + "?" + request;

    qDebug() << Q_FUNC_INFO << "getting account info";

    QNetworkReply* reply = m_networkManager->get(QNetworkRequest(QUrl(request)));
    securelyErase(request);

    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
    if (silent)
    {
        connect(reply, SIGNAL(finished()), this, SLOT(getAccountInfoFinishedSilent()));
    }
    else
    {
        connect(reply, SIGNAL(finished()), this, SLOT(getAccountInfoFinished()));
        if (!authentificated()) // try to fix #3 error at startup
        {
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        }
        else
        {
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onSilentError(QNetworkReply::NetworkError)));
        }
    }
}

void BxNet::getAccountInfoFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);

    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

    QDomElement root;
    RESPONSE_STATUS status = responseStatus(reply, &root);

    qDebug() << Q_FUNC_INFO << "get account info finished with status=" << status;

    if (status != get_account_info_ok)
    {
        if (m_checkingAuthToken)
        {
            onGetAccountInfoFailed();
            m_checkingAuthToken = false;
        }
        else
        {
            emit authError(status);
        }
        return;
    }

    QDomNode child = root.firstChild().nextSibling();
    QDomElement element = child.toElement();

    QDomElement elemUser;
    elemUser = element.firstChildElement("space_amount");
    const QString space_amount = elemUser.text();

    elemUser = element.firstChildElement("space_used");
    const QString space_used = elemUser.text();

    elemUser = element.firstChildElement("max_upload_size");
    const QString max_upload_size = elemUser.text();

    m_maxUploadSize = max_upload_size.toDouble();
    m_spaceAmount   = space_amount.toDouble();
    m_spaceUsed     = space_used.toDouble();

    if (m_checkingAuthToken)
    {
        onGetAccountInfoCompleted();
        m_checkingAuthToken = false;

        getUserInfo(); // getting avatar and possible other info
    }

    emit accountInfoCompleted();

}

void BxNet::getAccountInfoFinishedSilent()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

    QDomElement root;
    RESPONSE_STATUS status = responseStatus(reply, &root);

    qDebug() << Q_FUNC_INFO << "get account info finished with status=" << status;

    if (status == get_account_info_ok)
    {
        QDomNode child = root.firstChild().nextSibling();
        QDomElement element = child.toElement();

        QDomElement elemUser;
        elemUser = element.firstChildElement("space_amount");
        const QString space_amount = elemUser.text();

        elemUser = element.firstChildElement("space_used");
        const QString space_used = elemUser.text();

        elemUser = element.firstChildElement("max_upload_size");
        const QString max_upload_size = elemUser.text();

        m_maxUploadSize = max_upload_size.toDouble();
        m_spaceAmount   = space_amount.toDouble();
        m_spaceUsed     = space_used.toDouble();

        emit accountInfoCompleted();

    }
}

void BxNet::getUserInfo()
{   
    QString request = command(QStringList()
                                        << apiKeyParam()
                                        << "action=get_user_info"
                                        << "auth_token=" + m_authToken
                                        << "user_id=" + m_userId);
    const QString requestUrl = apiUrl(m_https, RequestRest);

    qDebug() << Q_FUNC_INFO << "getting user info";

    QNetworkReply* reply = m_networkManager->get(QNetworkRequest(QUrl(requestUrl + "?" + request)));
    securelyErase(request);

    connect(reply, SIGNAL(finished()), this, SLOT(getUserInfoFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
}

void BxNet::getUserInfoFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

    QDomElement root;
    RESPONSE_STATUS status = responseStatus(reply, &root);

    qDebug() << Q_FUNC_INFO << "get user info finished with status=" << status;

    if (status != s_get_user_info)
    {
        //emit authError(status);
        return;
    }

    if (m_loadAvatar)
    {
        QDomNodeList nodes = root.elementsByTagName("avatar_url");
        if(nodes.count() != 0)
        {
            // get the first node
            loadAvatar(nodes.at(0).toElement().text());
        }
    }
}

void BxNet::requestAuthToken()
{
    //"https://www.box.net/api/1.0/rest?action=get_auth_token&api_key="+m_apiKey+"&ticket="+m_ticket

    const QString request = command(QStringList()
                                        << "action=get_auth_token"
                                        << apiKeyParam()
                                        << "ticket="+m_ticket);
    const QString requestUrl = apiUrl(true, RequestRest);

    qDebug() << Q_FUNC_INFO << "requesting auth token";

    const QString url = requestUrl + "?" + request;

    QNetworkReply* reply = m_networkManager->get(QNetworkRequest(QUrl(url)));
    connect(reply, SIGNAL(finished()), this, SLOT(getAuthTokenFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onAuthError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
}

void BxNet::getAuthTokenFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }

    QDomElement root;
    RESPONSE_STATUS status = responseStatus(reply, &root);

    qDebug() << Q_FUNC_INFO << "get auth token finished with status=" << status;

    if (m_waitingForAuthToken && (status == not_logged_in))
    {
        qDebug() << "not authentificated...";
        QTimer::singleShot(10000, this, SLOT(requestAuthToken())); // delay
        return;
    }

    if (status != get_auth_token_ok)
    {
        emit authError(status);
        return;
    }    

    QDomNode child = root.firstChild().nextSibling();
    QDomElement element = child.toElement();
    m_authToken         = element.text();

    child   = child.nextSibling();
    element = child.toElement();
    //this is the user element, we get it,s elements
    QDomElement elemUser;
    elemUser = element.firstChildElement("login");
    m_login  = elemUser.text();

    elemUser = element.firstChildElement("email");
    m_email  = elemUser.text();

    //elemUser   = element.firstChildElement("access_id");
    //m_accessId = elemUser.text();

    elemUser = element.firstChildElement("user_id");
    m_userId = elemUser.text();

    elemUser      = element.firstChildElement("space_amount");
    m_spaceAmount = elemUser.text().toDouble();

    elemUser    = element.firstChildElement("space_used");
    m_spaceUsed = elemUser.text().toDouble();

    elemUser        = element.firstChildElement("max_upload_size");
    m_maxUploadSize = elemUser.text().toDouble();

    securelyErase(m_ticket);
    m_authentificated = true;

    emit accountInfoCompleted();
    onFinishAuth();

    // get more user info
    getUserInfo();

    m_waitingForAuthToken = false;
}

void BxNet::onGetAccountInfoCompleted()
{
    if (m_checkingAuthToken)
    {
        // authToken is valid. we can process
        qDebug() << Q_FUNC_INFO << "auth token valid";

        onFinishAuth();
    }
}

void BxNet::onGetAccountInfoFailed()
{
    if (m_checkingAuthToken)
    {
        qDebug() << Q_FUNC_INFO << "auth token invalid";

        // authToken is invalid.
        //directLogin(m_login, m_password);
        qDebug() << Q_FUNC_INFO << "emit notLoggedIn()";
        emit notLoggedIn();
    }
}

/*
void BxNet::createBookmark(const QString& name, const QString& link)
{
    const QString request = command(QStringList()
                                        << "url=" + link
                                        << "name=" + name
                                        << "description=description"
                                        << "folder_id='0'"
                                        << "import_as=link");
    QString requestUrl = apiUrl(m_https, RequestEmpty, "import");

    LOGFILE  << "creating bookmark" << FLUSHLOG;

    QNetworkReply* reply = m_networkManager->get(QNetworkRequest(QUrl(requestUrl + "?" + request)));
    connect(reply, SIGNAL(finished()), this, SLOT(createBookmarkFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
}


void BxNet::createBookmarkFinished()
{
    QWebView* webView = qobject_cast<QWebView*>(sender());

    const QString response = webView->page()->mainFrame()->toPlainText();

    qDebug() << response;
}
*/

void BxNet::logout()
{
    qDebug() << Q_FUNC_INFO << "logout";

    stopCurrentUpload();

    if (!m_authToken.isEmpty() && m_authentificated)
    {
        QString request = command(QStringList()
                                        << apiKeyParam()
                                        << "action=logout"
                                        << "auth_token=" + m_authToken);
        const QString requestUrl = apiUrl(true, RequestRest);

        QNetworkReply* reply = m_networkManager->get(QNetworkRequest(QUrl(requestUrl + "?" + request)));
        securelyErase(request);

        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
        connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
    }

    m_authentificated    = false;
    clearSettings();
    saveAuth();

    saveSettings();
}

void BxNet::onFinishAuth()
{
    qDebug() << Q_FUNC_INFO << "authentificated";

    m_authentificated    = true;    

    //create the upload folder, this also tests if the user is logged in
    createFolder(uploadFolder());

    saveAuth(); // save immediately after sucessfull login
    //saveSettings();

    m_disconnected = false;

    emit authCompleted();

    // if addtoQueue was called before user authentificated, starting upload
    if (m_uploadingQueue.size() > 0)
    {
        QTimer::singleShot(2000, this, SLOT(startUpload())); // delay upload
    }
}

void BxNet::setUploadFolder(const QString& folderName)
{

    if (m_uploadFolder != folderName)
    {
        if (folderName.isEmpty() || folderName == "\\" || folderName == "/")
        {
            qDebug() << Q_FUNC_INFO << "set upload folder = \"\"";
            m_uploadFolder = "";
            m_uploadFolderId = "0";
        }

        qDebug() << Q_FUNC_INFO << "set upload folder =" << folderName;
        m_uploadFolder = folderName;
        if (m_authentificated)
        {
            createFolder(m_uploadFolder);
        }
    }
}


void BxNet::setUploadFolder(const QString& folderName, const QString& folderId)
{
    if (m_uploadFolder != folderName || m_uploadFolderId != folderId)
    {
        if (folderName.isEmpty() || folderName == "\\" || folderName == "/")
        {
            qDebug() << Q_FUNC_INFO << "set upload folder = \"\"";
            m_uploadFolder = "";
            m_uploadFolderId = "0";
        }

        qDebug() << Q_FUNC_INFO << "set upload folder =" << folderName << "folder id =" << folderId;
        m_uploadFolder      = folderName;
        m_uploadFolderId    = folderId;
    }
}

QString BxNet::uploadFolder() const
{
    return m_uploadFolder;
}

QString BxNet::uploadFolderId() const
{
    return m_uploadFolderId;
}

void BxNet::setSecured(bool https)
{
    m_https = https;
}

void BxNet::setRetryCount(int count)
{
    m_retryCount = count;
}

int BxNet::retryCount() const
{
    return m_retryCount;
}

bool BxNet::secured() const
{
    return m_https;
}

QString BxNet::userName() const
{
    return m_login;
}

QString BxNet::userId() const
{
    return m_userId;
}

bool BxNet::authentificated() const
{
    return m_authentificated;
}

QString BxNet::uploadDirectoryLink() const
{
    if (!authentificated())
    {
        return QString();
    }
    QString link = m_https ? "https://www.box.net/files/0/f/" : "http://www.box.net/files/0/f/";
    link += m_uploadFolderId + "/" + m_uploadFolder;
    //if (!m_authToken.isEmpty())
    //{
    //    link += "?auth_token=" + m_authToken;
    //}
    return link;
}

double BxNet::spaceAmount() const
{
    return m_spaceAmount;
}

double BxNet::spaceUsed() const
{
    return m_spaceUsed;
}

bool BxNet::checkAuth()
{
    if (m_authentificated)
    {
        qDebug() << Q_FUNC_INFO << "call checkAuth but already authentificated";
    }

    if (!m_checkingAuthToken)
    {
        // check if authToken valid.
        m_checkingAuthToken = true;
        getAccountInfo();
        return true;
    }
    return false;
}

bool BxNet::convertRegistryAuthSettings()
{
    if (loadFromOldSettings())
    {
        // clear old config and save new
        saveAuth();
        return true;
    }

    QSettings settings("BoxNet", "SimpleShare");

    m_authToken         = decryptXor(settings.value("auth/authToken", "").toString());
    m_login             = settings.value("auth/login", "").toString();
    m_email             = settings.value("auth/email", "").toString();
    m_userId            = decryptXor(settings.value("auth/userId", "").toString());

    if (!m_authToken.isEmpty())
    {
        saveAuth();
        return true;
    }
    return false;
}

void BxNet::loadAuth()
{
    const QString fileName = QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QDir::separator() + AUTH_FILE;

    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists())
    {
        if (convertRegistryAuthSettings())
        {
            QSettings settings("BoxNet", "SimpleShare");

            settings.setValue("auth/authToken", "");
            settings.setValue("auth/login",     "");
            settings.setValue("auth/email",     "");
            settings.setValue("auth/userId",    "");
            settings.setValue("auth/accessId",    "");
        }
    }

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream out(&file);

        QString storedString = out.readAll();
        file.close();

        QString password = generatePasswordForComputer();
        storedString = QString::fromAscii(decrypt(storedString.toAscii(), password));
        securelyErase(password);

        QStringList list = storedString.split("|");
        int count = list.count();
        if (count == 10 || count == 8)
        {
            int index = 1;
            m_authToken = list.at(index); index += 2;
            m_login     = list.at(index); index += 2;
            if (count == 8)
            {
                m_email = m_login;
            }
            else
            {
                m_email = list.at(index); index += 2;
            }
            //m_accessId  = list.at(index); index += 2;
            m_userId    = list.at(index); index += 2;
        }
        for(int i = 0; i < list.size(); ++i)
        {
            securelyErase(list[i]);
        }
        securelyErase(storedString);
    }
}

void BxNet::saveAuth()
{
    const QString dirName = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    const QString fileName = dirName + QDir::separator() + AUTH_FILE;

    setRandSeed();

    QString storeString;
    if (m_authToken.isEmpty())
    {
        storeString = randStr(256);
    }
    else
    {
        storeString   = randStr() + "|" + m_authToken
                + "|" + randStr() + "|" + m_login;

        if (m_email != m_login)
        {
            storeString = storeString
                + "|" + randStr() + "|" + m_email;
        }

        storeString = storeString
                //+ "|" + randStr() + "|" + m_accessId
                + "|" + randStr() + "|" + m_userId
                + "|" + randStr();

        int additionalLength = (8 - storeString.length() % 8) + 7;
        storeString = storeString + "|" + randStr(additionalLength);
    }

    QDir dir(dirName);
    dir.mkpath(dirName);

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);

        QString password = generatePasswordForComputer();
        out << QString::fromAscii(encrypt(storeString.toAscii(), password));
        securelyErase(password);

        securelyErase(storeString);

        file.close();
    }
}

void BxNet::loadSettings()
{
    if (loadFromOldSettings())
    {
        // clear old config and save new
        QSettings settings;
        settings.setValue("defaltuser", "");
        saveSettings();
    }

    QSettings settings;


    //m_authToken         = decrypt(settings.value("auth/authToken", "").toString());
    //m_login             = settings.value("auth/login", "").toString();
    ////m_email             = settings.value("auth/email", "").toString();
    ////m_accessId          = decrypt(settings.value("auth/accessId", "").toString());
    //m_userId            = decrypt(settings.value("auth/userId", "").toString());


    m_spaceAmount       = 0.0;
    m_spaceUsed         = 0.0;
    m_maxUploadSize     = 0.0;

    m_uploadFolder      = settings.value("options/uploadFolder", DEFAULT_UPLOAD_FOLDER).toString();;
    m_uploadFolderId    = settings.value("options/uploadFolderId", "").toString();;

    //m_spaceAmount       = settings.value("bxnet/spaceAmount", 0.0).toDouble();
    //m_spaceUsed         = settings.value("bxnet/spaceUsed", 0.0).toDouble();
    //m_maxUploadSize     = settings.value("bxnet/maxUploadSize", 0.0).toDouble();

    // loading proxy info

}

void BxNet::clearSettings()
{
    securelyErase(m_authToken);
    m_login.clear();
    m_email.clear();
    //m_accessId.clear();
    m_userId.clear();
    m_spaceAmount       = 0.0;
    m_spaceUsed         = 0.0;
    m_maxUploadSize     = 0.0;
    //do not reset upload folder name
    m_uploadFolderId    = "";
    //m_uploadFolder      = DEFAULT_UPLOAD_FOLDER;
}

void BxNet::saveSettings()
{
    QSettings settings;

    //settings.setValue("auth/authToken", encrypt(m_authToken));
    //settings.setValue("auth/login", m_login);
    ////settings.setValue("auth/email", m_email);
    ////settings.setValue("auth/accessId", encrypt(m_accessId));
    //settings.setValue("auth/userId", encrypt(m_userId));

    settings.setValue("options/uploadFolder", m_uploadFolder);
    settings.setValue("options/uploadFolderId", m_uploadFolderId);

    // don't save proxy settings becoause application uses Internet Explorer Settings for proxing
    // QNetworkProxy proxy = QNetworkProxy::applicationProxy();

    // settings.setValue("network/proxy", proxy.hostName());
    // settings.setValue("network/port", proxy.port());
    // settings.setValue("network/type", proxy.type());
    // settings.setValue("network/user", encrypt(proxy.user()));
    // settings.setValue("network/password", encrypt(proxy.password()));
}

bool BxNet::loadFromOldSettings()
{
    QSettings settings;

    if (!settings.contains("defaltuser"))
    {
        return false;
    }
    QVariant value = settings.value("defaltuser");
    if(!value.isValid())
    {
        return false;
    }

    QString str = value.toString();
    if (str.isEmpty())
    {
        return false;
    }

    QStringList parts = str.split("\n");

    m_authToken         = parts.at(0);
    m_login             = parts.at(1);
    m_email             = parts.at(2);
    //m_accessId          = parts.at(3);
    m_userId            = parts.at(4);
    m_spaceAmount       = parts.at(5).toDouble();
    m_spaceUsed         = parts.at(6).toDouble();
    m_maxUploadSize     = parts.at(7).toDouble();
    // never store password:
    //m_password          = parts.at(8);

    if (parts.length() > 9 && parts.at(9) != "")
    {
        m_uploadFolderId = parts.at(9);
    }
    else
    {
        m_uploadFolderId = "0";
    }

    return true;
}

void BxNet::readProxySettingsFromSystem()
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

QString BxNet::proxySettings() const
{
    QNetworkProxy proxy = QNetworkProxy::applicationProxy();
    return !proxy.hostName().isEmpty() ? proxy.hostName() + ":" +QString::number(proxy.port()) : "";
}

void BxNet::setProxySettings(const QString& proxy)
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

void BxNet::enableProxying(const QString& host, const QString& port,
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

void BxNet::ssoLogin(const QString &name)
{
    if (m_checkingAuthToken)
    {
        qDebug() << Q_FUNC_INFO << "trying to login while authenticating in progress";
        return;
    }

    qDebug() << Q_FUNC_INFO << "SSO login";

    m_email = name;

    requestTicket();
}


void BxNet::login()
{
    if (m_checkingAuthToken)
    {
        qDebug() << Q_FUNC_INFO << "trying to login while authenticating in progress";
        return;
    }

    qDebug() << Q_FUNC_INFO << "login";

    m_email = "";

    requestTicket();
}

int BxNet::uploadingQueueLength() const
{
    int result = m_uploadingQueue.count();

    return result;
}

bool BxNet::removeItemFromQueue(int index)
{
    if (index >= 0 && index < m_uploadingQueue.count())
    {
        m_uploadingQueue.removeAt(index);
        emit uploadQueueChanged();
        return true;
    }

    return false;
}

QString BxNet::currentUpload() const
{
    if (m_upf != NULL)
    {
        return m_currentUploadingName;
    }
    else
    {
        return "";
    }
}

QString BxNet::uploadingQueueItem(int index) const
{
    if (index >= 0 && index < m_uploadingQueue.count())
    {
        return m_uploadingQueue.at(index);
    }

    return "";
}

void BxNet::stopCurrentUpload()
{
    if (m_uploadsReply)
    {
        m_uploadsReply->abort();
        m_uploadsReply = NULL;
        //closeFile();

        if (m_uploadingQueue.size() > 0)
        {
            startUpload(); //force next upload
        }
        else
        {
            emit uploadQueueChanged();
        }
    }
}

bool BxNet::isUploading() const
{
    return m_uploadsReply != NULL;
}

QIcon& BxNet::getFileTypeIcon(int fileType)
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

int BxNet::maxUploadFileSizeLimit() const
{
    return m_maxUploadSize;
}

/////////////////////////////////

QString BxNet::treeXmlPath() const
{
    return m_xmlPath;
}

void BxNet::readTree(int folderId, bool folders, bool onelevel)
{
    qDebug() << Q_FUNC_INFO << "reading tree";

    m_xmlPath = "";
    QTemporaryFile* file = new QTemporaryFile(QDir::tempPath() + QDir::separator() + "boxtree");
    if (file && file->open())
    {
        QStringList commandArgs;
        commandArgs << apiKeyParam()
                    << "action=get_account_tree"
                    << "auth_token=" + m_authToken
                    << "folder_id=" +QString::number(folderId)
                    << "params[]=simple";
        if (folders)
        {
            commandArgs << "params[]=nofiles";
        }
        if (onelevel)
        {
            commandArgs << "params[]=onelevel";
        }

        QString request = command(commandArgs, RequestRest);
        const QString requestUrl = apiUrl(m_https, RequestRest);

        QNetworkReply* reply = m_networkManager->post(QNetworkRequest(QUrl(requestUrl)), request.toUtf8());
        securelyErase(request);

        Q_ASSERT(reply);
        if (reply != NULL)
        {
            reply->setProperty("file", reinterpret_cast<quint64>(file));

            connect(reply, SIGNAL(readyRead()), this, SLOT(readTreeReadyRead()));
            connect(reply, SIGNAL(finished()), this, SLOT(readTreeFinished()));
            connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslError(const QList<QSslError>&)));
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onReadTreeError(QNetworkReply::NetworkError)));
        }

        m_xmlPath = file->fileName();
    }
}

void BxNet::readTreeReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }
    QFile* file = reinterpret_cast<QFile*>(reply->property("file").toLongLong());
    if (file == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply without file";
        return;
    }

    size_t size = file->write(reply->readAll());

    qDebug() << size << " bytes writed to" << file->fileName();
}

void BxNet::readTreeFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply is NULL";
        return;
    }
    if (reply == NULL || reply->error() != QNetworkReply::NoError)
    {
        qDebug() << Q_FUNC_INFO << "reply error: " << reply->error();
        return;
    }
    QFile* file = reinterpret_cast<QFile*>(reply->property("file").toLongLong());
    if (file == NULL)
    {
        qDebug() << Q_FUNC_INFO << "reply without file";
        return;
    }

    file->seek(0);

    // Parse file:

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(file, false, &errorStr, &errorLine, &errorColumn))
    {
        qDebug() << Q_FUNC_INFO << "XML Error: Parse error at line " << errorLine << ", " << "column " << errorColumn << ": " << qPrintable(errorStr);
        return;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "response")
    {
        qDebug() << Q_FUNC_INFO << "XML Error: Not a response file" << "\n";
        return;
    }

    const QDomNode child = root.firstChild();
    if (child.isNull())
    {
        qDebug() << Q_FUNC_INFO << "XML Error: child is NULL";
        return;
    }

    const QDomElement element = child.toElement();
    if (element.tagName() != "status")
    {
        qDebug() << Q_FUNC_INFO << "XML Error: node name not a \"status\"\n";
        return;
    }

    const QString statusStr = element.text();

    RESPONSE_STATUS status = responseStatusFromString(statusStr);

    qDebug() << Q_FUNC_INFO << "reading tree finished with status=" << status;

    if(status != listing_ok)
    {
        //TODO: what to emit here
        //emit networkError(status);
        return;
    }

    parseTree(root);

    QFileInfo info(file->fileName());
    file->close();
    file->deleteLater();

    info.dir().remove(info.fileName());
}

void BxNet::onReadTreeError(QNetworkReply::NetworkError error)
{
    onSilentError(error);
}

void BxNet::parseTree(const QDomElement& root)
{
    qDebug() << "parsing Tree. root name=" << root.tagName();

    for(int i = 0; i < root.childNodes().count(); ++i)
    {
        QDomNode node = root.childNodes().at(i);
        if (!node.isNull())
        {
            QDomElement element = node.toElement();
            //qDebug() << "> node name =" << element.tagName();
            //qDebug() << "> node text =" << element.text();

            if (element.tagName() == "tree")
            {
                // zipped base64 binary data.... this is SPARTA!

                QByteArray xcode("");
                xcode.append(element.text().toLatin1());

                QTemporaryFile buffer(QDir::tempPath() + QDir::separator() + "boxtreebuff");
                if (!buffer.open())
                {
                    return;
                }
                buffer.write(QByteArray::fromBase64(xcode));
                buffer.seek(0);

                QZip::QZipReader unzip(&buffer);

                QTemporaryFile file(QDir::tempPath() + QDir::separator() + "unzip");
                if (!file.open())
                {
                    return;
                }
                const QString fileName = QFileInfo(file.fileName()).fileName();

                const QString tempDir = QDir::tempPath();
                QDir tmp(tempDir);

                const QString destDir = tmp.path() + QDir::separator() + "box" + fileName;
                file.close();
                tmp.remove(fileName);
                deletePath(destDir);
                tmp.mkpath(destDir);

                unzip.extractAll(destDir);

                tmp.remove(buffer.fileName());

                // process destDir...

                parseTreeFromDir(destDir);

                deletePath(destDir);
                tmp.rmdir(destDir);

            }
        }
    }
}

void BxNet::parseTreeFromDir(const QString& path)
{
    if (path.isEmpty())
    {
        return;
    }


    QDir dir(path);

    if (dir.exists())
    {
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
        foreach(const QFileInfo& fileInfo, dir.entryInfoList())
        {
            if (fileInfo.isDir())
            {
                parseTreeFromDir(fileInfo.absoluteFilePath());
            }
            else
            {
                parseTreeFromFile(fileInfo.filePath());
            }
        }
    }
}

void fixXMLfile(const QString& path)
{
    QFileInfo info(path);
    QTemporaryFile tmpFile(info.dir().path() + QDir::separator() + "xml");
    QString tmpFileName;
    if (tmpFile.open())
    {
        tmpFileName = tmpFile.fileName();
        tmpFile.write("<?xml version='1.0' encoding='UTF-8' ?><root>");

        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
        {
            while(true)
            {
                char buff[4096];
                size_t sz = file.read(buff, sizeof(buff));
                if (sz > 0)
                {
                    tmpFile.write(buff, sz);
                }
                else
                {
                    break;
                }
            }

            file.close();
        }
        tmpFile.write("</root>");
        tmpFile.close();
    }

    renameFile(tmpFileName, info.filePath());

}

void BxNet::parseTreeFromFile(const QString& path)
{
    fixXMLfile(path);

    QFile file(path);
    qDebug() << "processing " << path;

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    {
        qDebug() << Q_FUNC_INFO << "XML Error: Parse error at line " << errorLine << ", " << "column " << errorColumn << ": " << qPrintable(errorStr);
        return;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "root")
    {
        qDebug() << Q_FUNC_INFO << "XML Error: Not a folder xml element" << "\n";
        return;
    }

    QDomElement folder = root.firstChildElement("folder");
    if (!folder.isNull())
    {
        parseFolder(folder, "0");
    }

    emit treeFinished();
}

void BxNet::parseFolder(const QDomElement& folder, QString parentId)
{   
    if (folder.isNull())
    {
        return;
    }

    const QString id = folder.attribute("id");
    //qDebug() << "folderName = " << folder.attribute("name");
    emit treeFolder(id, parentId, folder.attribute("name"), "");


    for(int i = 0; i < folder.childNodes().count(); ++i)
    {
        QDomElement element = folder.childNodes().at(i).toElement();

        if (element.tagName() == "folders")
        {
            for(int j = 0; j < element.childNodes().count(); ++j)
            {
                QDomElement f = element.childNodes().at(j).toElement();
                if (f.tagName() == "folder")
                {
                    parseFolder(f, id);
                }
            }
        }

        if (element.tagName() == "files")
        {
            for(int j = 0; j < element.childNodes().count(); ++j)
            {
                QDomElement f = element.childNodes().at(j).toElement();
                if (f.tagName() == "file")
                {
                    parseFile(f, id);
                }
            }
        }
    }

}

void BxNet::parseFile(const QDomElement& file, QString parentId)
{
    if (file.isNull())
    {
        return;
    }

    //qDebug() << "fileName = " << file.attribute("file_name");
    emit treeFile(file.attribute("id"), parentId, file.attribute("file_name"), "");
}

QString BxNet::linkTo(const QString& folderId) const
{
    QString prefix = m_https ? "https://" : "http://";
    prefix += "www.box.net/files/0";

    if (folderId == "0")
    {
        return prefix;
    }

    return prefix + "/f/" + folderId;
}

QString BxNet::linkTo(const QString& folderId, const QString& fileId) const
{
    return linkTo(folderId) + "/1/f_" + fileId;
}
