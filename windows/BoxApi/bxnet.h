#ifndef BXNET_H
#define BXNET_H
#include <QObject>
#include <QNetworkReply>
#include <QString>
#include <QHash>
#include <QList>
#include <QPair>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QStringList>
#include <QDomElement>
#include <QFile>
#include <QNetworkProxy>
#include <QMessageBox>

class QUpFile;
class UserSession;

class Settings;

class WebLoginForm;

#define DEFAULT_UPLOAD_FOLDER       "SimpleShare Uploads"
#define AUTH_FILE                   "box.net"

class BxNet: public QObject
{
    Q_OBJECT
public:
    enum RESPONSE_STATUS
    {
        // SUCCESS CODES:
        logged,                     //
        upload_ok,                  //
        get_ticket_ok,              //
        s_folder_exists,            // A folder withthe same name already exists in that location.
        create_ok,                  // folder created ok
        share_ok,                   //
        get_account_info_ok,        //
        s_get_user_info,            //
        get_auth_token_ok,          //
        listing_ok,                 // read_tree
        // FAIL CODES:
        not_logged_in,              // The user did not successfully authenticate on the page provided in the
        get_auth_token_error,       // Generic error for other invalid inputs.
        wrong_auth_token,           // The user is not logged into your application.  Your authentication_token is not valid.
        application_restricted,     // You provided an invalid api_key, or the api_key is restricted from calling this function.
        upload_some_files_failed,   // Some of the files were not successfully uploaded.
        not_enough_free_space,      // There is not enough space in this user's account to accommodate the new files.
        filesize_limit_exceeded,    // A file is too large to be uploaded to a user's account (Lite users have a 25 MB upload limit per file, premium users have a 1 GB limit per file)
        access_denied,              // The user does not have uploading privileges for that particular folder.
        upload_wrong_folder_id,     // The specified folder_id is not valid.
        upload_invalid_file_name,   // The name of the file contains invalid characters not accepted by Box.net.
        no_parent,                  // The folder_id provided is not a valid folder_id for the user's account.
        invalid_folder_name,        // The name provided for the new folder contained invalid characters or too many characters.
        e_no_access,                // The user does not have the necessary permissions to perform the specified operation.  Most likely the user is trying to create a folder within a collaborated folder, for which the user has view-only permission.
        e_no_folder_name,           // A folder name was not properly provided.
        folder_name_too_bid,        // The folder name contained more than 100 characters, exceeding the folder name length limit.
        e_input_params,             // Another invalid input was provided (example: an invalid value for the 'share' parameter).
        invalid_login,              //
        login_limit,                // login limit reached
        ssl_error,                  // SSL handshake error
        e_folder_id,                // for read_tree
        //applications own status errors
        error_open_file,            //
        network_error,              //
        replay_parssing_error,      // the server replay is not valid and the parsing faield
        upload_canceled,
        //
        unknown_status = -1
    };   

    enum RequestType
    {
        RequestEmpty = 0,
        RequestRest,
        RequestXML
    };

public:
    BxNet();
    ~BxNet();

public:
    void readProxySettingsFromSystem();
    void enableProxying(const QString& host, const QString& port = "",
                        QNetworkProxy::ProxyType type = QNetworkProxy::HttpProxy,
                        const QString& username = "", const QString& password = "");

    QString proxySettings() const;
    void setProxySettings(const QString& proxy);

    void setApiKey(const QString& apiKey);
    QString apiKey();
    bool isLoadAvatarEnabled() const;
    void enableLoadAvatar(bool value = true);
    void setUploadFolder(const QString& folderName);
    void setUploadFolder(const QString& folderName, const QString& folderId);
    QString uploadFolder() const;
    QString uploadFolderId() const;
    void setSecured(bool https);
    bool secured() const;
    void setRetryCount(int count);
    int retryCount() const;

    void login();
    void directLogin(const QString &name,const QString &password);
    void ssoLogin(const QString &name);
    bool checkAuth();
    void logout();
    QString userName() const;
    QString userId() const;
    bool authentificated() const;
    QString uploadDirectoryLink() const;
    double spaceAmount() const;
    double spaceUsed() const;

    //void createBookmark(const QString& name, const QString& link);
    void addFileToUploadQueue(const QString &fileName);
    void createFolder(QString folderName, const QString& parentID = "0", const QString additionalPart = "");

    int warningWithLog(QWidget* parent, const QString& title, const QString& text, bool logButton = true);

    int uploadingQueueLength() const;
    QString uploadingQueueItem(int index) const;
    bool removeItemFromQueue(int index);
    QString currentUpload() const;
    void stopCurrentUpload();
    bool isUploading() const;

    QIcon& getFileTypeIcon(int fileType);

    int maxUploadFileSizeLimit() const;

    // read box.net folder/files tree

    void readTree(int folderId, bool folders, bool onelevel);
    QString treeXmlPath() const;
    QString linkTo(const QString& folderId) const;
    QString linkTo(const QString& folderId, const QString& fileId) const;

    void loadAuth();
    void saveAuth();
    void loadSettings();
    void saveSettings();
    void clearSettings();

public slots:
    void getAccountInfo(bool silent = false); // for actual amount and used sizes

protected:
    void loadAvatar(const QUrl& url);
    void requestTicket();
    void useTicketToLogin(const QString& ticket);
    void getUserInfo();
    void shareFile(const QString& fileId, const QString& fileName);

    static QString apiUrl(bool https = true, RequestType type = RequestRest, const QString& path = "");
    QString apiKeyParam() const;
    QString command(QStringList& params, RequestType type = RequestRest) const;
    QString command(QString& param, RequestType type = RequestRest) const;
    QString command(const QString& param, RequestType type = RequestRest) const;

    void initLog();
    void doneLog();    

private:
    void clearPassword();
    void closeFile();
    bool convertRegistryAuthSettings();
    bool loadFromOldSettings();
    void onFinishAuth();
    void onGetAccountInfoCompleted();
    void onGetAccountInfoFailed();
    void waitingForAuthToken();
    void openLoginForm(const QString& url);


    RESPONSE_STATUS responseStatus(QNetworkReply* reply, QDomElement* root = NULL);
    RESPONSE_STATUS responseStatusFromString(const QString& status) const;
    void initializeStatusMessagesHash();

    void parseTree(const QDomElement& root);
    void parseTreeFromDir(const QString& path);
    virtual void parseTreeFromFile(const QString& path);
    void parseFolder(const QDomElement& folder, QString parentId);
    void parseFile(const QDomElement& file, QString parentId);

signals:
    void networkError(QNetworkReply::NetworkError error, const QString& errorString);
    void authError(BxNet::RESPONSE_STATUS err);
    void authCompleted();
    void disconnected();
    void changeUserIcon(QPixmap pixmap);
    void addUploadToLog(const QString& fileName, const QString& link);
    void accountInfoCompleted();
    void createFolderError(BxNet::RESPONSE_STATUS);
    void folderCreated(const QString folderName, const QString folderId);
    void uploadProgressSignal(int progress, qint64 uploaded, qint64 total);
    void startUploadSignal();
    void uploadComplete(const QString& link);
    void queueEmpty();
    void uploadFailed(const QString& fileName, BxNet::RESPONSE_STATUS status);
    void shareSucceded(const QString& fileName, const QString& publicUrl);
    void shareFailed(BxNet::RESPONSE_STATUS);
    void notLoggedIn();
    void errorOpenBrowser(QString url);
    void uploadQueueChanged();
    void fileSizeLimit(const QString& fileName);
    void beginSSO();

    void treeFolder(QString id, QString parentId, QString name, QString link);
    void treeFile(QString id, QString parentId, QString name, QString link);
    void treeFinished();

private slots:
    void readTreeReadyRead();
    void readTreeFinished();
    void avatarReplyFinished();
    void getTicketFinished();
    void directLoginFinished();
    void getUserInfoFinished();
    void getAccountInfoFinished();
    void getAccountInfoFinishedSilent();
    void getAuthTokenFinished();
    void shareFileFinished();
    void uploadFinished();   
    void createFolderFinished();
    //void createBookmarkFinished();

    void requestAuthToken();
    void onTicketId(const QString& ticketId);

    void onError(QNetworkReply::NetworkError);
    void onSilentError(QNetworkReply::NetworkError);
    void onReadTreeError(QNetworkReply::NetworkError);
    void onAuthError(QNetworkReply::NetworkError error);
    void onUploadError(QNetworkReply::NetworkError error);
    void onShareError(QNetworkReply::NetworkError error);
    void onCreateFolderError(QNetworkReply::NetworkError error);
    void onSslError(const QList<QSslError>& errors);
    void onUploadProgress(qint64 uploaded, qint64 total);
    void startUpload();

private:
    QNetworkAccessManager*  m_networkManager;
    QNetworkReply*          m_uploadsReply;

    QString     m_apiKey;
    bool        m_https;

    bool        m_loadAvatar;
    bool        m_authentificated;
    bool        m_checkingAuthToken;
    bool        m_waitingForAuthToken;
    QString     m_authToken;
    QString     m_ticket;
    QString     m_userId;
    QString     m_avatarUrl;
    //QString     m_password; // temporary storage.

    QString     m_login;
    QString     m_email;
    //QString     m_accessId;

    QString     m_uploadFolder;
    QString     m_uploadFolderId;
    double      m_spaceAmount;
    double      m_spaceUsed;
    double      m_maxUploadSize;

    int         m_uploadProgress;
    QStringList m_uploadingQueue;
    QUpFile*    m_upf;
    QString     m_currentUploadingName;

    QHash<QString, RESPONSE_STATUS> m_statusMessages;
    // file type icons:
    QVector<QIcon*>     m_fileTypeIcons;

    bool            m_disconnected;
    int             m_retryCount;
    quint64         m_total;
    quint64         m_uploaded;

    QString         m_xmlPath;

    QTimer*         m_timer;

    QString         m_createFolderHelper;

    WebLoginForm*   m_webLoginForm;
};

void fixXMLfile(const QString& path);

#endif // BXNET_H
