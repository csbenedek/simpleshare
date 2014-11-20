#ifndef PreferencesWidget_H
#define PreferencesWidget_H

#include <QTabWidget>
#include <QCache>
#include <QPointer>
#include <QMutex>
#include <QFileDialog>
#include <QTimer>
#include "settings.h"
#include "bxnet.h"
#include "urlshortener.h"
#ifdef Q_OS_WIN
#include "videograber_win.h"
#else
#include "videograber_x11.h"
#endif
#include "config.h"
#include "droptargetsystray.h"

#define UPLOAD_FINISH_ICON  14
#define UPLOAD_START_ICON   1
#define ANIMATION_TIMER_TICK    200

#define GOOGLE_SHORTENER_API_KEY    "AIzaSyDa2a7hMeFm85zmEIQBUAescjQSUsNiV_w"

class BxNet;
class LoginDialog;

class QxtGlobalShortcut;
class QKeySequenceWidget;
class RecordingFrame;
class BasicUpdater;
class ImageEditor;
class VideoEditor;
class FolderDialog;

namespace Ui {
    class PreferencesWidget;
}

class PreferencesWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit PreferencesWidget(QWidget *parent = 0);
    ~PreferencesWidget();

    void notify(const QString& title, const QString& message, SystemTrayIcon_::MessageIcon type);
    QMenu* contextMenu();
    BxNet* boxnet();

protected:
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dropEvent(QDropEvent* event);
    virtual void timerEvent(QTimerEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void showEvent(QShowEvent *event);

private:
    void initTrayIcon();
    bool hideApplication(bool forceHideAllWindows = true);
    void setPixmap(const QPixmap& pixmap);
    QAction* insertRecentAction(const QString& name, const QString& link);
    void readRecentActionsFromSettings();
    void updateAutoUploadsFromSettings();
    bool openUrl(const QUrl& link);
    QString uploadDirectoryLink() const;
    void startUploadingNonGrabedPath(const QString& path);
    void startUploadingNonGrabedImageFile(const QString& fileName);
    void startUploadingNonGrabedDirectory(const QString& path);
    void clearTextBrowser();
    void loadSettings();
    void saveSettings();
    void updateAutoStartup();
    void createStartupItem();
    void removeStartupItem();
    void startCaptureVideo(QRect rect);
    void initShortcuts();
    void resetShortcutsToDefaults();
    void shortcutInUse();
    void notifyUrlUploaded(const QString& link, const QString& fileName = "", bool clipboard = true);
    void updateUserStorageInfo();
    void processGrabbedImage(bool annotate = false);

    QString decorateFileName(const QString& prefix, const QString& ext = "png", bool tempLocation = false) const;
    QString makeFullFileName(const QString& fileName) const;

    QPoint tooTipPos() const;
    void shareOnFacebook(const QString& link);
    void shareOnTweeter(const QString& link);

    bool isShortcutsEditing();
    void showLoginDialog();
    void updateTextWidget();

public slots:
    void previewNotClicked(const QPixmap& pixmap, const QString& name);
    void runEditor(const QPixmap& pixmap, const QString& name);
    void uploadFiles(const QStringList& files);
    void trayClick(QSystemTrayIcon::ActivationReason reason);
    void handleMessage(const QString& message);

    void onEditorSaveToFile(QPixmap& pixmap, const QString& fileName);
    void onEditorCopyToClipboard(QPixmap& pixmap);
    void onEditorUploadToNet(QPixmap& pixmap, bool facebook, bool twitter);

    void updateShortcutsControls();
    void onCustomContextMenuRequested(const QPoint& pos);

    void onStartScreencast(QString fileName);
    void onStopScreencast(QString fileName);
    void onMaximumSizeReached();
    void logout();
    void login();
    void onLoginCompleted();
    void stopUpload();

    void showOptions(int index = -1);
    void onUploadsFolderPushButtonClicked();
    void onUploadsFolderChanged(const QString folderName);
    void onAnchorClicked(QUrl url);

private slots:
    void currentStackedChanged(int index);
    void currentTabChanged(int index);
    void onQuit();
    void grabFullscreen();
    void grabRegion(bool annotate = false);
    void grabActiveWindow();
    void grabFullscreenShortcut();
    void grabRegionShortcut();
    void grabAnnotateShortcut();
    void grabActiveWindowShortcut();
    void onUploadFilesFromShellShortcut();
    void beginCaptureRegionVideoShortcut();
    //void onRepeatGrabShortcut();
    void uploadImage(const QString& imageName = "", bool facebook = false, bool tweeter = false);
    void toggleAutoUploads();
    void recentMenuClicked();   

    void beginCaptureFullScreenVideo();
    void beginCaptureRegionVideo();
    void endCaptureVideo();
    void muteVideo(bool muted);
    void onUploadFilesFromShell();

    void onUploadProgress(int progress, qint64 uploaded, qint64 total);
    void startUploadAnimation();
    void stopUploadAnimation();
    void updateUploadAnimation();
    void setNormalIcon();

    void keyNotSupported();
    void startShortcutRecording();
    void endShortcutRecording();
    void acceptShortcutFullScreen(const QKeySequence& shortcut);
    void acceptShortcutRegion(const QKeySequence& shortcut);
    void acceptShortcutScreencast(const QKeySequence& shortcut);
    void acceptShortcutUpload(const QKeySequence& shortcut);
    void acceptShortcutWindow(const QKeySequence& shortcut);
    //void acceptShortcutRepeat(const QKeySequence& shortcut);
    void acceptShortcutAnnotate(const QKeySequence& shortcut);

    void onStartUpload();
    void onUploadCompleted(QString fileName);
    void onUploadFailed(QString fileName, BxNet::RESPONSE_STATUS status);
    void onNotLoggedIn();
    void onChangeUserIcon(QPixmap);
    void onLogout();
    void onAuthFailed(BxNet::RESPONSE_STATUS err);
    void onCreateFolderFailed(BxNet::RESPONSE_STATUS);
    void onNetworkError(QNetworkReply::NetworkError error, const QString& errorString = "");
    void onShareSuccessfull(const QString& fileName, const QString& link);
    void onShareFailed(BxNet::RESPONSE_STATUS status);
#ifdef UI_SHORTEN_URL
    void onCantShorten(const QString& message);
    void onShortenLink(const QString& link, const QString& fileName);
#endif
    void onErrorOpenUrl(QString url); // TODO: make this const QString&
    void onUploadFile();
    void checkUpdates();
    void onUpdatesAvailable(const QString& version, const QString& link);
    void onAddUploadToLog(const QString &fileName, const QString &link);
    void onUpdateAccountInfo();
    void onFileSizeLimit(const QString& fileName);
    void onQueueEmpty();
    void onDisconnected();
    void onConnected();

    void onCompressed(const QString& zipName);

    void messageClicked();

    void on_okpushButton_clicked();
    void on_cancelpushButton_clicked();

    void on_actionVideo_triggered();
    void on_defaultsShortcutsPushButton_clicked();


private:
    Ui::PreferencesWidget *ui;

    QPointer<LoginDialog>  m_loginDialog;

    Settings            m_settings;
    BxNet*              m_bxnet;

    // system tray menu
    QIcon               m_normalAppIcon;
    QIcon               m_noUploadAppIcon;
    QIcon               m_disconnectedAppIcon;

    //QAction*          m_recentItemsSeparator;
    QMenu*              m_contextMenu;
    bool                m_customSystemTrayIcon;
    SystemTrayIcon_*    m_systemTrayIcon;
    QVector<QIcon>      m_busyIcons;
    int                 m_busyIconIndex;
    int                 m_uploadPercent;
    int                 m_finishIconSustantElapse;
    QCache<QString, QAction>   m_recentActions;
    QTimer              m_animationTimer;
    int                 m_updateTimerId;
    int                 m_checkNetworkTimerId;

    DropTargetSysTray*  m_dropTargetSysTray;
    // system tray menu

    // video
#ifdef Q_OS_WIN
    VideoGraber                 m_videoGraber;
#else
    X11VideoGraber              m_videoGraber;
#endif
    QPointer<RecordingFrame>    m_recordingFrame;
    // video

    bool                m_showUploadProgress;

    // shortcuts
    bool                m_disableShortcutsForEditing;
    QxtGlobalShortcut*  m_fullScreenShortcut;
    QxtGlobalShortcut*  m_regionShortcut;
    QxtGlobalShortcut*  m_activeWinShortcut;
    QxtGlobalShortcut*  m_screencastShortcut;
    QxtGlobalShortcut*  m_uploadShortcut;
    //QxtGlobalShortcut*  m_repeatShortcut;
    QxtGlobalShortcut*  m_annotateShortcut;

    QHash<QString,QString> m_otherFilesToUpload;
    QHash<QString,QString> m_readonlyFilesToUpload;
    QHash<QString,QString> m_temporaryUploadFile;
    QHash<QString,QStringList> m_zippedUploadFiles;

    QPixmap             m_grabedImage;
    QMutex              m_grabMutex;

    BasicUpdater*       m_updateManager;
    QFileDialog*        m_fileDialog;
    QFileDialog*        m_saveDialog;
    ImageEditor*        m_editor;
    VideoEditor*        m_videoEditor;

    // uploads tab
    //UploadsItemModel* m_uploadsModel;
    //QElapsedTimer     m_startUploadTimer;

    QStringList         m_facebookList;
    QStringList         m_twitterList;
    bool                m_wantToExit;
    bool                m_forceQuit;

    QPointer<FolderDialog> m_folderDialog;
};

extern const QString fullAppName();

#endif // PREFERENCESWIDGET_H
