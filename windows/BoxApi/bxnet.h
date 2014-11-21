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
#include <QPointer>

class QUpFile;
class UserSession;

class Settings;

class WebLoginForm;
class SyncHTTP;


#define DEFAULT_UPLOAD_FOLDER       "SimpleShare Uploads"
#define AUTH_FILE                   "box.net"
#define AUTH_DO_NOT_USE_MD5

// OAuth2

#define OAUTH2_CLIENT_ID      "eo0cww5szjn2wywnkcje927zdk0opxxp"
#define OAUTH2_CLIENT_SECRET  "YrtmNDYRVSPuchqX6yv52c4AkIhNydh9"

#define OAUTH2_AUTHORIZE_URL       "https://api.box.com/oauth2/authorize?"
#define OAUTH2_AUTHORIZE_CODE_URL  OAUTH2_AUTH_AUTHORIZE_URL "response_type=code&client_id=%s&redirect_uri=%s"

#define OAUTH2_REDIRECT_URL        "https://app.box.com/oauth2/logged_in"

#define OAUTH2_AUTH_TOKEN_URL      "https://api.box.com/oauth2/token"


#define BOXNET_API_KEY        OAUTH2_CLIENT_ID


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
        auth_attempts_for_ip_limit_reached,
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
        successful_register,        // create_new_user statuses;
        email_invalid,              // email invalid
        email_already_registered,   //
        e_register,
        //applications own status errors
        error_open_file,            //
        network_error,              //
        replay_parssing_error,      // the server replay is not valid and the parsing faield
        upload_canceled,
        //
        not_a_sso_user,             // SSO results
        incorrect_signon,
        sign_on_canceled,
        sign_on_timeout,
        //
        no_memory,
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
    bool isConnected() const; // TODO: refactor this
    void readProxySettingsFromSystem();
    void enableProxying(const QString& host, const QString& port = "",
                        QNetworkProxy::ProxyType type = QNetworkProxy::HttpProxy,
                        const QString& username = "", const QString& password = "");

    QString proxySettings() const;
    void setProxySettings(const QString& proxy);

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

    void newUser(const QString& name, QString password);
    bool isEmailExists(const QString& email);
    bool checkAuth(bool silent = false);
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

    void readUserFromXML(QDomNode& node);

    // read box.net folder/files tree

    void readTree(const QString& folderId, bool folders, bool onelevel, QVariant tag);
    QString linkTo(const QString& folderId) const;
    QString linkTo(const QString& folderId, const QString& fileId) const;

    void loadAuth();
    void saveAuth();
    void loadSettings();
    void saveSettings();
    void clearSettings();

public slots:
    void checkConnection();
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
    void openLoginForm();
    void closeLoginForm();

    RESPONSE_STATUS responseStatus(QNetworkReply* reply, QDomElement* root = NULL);
    RESPONSE_STATUS responseStatusFromString(const QString& status) const;
    RESPONSE_STATUS responseStatusFromXml(const QString& response, QDomElement* root = NULL) const;

    void initializeStatusMessagesHash();
    QString statusStringToStatus(RESPONSE_STATUS status);

    void parseTree(const QDomElement& root, const QString& rootId = "0", QVariant tag = QVariant());
    virtual void parseTreeFromBuff(QByteArray& buff, const QString& rootId = "0", QVariant tag = QVariant());
    void parseFolder(const QDomElement& folder, QString parentId, QVariant tag);
    void parseFile(const QDomElement& file, QString parentId, QVariant tag);


signals:
    void networkError(QNetworkReply::NetworkError error, const QString& errorString);
    void authFailed(BxNet::RESPONSE_STATUS err);
    void authCompleted();
    void disconnected();
    void connected();
    void changeUserIcon(QPixmap pixmap);
    void addUploadToLog(const QString& fileName, const QString& link);
    void accountInfoCompleted();
    void createFolderFailed(BxNet::RESPONSE_STATUS);
    void folderCreated(const QString folderName, const QString folderId);
    void uploadsFolderChanged(const QString folderName);
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
    void newUserSucceded();
    void newUserFailed(BxNet::RESPONSE_STATUS);

    void treeFolder(QString id, QString parentId, QString name, QString link, QVariant tag);
    void treeFile(QString id, QString parentId, QString name, QString link, QVariant tag);
    void treeFinished(QVariant tag);

private slots:
    void readTreeReadyRead();
    void readTreeFinished();
    void avatarReplyFinished();
    void getUserInfoFinished();
    void getAccountInfoFinished();
    void getAccountInfoFinishedSilent();
    void shareFileFinished();
    void uploadFinished();   
    void createFolderFinished();
    void newUserFinished();
    //void createBookmarkFinished();

    void onOAuth2Code(BxNet::RESPONSE_STATUS status, const QString& oauth2_code);
    void onAuthError(BxNet::RESPONSE_STATUS status);
    void onOAuth2TokenFinished();

    void onError(QNetworkReply::NetworkError);
    void onSilentError(QNetworkReply::NetworkError);
    void onReadTreeError(QNetworkReply::NetworkError);
    void onAuthError(QNetworkReply::NetworkError error);
    void onUploadError(QNetworkReply::NetworkError error);
    void onShareError(QNetworkReply::NetworkError error);
    void onCreateFolderError(QNetworkReply::NetworkError error);
    void onNewUserError(QNetworkReply::NetworkError error);
    void onSslError(const QList<QSslError>& errors);
    void onUploadProgress(qint64 uploaded, qint64 total);
    void startUpload();

private:
    QPointer<QNetworkAccessManager> m_networkManager;
    QPointer<QNetworkReply>         m_uploadsReply;

    bool        m_https;

    bool        m_loadAvatar;
    bool        m_authentificated;
    bool        m_connected;
    bool        m_checkingAuthToken;
    bool        m_waitingForAuthToken;
    QString     m_authToken;
    QString     m_ticket;
    QString     m_userId;
    QString     m_avatarUrl;
    //QString     m_password; // temporary storage.

    //QString     m_oauth2_code;
    QString     m_oauth2_token;
    QString     m_oauth2_refresh_token;
    QString     m_oauth2_token_type;

    QString     m_login;
    QString     m_email;
    //QString     m_accessId;

    QString     m_uploadFolder;
    QString     m_uploadFolderId;
    double      m_spaceAmount;
    double      m_spaceUsed;
    double      m_maxUploadSize;

    int                 m_uploadProgress;
    QStringList         m_uploadingQueue;
    QPointer<QUpFile>   m_upf;
    QString             m_currentUploadingName;

    QHash<QString, RESPONSE_STATUS> m_statusMessages;
    // file type icons:
    QVector<QIcon*>     m_fileTypeIcons;

    bool            m_disconnected;
    int             m_retryCount;
    quint64         m_total;
    quint64         m_uploaded;

    QPointer<QTimer>            m_timer;
    QString                     m_createFolderHelper;
    QPointer<WebLoginForm>      m_webLoginForm;

    // check connection:
    QPointer<SyncHTTP>          m_syncHttp;
    bool                        m_isConnected;
    bool                        m_isSSLErrors;
};

void fixXMLfile(const QString& path);

#endif // BXNET_H
