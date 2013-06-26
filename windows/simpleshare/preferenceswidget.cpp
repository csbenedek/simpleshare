#include "PreferencesWidget.h"
#include "graber.h"
#include "bxnet.h"
#include "ui_PreferencesWidget.h"
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDateTime>
#include <QTimer>
#include <QClipboard>
#include <QFileDialog>
#include <QPainter>
#include <QDir>
#include <QToolTip>
#include "qxtglobalshortcut.h"
#include "recordingframe.h"
#include "keysequencewidget.h"
#include "basicupdater.h"
#include "ziphelper.h"
#include "urlshortener.h"
#include "imageeditor.h"
#include "utils.h"
#ifdef Q_WS_MAC
#include <CoreServices/CoreServices.h>
#endif
#ifdef Q_WS_WIN
#include "shellutils.h"
#include "oleutils.h"
#include <windows.h>
#endif
#include <QDebug>

const QString fullAppName()
{
    return qApp->applicationName() + " " + qApp->applicationVersion();
}

// PreferencesWidget

PreferencesWidget::PreferencesWidget(QWidget *parent)
    : QTabWidget(parent)
    , ui(new Ui::PreferencesWidget)
    , m_bxnet(NULL)
    , m_contextMenu(NULL)
    , m_uploadPercent(0)
    , m_dropTargetSysTray(NULL)
    , m_recordingFrame(NULL)
    , m_disableShortcutsForEditing(false)
    , m_fullScreenShortcut(NULL)
    , m_regionShortcut(NULL)
    , m_activeWinShortcut(NULL)
    , m_screencastShortcut(NULL)
    , m_uploadShortcut(NULL)
    , m_annotateShortcut(NULL)
    , m_grabMutex(QMutex::NonRecursive)
#ifdef UI_SHORTEN_URL
    , m_urlShortener(NULL)
#endif
    , m_updateManager(NULL)
    , m_fileDialog(NULL)
    , m_saveDialog(NULL)
    , m_editor(NULL)
    , m_videoEditor(NULL)
    , m_wantToExit(false)
    , m_forceQuit(false)
    , m_blockFolderModifications(false)
{
    ui->setupUi(this);	

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

#ifndef UI_SHORTEN_URL
    ui->urlShortenerCheckBox->setVisible(false);
    ui->urlShortenerLabel->setVisible(false);
#endif

#ifdef UI_EXTERNAL_AUTH
    ui->userEmailLabel->setVisible(false);
    ui->userEmailLineEdit->setVisible(false);
    ui->passwordLabel->setVisible(false);
    ui->passwordLineEdit->setVisible(false);

    ui->verticalSpacer_7->changeSize(0,0, QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->verticalSpacer_10->changeSize(0,0, QSizePolicy::Fixed, QSizePolicy::Fixed);
#else
    ui->descriptionLabel->setVisible(false);
#endif

    ui->loggedWidget->setVisible(false);
    ui->loginWidget_2->setVisible(true);
    ui->registerWidget->setVisible(true);
    ui->viewFolderWidget->setVisible(false);
    ui->instructionWidget->setVisible(false);

    m_bxnet  = new BxNet();
    m_bxnet->setApiKey(BOXNET_API_KEY);
	//Prabhu call oauth2 authorize
	//m_bxnet->takeUsrToAuthorizePage();	

#ifdef UI_SHORTEN_URL
    m_urlShortener = new UrlShortener(this);
    m_urlShortener->setApiKey(GOOGLE_SHORTENER_API_KEY);
    connect(m_urlShortener, SIGNAL(shortened(QString, QString)), this, SLOT(onShortenLink(QString, QString)));
    connect(m_urlShortener, SIGNAL(errorMessage(QString)), this, SLOT(onCantShorten(QString)));
#endif

    initShortcuts();

    loadSettings();

    initTrayIcon();

    m_bxnet->setSecured(m_settings.https);
    //m_bxnet->setUploadFolder(DEFAULT_UPLOAD_FOLDER);
    //check valid authToken:

    if (!m_bxnet->userName().isEmpty())
    {
        m_settings.lastUserName = m_bxnet->userName();
    }
    ui->userEmailLineEdit->setText(m_settings.lastUserName);

    if (!m_settings.proxy.isEmpty())
    {
        m_bxnet->setProxySettings(m_settings.proxy);
    }
    else
    {
        m_bxnet->readProxySettingsFromSystem();
    }

    m_bxnet->setRetryCount(m_settings.retryCount);

    connect(m_bxnet, SIGNAL(authError(BxNet::RESPONSE_STATUS)), this, SLOT(onAuthError(BxNet::RESPONSE_STATUS)));
    connect(m_bxnet, SIGNAL(authCompleted()), this, SLOT(onLogin()));
    connect(m_bxnet, SIGNAL(notLoggedIn()), this, SLOT(onNotLoggedIn()));

    connect(m_bxnet, SIGNAL(uploadProgressSignal(int, qint64, qint64)), this, SLOT(onUploadProgress(int, qint64, qint64)));
    connect(m_bxnet, SIGNAL(startUploadSignal()), this, SLOT(onStartUpload()));
    connect(m_bxnet, SIGNAL(uploadComplete(QString)), this, SLOT(onUploadCompleted(QString)));
    connect(m_bxnet, SIGNAL(uploadFailed(QString, BxNet::RESPONSE_STATUS)), this, SLOT(onUploadFailed(QString, BxNet::RESPONSE_STATUS)));
    connect(m_bxnet, SIGNAL(shareSucceded(QString, QString)), this, SLOT(onShareSuccessfull(QString, QString)));
    connect(m_bxnet, SIGNAL(shareFailed(BxNet::RESPONSE_STATUS)), this, SLOT(onShareFailed(BxNet::RESPONSE_STATUS)));
    connect(m_bxnet, SIGNAL(createFolderError(BxNet::RESPONSE_STATUS)), this, SLOT(onCreateFolderError(BxNet::RESPONSE_STATUS)));
    connect(m_bxnet, SIGNAL(networkError(QNetworkReply::NetworkError, QString)), this, SLOT(onNetworkError(QNetworkReply::NetworkError, QString)));
    connect(m_bxnet, SIGNAL(changeUserIcon(QPixmap)), this, SLOT(onChangeUserIcon(QPixmap)));
    connect(m_bxnet, SIGNAL(addUploadToLog(QString, QString)), this, SLOT(onAddUploadToLog(QString, QString)));
    connect(m_bxnet, SIGNAL(accountInfoCompleted()), this, SLOT(onUpdateAccountInfo()));
    connect(m_bxnet, SIGNAL(errorOpenBrowser(QString)), this, SLOT(onErrorOpenUrl(QString)));
    connect(m_bxnet, SIGNAL(fileSizeLimit(QString)), this, SLOT(onFileSizeLimit(QString)));
    connect(m_bxnet, SIGNAL(queueEmpty()), this, SLOT(onQueueEmpty()));
    connect(m_bxnet, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    connect(m_bxnet, SIGNAL(folderCreated(QString, QString)), this, SLOT(updateFoldersList(QString, QString)));
    connect(m_bxnet, SIGNAL(treeFolder(QString,QString,QString,QString)), this, SLOT(treeFolder(QString,QString,QString,QString)));
    connect(m_bxnet, SIGNAL(treeFile(QString,QString,QString,QString)), this, SLOT(treeFile(QString,QString,QString,QString)));
    connect(m_bxnet, SIGNAL(treeFinished()), this, SLOT(treeFinished()));
    //connect(m_bxnet, SIGNAL(uploadFolderChanged(QString)), this, SLOT(uploadFolderChanged(QString)));
    connect(m_bxnet, SIGNAL(beginSSO()), this, SLOT(hide()));

    connect(ui->textBrowser, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(signUpClicked(const QUrl&)));

    onLogout();
#ifndef UI_EXTERNAL_AUTH
    ui->passwordLineEdit->setText("");
    ui->userEmailLineEdit->setText(m_bxnet->userName().isEmpty() ? m_settings.lastUserName : m_bxnet->userName());
#endif

    if (m_bxnet->checkAuth())
    {
        ui->loginPushButton->setEnabled(false);
        ui->userEmailLineEdit->setEnabled(false);
        ui->passwordLineEdit->setEnabled(false);
    }


    m_showUploadProgress=false;
#ifdef Q_WS_MAC
    m_showUploadProgress=true;
#endif

    if (m_settings.startAtLogin)
    {
        createStartupItem();
    }

    ui->loggedWidget->setVisible(false);
    ui->viewFolderWidget->setVisible(false);

    readRecentActionsFromSettings();

    updateAutoUploadsFromSettings();

    // update version inforamtion in label
    ui->versionLabel->setText("v" + qApp->applicationVersion());


    // set the Form size
    //setAdvansedOptions(false);
    setFixedHeight(280);
    setFixedWidth(550);
    centrateWidget(this);

    Q_ASSERT(m_systemTrayIcon);
    if (m_systemTrayIcon)
    {
        m_systemTrayIcon->show();
#ifdef Q_OS_WIN
        // create drop target in system tray:
        m_dropTargetSysTray  = new DropTargetSysTray(m_systemTrayIcon, this);
#endif
        connect(m_systemTrayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    }

    connect(&m_animationTimer, SIGNAL(timeout()), this, SLOT(updateUploadAnimation()));

    // user can change contents using browser. trying too sync our data with web data each 15 min
    m_updateTimerId = startTimer(15*60*1000); // update storage info each 5 min

    checkUpdates();

    //connect the signal to recive the message  from other instances
    QObject::connect(qApp, SIGNAL(messageReceived(const QString&)), this, SLOT(handleMessage(const QString&)));

    setAcceptDrops(true);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));

    connect(&m_videoGraber, SIGNAL(startScreencast(QString)), this, SLOT(onStartScreencast(QString)));
    connect(&m_videoGraber, SIGNAL(stopScreencast(QString)), this, SLOT(onStopScreencast(QString)));
    connect(&m_videoGraber, SIGNAL(maximumSizeReached()), this, SLOT(onMaximumSizeReached()));

    //ui->foldersComboBox->setEditText(DEFAULT_UPLOAD_FOLDER);
    connect(ui->foldersComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(folderTextChanged(QString)));
    connect(ui->foldersComboBox->lineEdit(), SIGNAL(editingFinished()), this, SLOT(folderTextFinished()));

}

PreferencesWidget::~PreferencesWidget()
{      
    killTimer(m_updateTimerId);
    m_animationTimer.stop();

    updateAutoStartup();

    endCaptureVideo();

    delete m_fileDialog;
    delete m_saveDialog;
    delete m_editor;
    delete m_bxnet;
    delete m_updateManager;
    delete m_systemTrayIcon;
    delete m_contextMenu;

    delete ui;  

}

void PreferencesWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void PreferencesWidget::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();

    QStringList files;

    foreach(const QUrl& url, urls)
    {
        QString fileName = url.toLocalFile();
        if (!fileName.isNull() && !fileName.isEmpty())
        {
            files << fileName;
        }
    }

    if (!files.isEmpty())
    {
        uploadFiles(files);
    }
}

QMenu* PreferencesWidget::contextMenu()
{
    return m_contextMenu;
}

BxNet* PreferencesWidget::boxnet()
{
    return m_bxnet;
}


void PreferencesWidget::resetShortcutsToDefaults()
{
    Q_ASSERT(m_activeWinShortcut);
    if (!m_activeWinShortcut->shortcut().isEmpty())
    {
        m_activeWinShortcut->unsetShortcut();
    }
    Q_ASSERT(m_fullScreenShortcut);
    if (!m_fullScreenShortcut->shortcut().isEmpty())
    {
        m_fullScreenShortcut->unsetShortcut();
    }
    Q_ASSERT(m_regionShortcut);
    if (!m_regionShortcut->shortcut().isEmpty())
    {
        m_regionShortcut->unsetShortcut();
    }
    Q_ASSERT(m_screencastShortcut);
    if (!m_screencastShortcut->shortcut().isEmpty())
    {
        m_screencastShortcut->unsetShortcut();
    }
    Q_ASSERT(m_uploadShortcut);
    if (!m_uploadShortcut->shortcut().isEmpty())
    {
        m_uploadShortcut->unsetShortcut();
    }
    //Q_ASSERT(m_repeatShortcut);
    //if (!m_repeatShortcut->shortcut().isEmpty())
    //{
    //    m_repeatShortcut->unsetShortcut();
    //}
    Q_ASSERT(m_annotateShortcut);
    if (!m_annotateShortcut->shortcut().isEmpty())
    {
        m_annotateShortcut->unsetShortcut();
    }

#ifdef Q_WS_MAC
    m_activeWinShortcut->setShortcut(QKeySequence("Ctrl+Alt+6"));
#else
    m_activeWinShortcut->setShortcut(QKeySequence("Alt+Shift+6"));
#endif

#ifdef Q_WS_MAC
    m_fullScreenShortcut->setShortcut(QKeySequence("Ctrl+Alt+3"));
#else
    m_fullScreenShortcut->setShortcut(QKeySequence("Alt+Shift+3"));
#endif

#ifdef Q_WS_MAC
    m_regionShortcut->setShortcut(QKeySequence("Ctrl+Alt+4"));
#else
    m_regionShortcut->setShortcut(QKeySequence("Alt+Shift+4"));
#endif

#ifdef Q_WS_MAC
    m_screencastShortcut->setShortcut(QKeySequence("Ctrl+Alt+S"));
#else
    m_screencastShortcut->setShortcut(QKeySequence("Alt+Shift+S"));
#endif

#ifdef Q_WS_MAC
    m_uploadShortcut->setShortcut(QKeySequence("Ctrl+Alt+U"));
#else
    m_uploadShortcut->setShortcut(QKeySequence("Alt+Shift+U"));
#endif
/*
#ifdef Q_WS_MAC
    m_repeatShortcut->setShortcut(QKeySequence("Ctrl+Alt+R"));
#else
    m_repeatShortcut->setShortcut(QKeySequence("Alt+Shift+R"));
#endif
*/
#ifdef Q_WS_MAC
    m_annotateShortcut->setShortcut(QKeySequence("Ctrl+Alt+2"));
#else
    m_annotateShortcut->setShortcut(QKeySequence("Alt+Shift+2"));
#endif

    updateShortcutsControls();
}

void PreferencesWidget::grabFullscreenShortcut()
{
    if (isShortcutsEditing())
    {
        shortcutInUse();
        return;
    }

    grabFullscreen();
}

void PreferencesWidget::grabRegionShortcut()
{
    if (isShortcutsEditing())
    {
        shortcutInUse();
        return;
    }

    grabRegion();
}

void PreferencesWidget::grabActiveWindowShortcut()
{
    if (isShortcutsEditing())
    {
        shortcutInUse();
        return;
    }

    grabActiveWindow();
}

void PreferencesWidget::grabAnnotateShortcut()
{
    if (isShortcutsEditing())
    {
        shortcutInUse();
        return;
    }

    grabRegion(true);
}

void PreferencesWidget::onUploadFilesFromShellShortcut()
{
    if (isShortcutsEditing())
    {
        shortcutInUse();
        return;
    }

    onUploadFilesFromShell();
}

void PreferencesWidget::beginCaptureRegionVideoShortcut()
{
    if (isShortcutsEditing())
    {
        shortcutInUse();
        return;
    }

    beginCaptureRegionVideo();
}

//void PreferencesWidget::onRepeatGrabShortcut()
//{
//    if (isShortcutsEditing())
//    {
//        shortcutInUse();
//        return;
//    }
//
//    //onRepeatGrab();
//}

void PreferencesWidget::initShortcuts()
{
    m_activeWinShortcut = new QxtGlobalShortcut(this);
    connect(m_activeWinShortcut, SIGNAL(activated()), this, SLOT(grabActiveWindowShortcut()));

    m_fullScreenShortcut = new QxtGlobalShortcut(this);
    connect(m_fullScreenShortcut, SIGNAL(activated()), this, SLOT(grabFullscreenShortcut()));

    m_regionShortcut = new QxtGlobalShortcut(this);
    connect(m_regionShortcut, SIGNAL(activated()), this, SLOT(grabRegionShortcut()));

    m_screencastShortcut = new QxtGlobalShortcut(this);
    connect(m_screencastShortcut, SIGNAL(activated()), this, SLOT(beginCaptureRegionVideoShortcut()));

    m_uploadShortcut = new QxtGlobalShortcut(this);
    connect(m_uploadShortcut, SIGNAL(activated()), this, SLOT(onUploadFilesFromShellShortcut()));

    //m_repeatShortcut = new QxtGlobalShortcut(this);
    //connect(m_repeatShortcut, SIGNAL(activated()), this, SLOT(onRepeatGrabShortcut()));

    m_annotateShortcut = new QxtGlobalShortcut(this);
    connect(m_annotateShortcut, SIGNAL(activated()), this, SLOT(grabAnnotateShortcut()));

    resetShortcutsToDefaults();

    connect(ui->fullScreenKeyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcutFullScreen(QKeySequence)));
    connect(ui->fullScreenKeyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    connect(ui->fullScreenKeyWidget, SIGNAL(keySequenceStart()), this, SLOT(startShortcutRecording()));
    connect(ui->fullScreenKeyWidget, SIGNAL(keySequenceEnd()), this, SLOT(endShortcutRecording()));

    connect(ui->regionKeyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcutRegion(QKeySequence)));
    connect(ui->regionKeyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    connect(ui->regionKeyWidget, SIGNAL(keySequenceStart()), this, SLOT(startShortcutRecording()));
    connect(ui->regionKeyWidget, SIGNAL(keySequenceEnd()), this, SLOT(endShortcutRecording()));

    connect(ui->screencastKeyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcutScreencast(QKeySequence)));
    connect(ui->screencastKeyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    connect(ui->screencastKeyWidget, SIGNAL(keySequenceStart()), this, SLOT(startShortcutRecording()));
    connect(ui->screencastKeyWidget, SIGNAL(keySequenceEnd()), this, SLOT(endShortcutRecording()));

    connect(ui->uploadKeyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcutUpload(QKeySequence)));
    connect(ui->uploadKeyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    connect(ui->uploadKeyWidget, SIGNAL(keySequenceStart()), this, SLOT(startShortcutRecording()));
    connect(ui->uploadKeyWidget, SIGNAL(keySequenceEnd()), this, SLOT(endShortcutRecording()));

    connect(ui->windowKeyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcutWindow(QKeySequence)));
    connect(ui->windowKeyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    connect(ui->windowKeyWidget, SIGNAL(keySequenceStart()), this, SLOT(startShortcutRecording()));
    connect(ui->windowKeyWidget, SIGNAL(keySequenceEnd()), this, SLOT(endShortcutRecording()));

    //connect(ui->repeatKeyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcutRepeat(QKeySequence)));
    //connect(ui->repeatKeyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    //connect(ui->repeatKeyWidget, SIGNAL(keySequenceStart()), this, SLOT(startShortcutRecording()));
    //connect(ui->repeatKeyWidget, SIGNAL(keySequenceEnd()), this, SLOT(endShortcutRecording()));

    connect(ui->annotateKeyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcutAnnotate(QKeySequence)));
    connect(ui->annotateKeyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    connect(ui->annotateKeyWidget, SIGNAL(keySequenceStart()), this, SLOT(startShortcutRecording()));
    connect(ui->annotateKeyWidget, SIGNAL(keySequenceEnd()), this, SLOT(endShortcutRecording()));

}

void PreferencesWidget::updateShortcutsControls()
{
    ui->fullScreenKeyWidget->setKeySequence(m_fullScreenShortcut->shortcut());
    ui->regionKeyWidget->setKeySequence(m_regionShortcut->shortcut());
    ui->screencastKeyWidget->setKeySequence(m_screencastShortcut->shortcut());
    ui->uploadKeyWidget->setKeySequence(m_uploadShortcut->shortcut());
    ui->windowKeyWidget->setKeySequence(m_activeWinShortcut->shortcut());
    //ui->repeatKeyWidget->setKeySequence(m_repeatShortcut->shortcut());
    ui->annotateKeyWidget->setKeySequence(m_annotateShortcut->shortcut());
}

QAction* PreferencesWidget::insertRecentAction(const QString& name, const QString& link)
{
    if (link.isEmpty())
    {
        return NULL;
    }
    QAction* action = new QAction(name, this);
    Q_ASSERT(action);
    if (action)
    {
        action->setProperty("link", link);
        //action->setVisible(true); // already visible by default
        int fileType = getFileType(name);
        action->setIcon(m_bxnet->getFileTypeIcon(fileType));
        connect(action, SIGNAL(triggered()), this, SLOT(recentMenuClicked()));
        m_recentActions.insert(link, action, 1);
        //Q_ASSERT(m_recentItemsSeparator);
        m_contextMenu->insertAction(m_contextMenu->actions().at(0), action);
        m_systemTrayIcon->setContextMenu(m_contextMenu);
    }
    return action;
}

void PreferencesWidget::recentMenuClicked()
{
    QAction* action= qobject_cast<QAction*>(sender());
    if (action)
    {
        const QString link = action->property("link").toString();

        hideApplication(false);

        QDesktopServices::openUrl(QUrl(link));
    }
}

void PreferencesWidget::readRecentActionsFromSettings()
{
    int count = qMin(m_settings.recentUploads.size(), m_recentActions.maxCost());
    for(int i = 0; i < count; ++i)
    {
        const Settings::RecentItem& pair = m_settings.recentUploads.at(count-i-1); // add items in back order
        insertRecentAction(pair.first, pair.second);        
    }   
}

void PreferencesWidget::initTrayIcon()
{
    m_normalAppIcon.addFile(":/res/simple_capture-16x16.png");
    m_noUploadAppIcon.addFile(":/res/nointernet_grey.png");

   //ceate actions for recent uploads
    m_recentActions.setMaxCost(3);

    // connect to slots
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(onQuit()));
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(showOptions()) );
    connect(ui->actionFullScreen, SIGNAL(triggered()), this, SLOT(grabFullscreen()));
    connect(ui->actionRegion, SIGNAL(triggered()), this, SLOT(grabRegion()));
    connect(ui->actionWindow, SIGNAL(triggered()), this, SLOT(grabActiveWindow()));
    connect(ui->actionUpload, SIGNAL(triggered()), this, SLOT(onUploadFile()));
    //connect(openGrabs,SIGNAL(triggered()),this,SLOT(onOpenGrabsDirectory()));
    connect(ui->actionUploads, SIGNAL(triggered()), this, SLOT(toggleAutoUploads()));

    // create tray menu
    m_contextMenu = new QMenu(NULL);

    //m_contextMenu->addAction(recent1);
    //m_contextMenu->addAction(recent2);
    //m_contextMenu->addAction(recent3);
    //m_contextMenu->addSeparator();
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(ui->actionRegion);
    m_contextMenu->addAction(ui->actionFullScreen);
    m_contextMenu->addAction(ui->actionUpload);
    m_contextMenu->addAction(ui->actionVideo);
    //this is not usefull because it will grab the start bar
    //m_contextMenu->addAction(activeWindowAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(ui->actionUploads);
    m_contextMenu->addAction(ui->actionPreferences);
    //m_contextMenu->addSeparator();
    m_contextMenu->addAction(ui->actionQuit);

    m_systemTrayIcon = new SystemTrayIcon_(NULL);
    m_systemTrayIcon->setContextMenu(m_contextMenu);
    setNormalIcon();
    //m_systemTrayIcon->show(); show later

    updateAutoUploadsFromSettings();

    connect(m_systemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayClick(QSystemTrayIcon::ActivationReason)) );

    m_busyIcons.reserve(15);
    m_busyIcons.append(m_normalAppIcon);
    // create busy icons
    QString resFile;
    for(int i=1; i<=13; i++)
    {
        resFile.sprintf(":/res/loading_%d.png", i);
        m_busyIcons.append(QIcon(resFile));
    }
    m_busyIcons.append(QIcon(":/res/loading_14_checkmark.png"));
}


void PreferencesWidget::showOptions()
{
    if (m_bxnet->authentificated())
    {
        setCurrentIndex(1);
    }
    else
    {
        setCurrentIndex(0);
    }

    showNormal();
    activateWindow();
    raise();
}

void PreferencesWidget::handleMessage(const QString &message)
{   
    //if you start a second instance that will try to send a message and that message will be empty
    if (message.isEmpty())
    {
        showOptions();
    }
    else
    {
        QStringList arguments = message.split("<");

        bool commandLogout  = arguments.indexOf("--logout") != -1;
        bool commandQuit    = commandLogout
                                || (arguments.indexOf("--quit") != -1);

        if (commandLogout)
        {
            logout();
        }

        if (commandQuit)
        {
            m_forceQuit = true;
            qApp->quit();
            return;
        }
        else
        {
            uploadFiles(arguments);
        }
    }
}

void PreferencesWidget::trayClick(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick)
    {
        if (isVisible() && !isMinimized())
        {
            hideApplication();
        }
        else
        {
            showOptions();
        }
        //setVisible(!isVisible());
    }

    if (reason == QSystemTrayIcon::Trigger)
    {
        //qDebug() << "trayClick(QSystemTrayIcon::Trigger)";
    }
}

void PreferencesWidget::onUploadFile()
{
    //open a dialog to chose a file
    if (m_fileDialog == NULL)
    {
        m_fileDialog = new QFileDialog(this, tr("Upload files with SimpleShare"));
        m_fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
        m_fileDialog->setFileMode(QFileDialog::ExistingFiles);
        m_fileDialog->setDirectory(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
    }
    if (m_fileDialog)
    {
        m_fileDialog->selectFile("");
        centrateWidget(m_fileDialog);
        if (m_fileDialog->exec())
        {
            uploadFiles(m_fileDialog->selectedFiles());
        }
    }
}


void PreferencesWidget::onEditorSaveToFile(QPixmap& pixmap, const QString& fileName)
{
    m_grabedImage = pixmap;

    if (m_saveDialog == NULL)
    {
        m_saveDialog = new QFileDialog(this);
        m_saveDialog->setAcceptMode(QFileDialog::AcceptSave);
        m_saveDialog->setFileMode(QFileDialog::AnyFile);
        m_saveDialog->setDirectory(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));

        QStringList filters;
        filters << "Png files *.png"
                << "Jpeg files *.jpg"
                << "Any files (*)";

        m_saveDialog->setNameFilters(filters);
        m_saveDialog->setDefaultSuffix("png");
    }

    if (m_saveDialog)
    {
        if (fileName.isEmpty())
        {
            if (!m_editor->imageName().isEmpty())
            {
                QFileInfo fileInfo(m_editor->imageName());
                m_saveDialog->setDirectory(fileInfo.dir());
                m_saveDialog->selectFile(fileInfo.fileName());
            }
            else
            {
                m_saveDialog->selectFile("");
            }

            centrateWidget(m_saveDialog);

            if (m_saveDialog->exec())
            {
                if (m_saveDialog->selectedFiles().count() == 1)
                {
                    QString fileName = m_saveDialog->selectedFiles().at(0);
                    pixmap.save(fileName, 0, m_settings.quality);

                    m_editor->setImageName(fileName);
                }
            }
        }
        else
        {
            pixmap.save(fileName, 0, m_settings.quality);
        }
    }

    if (m_editor)
    {
        m_editor->close();
    }
}

void PreferencesWidget::onEditorCopyToClipboard(QPixmap& pixmap)
{
    m_grabedImage = pixmap;
    if (m_editor)
    {
        m_editor->close();
    }
    QApplication::clipboard()->setPixmap(m_grabedImage);
    notify(tr("Screenshot"), tr("screenshot copied to your clipboard"), SystemTrayIcon_::UserIcon);
}

void PreferencesWidget::onEditorUploadToNet(QPixmap& pixmap, bool facebook, bool twitter)
{
    m_grabedImage = pixmap;
    if (m_editor)
    {
        m_editor->close();
    }

    if (!m_bxnet || !m_bxnet->authentificated() || !isConnectedToNetwork())
    {
        // offline mode. copy image to clipboard
        QApplication::clipboard()->setPixmap(m_grabedImage);
        notify(tr("Screenshot"), tr("screenshot copied to your clipboard"), SystemTrayIcon_::UserIcon);
    }

    uploadImage(m_editor->imageName(), facebook, twitter);
}

void PreferencesWidget::previewNotClicked(const QPixmap& pixmap, const QString& name)
{
    if (!pixmap.isNull())
    {
        m_grabedImage = pixmap;
        uploadImage(name);
    }
}

void PreferencesWidget::runEditor(const QPixmap& pixmap, const QString& name)
{
    if (!pixmap.isNull())
    {
        m_grabedImage = pixmap;
    }

    if (!hideApplication())
    {
        return;
    }

    if (m_editor == NULL)
    {
        m_editor = new ImageEditor();

        connect(m_editor, SIGNAL(saveToFile(QPixmap&, const QString&)), this, SLOT(onEditorSaveToFile(QPixmap&, const QString&)));
        connect(m_editor, SIGNAL(copyToClipboard(QPixmap&)), this, SLOT(onEditorCopyToClipboard(QPixmap&)));
        connect(m_editor, SIGNAL(uploadToNet(QPixmap&, bool, bool)), this, SLOT(onEditorUploadToNet(QPixmap&, bool, bool)));
    }

    if (m_editor)
    {
        bool isHided = false;
        if(m_recordingFrame && m_recordingFrame->isVisible())
        {
            m_recordingFrame->hide();
            isHided = true;
        }
        m_editor->clear();
        m_editor->resize(100,100);
        m_editor->move(100,100);
        m_editor->setPixmap(m_grabedImage, name.isEmpty() ? decorateFileName("Screen shot", m_settings.suffix) : name);
        m_editor->show();
        m_editor->raise();
        m_editor->activateWindow();
        if (isHided)
        {
            m_recordingFrame->show();
        }
    }
}

void PreferencesWidget::processGrabbedImage(bool annotate)
{
    bool copied = false;
    if (!m_bxnet || !m_bxnet->authentificated() || !isConnectedToNetwork() || !m_settings.automaticUpload)
    {
        // offline mode. copy image to clipboard
        QApplication::clipboard()->setPixmap(m_grabedImage);
        copied = true;
    }

    if (m_settings.editor || annotate)
    {
        runEditor(m_grabedImage, decorateFileName("Screen shot", m_settings.suffix));
    }
    else if (m_settings.automaticUpload)
    {
        //if automatics uploads then upload the image
        uploadImage();
    }
    else if (copied)
    {
        notify(tr("Screenshot"), tr("screenshot copied to your clipboard"), SystemTrayIcon_::UserIcon);
    }
}

void PreferencesWidget::grabFullscreen()
{
    m_wantToExit = false;

    if (!hideApplication())
    {
        return;
    }

    QPixmap pix;
    pix = Graber::grabDesktop(m_settings.captureCursor);

    if (!pix.isNull())
    {
        setPixmap(pix);

        processGrabbedImage();
    }
}

void PreferencesWidget::grabRegion(bool annotate)
{
    m_wantToExit = false;

    if (!m_grabMutex.tryLock())
    {
        qDebug() << "prevent multiple region grabbing";
        return;
    }
    try
    {
        if (hideApplication())
        {

            QPixmap pix = Graber::grabRegion(m_settings.invertedSelection, m_settings.captureCursor);

            if (!pix.isNull())
            {
                setPixmap(pix);

                processGrabbedImage(annotate);
            }
        }
    }
    catch(...)
    {
        // do nothing for now
    }
    m_grabMutex.unlock();
}

void PreferencesWidget::grabActiveWindow()
{
    m_wantToExit = false;

    if (hideApplication())
    {

        QPixmap pix;

        pix = Graber::grabActiveWindow(m_settings.invertedSelection, m_settings.captureCursor);

        if (!pix.isNull())
        {
            setPixmap(pix);

            processGrabbedImage();
        }
    }
}

void PreferencesWidget::uploadImage(const QString& imageName, bool facebook, bool twitter)
{    
    if (m_grabedImage.isNull())
    {
        Q_ASSERT(m_bxnet);
        if (m_bxnet)
        {
            m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("The image is null can't save it"));
        }
        return;
    }

    QString fileName = imageName.isEmpty()
            ? decorateFileName("Screen shot", m_settings.suffix)
            : imageName;


    bool save = m_grabedImage.save(fileName, 0, m_settings.quality);
    if (!save)
    {
        //try to save to the current directory
        //on some Win7 machines you can't save to the desktop
        QFileInfo fileInfo(fileName);
        fileName = QDir::currentPath() + "/" + fileInfo.fileName();
        save = m_grabedImage.save(fileName, 0, m_settings.quality);
        if(!save)
        {
            Q_ASSERT(m_bxnet);
            if (m_bxnet)
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("Error saving temp file to")+fileName+" !");
            }
            return;
        }
    }

    QFileInfo fileInfo(fileName);
    if (facebook)
    {
        m_facebookList.push_back(fileInfo.fileName());
    }
    if (twitter)
    {
        m_twitterList.push_back(fileInfo.fileName());
    }

    m_bxnet->addFileToUploadQueue(fileName);

}

void PreferencesWidget::notify(const QString& title, const QString& message, SystemTrayIcon_::MessageIcon type)
{
    //CustomNotification::notify(title,message);
    Q_ASSERT(m_systemTrayIcon);
    if (m_systemTrayIcon == NULL)
    {
        return;
    }

    if (m_systemTrayIcon->supportsMessages())
    {
        if (SystemTrayIcon_::UserIcon)
        {

        }

        m_systemTrayIcon->showMessage(title, message, type, 2000, type == SystemTrayIcon_::UserIcon ? QIcon(":/res/icon.ico") : QIcon());
    }
    else
    {
        QMessageBox::information(this, fullAppName() + " " + title, message);
    }
}

bool PreferencesWidget::hideApplication(bool forceHideAllWindows)
{    
    hide();

    if (forceHideAllWindows)
    {

        //if (m_recordingFrame)
        //{
        //    m_recordingFrame->close();
        //}

        if (m_editor && m_editor->isVisible())
        {
            //m_editor->hide();
            m_editor->close();
            if (m_editor->isVisible())
            {
                // if editor still visible, user press cancel
                return false;
            }
        }

        if (m_videoEditor)
        {
            //m_videoEditor>hide();
        }
    }

    // TODO: user can make some changes in dialog. what to do: revert, or apply

    Graber::delay(m_settings.delay > 0 ? m_settings.delay: 250);

    return true;
}

void PreferencesWidget::setPixmap(const QPixmap& pixmap)
{
    m_grabedImage = pixmap;
}

void PreferencesWidget::updateAutoUploadsFromSettings()
{
    bool uploads = m_settings.automaticUpload;

#ifdef Q_OS_LINUX
    ui->actionUploads->setCheckable(true);
    ui->actionUploads->setChecked(uploads);
#else
    ui->actionUploads->setCheckable(false);
#endif
    if (uploads)
    {
        m_systemTrayIcon->setIcon(m_normalAppIcon);
#ifndef Q_OS_LINUX
        ui->actionUploads->setText(tr("Disable uploads"));
#else
        ui->actionUploads->setIcon(QIcon());
#endif
    }
    else
    {
        m_systemTrayIcon->setIcon(m_noUploadAppIcon);
#ifndef Q_OS_LINUX
        ui->actionUploads->setText(tr("Enable uploads"));
#else
        ui->actionUploads->setIcon(QIcon(":/res/icon_disable-upload.png"));
#endif
    }

    ui->disableAutoUploadCheckBox->setChecked(!m_settings.automaticUpload);    
}

void PreferencesWidget::toggleAutoUploads()
{
    m_settings.automaticUpload = ui->disableAutoUploadCheckBox->isChecked();
    m_settings.writeSettings();
    updateAutoUploadsFromSettings();
}

bool PreferencesWidget::openUrl(const QUrl& link)
{
    if (link.isEmpty())
    {
        return false;
    }
    return QDesktopServices::openUrl(link);
}

void PreferencesWidget::signUpClicked(const QUrl& link)
{
    openUrl(link);
}

void PreferencesWidget::on_viewFolder_pushButton_clicked()
{
    if (!m_bxnet->authentificated())
    {
        notify(tr("Information"), tr("Login first for open upload Directory"), SystemTrayIcon_::UserIcon);
        return;
    }
    hide();
    openUrl(m_bxnet->uploadDirectoryLink());
}

void PreferencesWidget::logout()
{
    ui->loginPushButton->setEnabled(true);
    ui->passwordLineEdit->setText("");
    ui->passwordLineEdit->setEnabled(true);
    ui->userEmailLineEdit->setEnabled(true);

    m_bxnet->logout();
    onLogout();
}

void PreferencesWidget::login()
{
    if (m_bxnet->authentificated())
    {
        m_bxnet->logout();
    }

#ifdef UI_EXTERNAL_AUTH
    m_bxnet->login();
#else
    ui->loginPushButton->setEnabled(false);
    ui->userEmailLineEdit->setEnabled(false);
    ui->passwordLineEdit->setEnabled(false);
    const QString name = ui->userEmailLineEdit->text();
    if (name != m_settings.lastUserName)
    {
        m_bxnet->setUploadFolder(DEFAULT_UPLOAD_FOLDER, "");
        m_settings.resetSettings();
    }
#ifdef UI_SSO_LOGIN
    if (ui->passwordLineEdit->text().isEmpty())
    {
        m_bxnet->ssoLogin(name);
    }
    else
#endif
    {
        m_bxnet->directLogin(
                name,
                ui->passwordLineEdit->text());
    }
    ui->passwordLineEdit->setText("");// clear password
#endif
}

void PreferencesWidget::on_logoutPushButton_clicked()
{
    logout();
}

void PreferencesWidget::on_passwordLineEdit_returnPressed()
{
    login();
}

void PreferencesWidget::on_userEmailLineEdit_returnPressed()
{
    ui->passwordLineEdit->setFocus();
}

void PreferencesWidget::on_loginPushButton_clicked()
{
    login();
}

void PreferencesWidget::on_okpushButton_clicked()
{
    saveSettings(); // save changes
    updateAutoUploadsFromSettings(); // update system tray icon

    // update upload folder information:
    folderTextChanged(ui->foldersComboBox->currentText());

    close();
}

void PreferencesWidget::on_cancelpushButton_clicked()
{
    loadSettings(); // restore modified by user setting
    close();
}

void PreferencesWidget::startUploadAnimation()
{
    m_busyIconIndex = UPLOAD_START_ICON;
    m_systemTrayIcon->setIcon(m_busyIcons.at(m_busyIconIndex));

    m_animationTimer.start(200);
}

void PreferencesWidget::stopUploadAnimation()
{
    Q_ASSERT(m_bxnet);
    if (m_bxnet == NULL)
    {
        return;
    }
    if(!m_bxnet->isUploading())
    {
        m_animationTimer.stop();

        //m_busyIconIndex = UPLOAD_FINISH_ICON;
        //m_systemTrayIcon->setIcon(m_busyIcons.at(m_busyIconIndex));

        //QTimer::singleShot(2000, this, SLOT(setNormalIcon()));
        setNormalIcon();
    }
}

void PreferencesWidget::updateUploadAnimation()
{
    Q_ASSERT(m_bxnet);
    if (m_bxnet == NULL)
    {
        return;
    }
    if(!m_bxnet->isUploading())
    {
        stopUploadAnimation();
    }

    int index = m_uploadPercent / 7;
    if(index < 0)
    {
        index = 0;
    }

    if (m_busyIconIndex <= UPLOAD_FINISH_ICON)
    {
        m_busyIconIndex++;

        const int animationIconIndex = (m_busyIconIndex < index) ? m_busyIconIndex : index;
        if (m_busyIconIndex  != animationIconIndex)
        {
            m_busyIconIndex = animationIconIndex;
            if(animationIconIndex >= 0)
            {
                m_systemTrayIcon->setIcon(m_busyIcons.at(animationIconIndex));
            }
        }
    }
}

void PreferencesWidget::setNormalIcon()
{
    m_busyIconIndex = 0;

    if (m_settings.automaticUpload)
    {
        m_systemTrayIcon->setIcon(m_normalAppIcon);
    }
    else
    {
        m_systemTrayIcon->setIcon(m_noUploadAppIcon);
    }
}

void PreferencesWidget::onStartUpload()
{
    if (m_systemTrayIcon)
    {
        m_systemTrayIcon->setToolTip("");
    }

    // Animate the icons
    m_busyIconIndex = 0;
    m_systemTrayIcon->setIcon(m_busyIcons.at(m_busyIconIndex));
    startUploadAnimation();
}

void PreferencesWidget::onUploadFailed(const QString& fileName, BxNet::RESPONSE_STATUS status)
{

    if (m_systemTrayIcon)
    {
        m_systemTrayIcon->setToolTip("");
    }

    switch(status)
    {
    case BxNet::network_error:
        {
            QString message = tr("Unable to upload file");
            if(m_bxnet == NULL)
            {
                notify(tr("Network error"), message, SystemTrayIcon_::Critical);
                break;
            }

            QFileInfo fileInfo(fileName);
            message += " " + fileInfo.fileName();
            notify(tr("Network error"), message, SystemTrayIcon_::Critical);

            if (m_bxnet->authentificated())
            {
                QMessageBox messageBox(this);

                messageBox.setWindowTitle(fullAppName() + tr(" Error"));
                messageBox.setText(tr("Error uploading file"));
                messageBox.setInformativeText(fileInfo.fileName());
                messageBox.setIcon(QMessageBox::Warning);

                messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Retry);

                messageBox.setDefaultButton(QMessageBox::Retry);

                centrateWidget(&messageBox);

                switch(messageBox.exec())
                {
                case QMessageBox::Ok:
                    break;
                case QMessageBox::Retry:
                    m_bxnet->addFileToUploadQueue(fileName);
                    break;
                }

            }
            else
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("Cannot upload fle because the application cannot connect to the internet"));
            }
        }
        break;
    case BxNet::upload_canceled:
        {
            notify(tr("Error"), tr("Uploading canceled by user"), SystemTrayIcon_::Critical);
        }
        break;
    case BxNet::error_open_file:
        {
            Q_ASSERT(m_bxnet);
            if (m_bxnet)
            {
                QFileInfo fileInfo(fileName);
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("Error opening file to upload ") + fileInfo.fileName());
            }
        }
        break;
    case BxNet::replay_parssing_error:
        {
            Q_ASSERT(m_bxnet);
            if (m_bxnet && m_bxnet->authentificated())
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("Application cannot understand the server response"));
            }
        }
        break;
    case BxNet::not_logged_in:
        {
            showOptions(); // raise main window if user not logged
            Q_ASSERT(m_bxnet);
            if (m_bxnet)
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("You are not loged in"));
            }
        }
        break;
    case BxNet::upload_invalid_file_name:
        {
            Q_ASSERT(m_bxnet);
            if (m_bxnet)
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("The file name is not valid"));
            }
        }
        break;
    case BxNet::application_restricted:
        {
            Q_ASSERT(m_bxnet);

            if (m_bxnet)
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error"), tr("The application is restricted to do this upload"));
            }
        }
        break;
    default:
        {
            Q_ASSERT(m_bxnet);
            if (m_bxnet)
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("Error uploading file"));
            }
        }
    }
    stopUploadAnimation();
    setNormalIcon();
}

void PreferencesWidget::onUploadCompleted(const QString& fileName)
{
    //hide the progress
    if (m_systemTrayIcon)
    {
        m_systemTrayIcon->setToolTip("");
    }

    //this->progressNotification->hide();
    //this->showUploadProgress=false;

    //QApplication::clipboard()->setText(fileName);

    if (m_readonlyFilesToUpload.contains(fileName))
    {
        m_readonlyFilesToUpload.remove(fileName);
        return;
    }

    if (m_otherFilesToUpload.contains(fileName))
    {
        if (m_settings.delete_other_files)
        {
            const QString str = m_otherFilesToUpload.value(fileName);
            bool res = QFile::remove(str);
            if (!res)
            {
                notify(tr("Error"), tr("Can't delete file ") + fileName, SystemTrayIcon_::Critical);
            }
        }
        m_otherFilesToUpload.remove(fileName);
        return;
    }
    if (m_temporaryUploadFile.contains(fileName))
    {
        QFile::remove(m_temporaryUploadFile.value(fileName));
        m_temporaryUploadFile.remove(fileName);

        if (m_settings.delete_other_files)
        {
            if (m_zippedUploadFiles.contains(fileName))
            {
                const QStringList& list = m_zippedUploadFiles[fileName];
                foreach(const QString& str, list)
                {
                    deletePath(str);
                }
                m_zippedUploadFiles.remove(fileName);
            }

        }

        return;
    }

    if(m_settings.delete_file)
    {

        QDir desktop(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
        bool res = desktop.remove(fileName);
        if (!res)
        {
            // TODO: need absolute fileName
            QDir temp(QDesktopServices::storageLocation(QDesktopServices::TempLocation));
            res = temp.remove(fileName);
            if (!res)
            {
                notify(tr("Error"), tr("can't delete file"), SystemTrayIcon_::Critical);
            }
        }
    }    
}

void PreferencesWidget::onChangeUserIcon(QPixmap pixmap)
{
    if (pixmap.isNull())
    {
        ui->lbUserIcon->setPixmap(QPixmap(":/res/avatar.png"));
    }
    else
    {
        ui->lbUserIcon->setPixmap(pixmap);
    }
}

void PreferencesWidget::onAuthError(BxNet::RESPONSE_STATUS err)
{
    //the user has not loged in in the browser
    ui->loginPushButton->setEnabled(true);
    ui->userEmailLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);

    //ui->details_label->setText(tr("You are not logged in"));
    clearTextBrowser();

    switch(err)
    {
    case BxNet::not_logged_in:
        {
#ifdef UI_EXTERNAL_AUTH
            //attempting an automatic login
            login();
#else
            notify(tr("Warning"), tr("You are not loggedin, try to autologin"), SystemTrayIcon_::UserIcon);
            showOptions();
#endif
            break;
        }
    case BxNet::application_restricted:
        {        
            QMessageBox::warning(this, fullAppName() + tr(" Error" ), tr("The application is blocked by box.net. Contact us please to check this issue"));
            break;
        }
    case BxNet::get_auth_token_error:
        {
            Q_ASSERT(m_bxnet);
            if (m_bxnet)
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("A generic error ocured, try again and report the problem to us"));
            }
            break;
        }
    case BxNet::network_error:
        {
            // do nothing, becoause onError slot will show information about network errors
            //Q_ASSERT(m_bxnet);
            //if (m_bxnet)
            //{
            //    m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("The application can't comunicate with the server"));
            //}
            break;
        }
    case BxNet::ssl_error:
        {
            Q_ASSERT(m_bxnet);
            if (m_bxnet)
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("SSL handshake failed"));
            }
            break;
        }
    case BxNet::replay_parssing_error:
        {
            Q_ASSERT(m_bxnet);
            if (m_bxnet)
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("The aplication does not understand the server reply"));
            }
            break;
        }
    case BxNet::login_limit:
    case BxNet::invalid_login:
        {
            QMessageBox::warning(this, fullAppName() + tr(" Error" ), (err == BxNet::login_limit)
                                                                            ? tr("Logins limit reached")
                                                                            : tr("Invalid login information"));
            if (ui->userEmailLineEdit->text().isEmpty())
            {
                ui->userEmailLineEdit->setFocus();
            }
            else
            {
                ui->passwordLineEdit->setFocus();
            }
            break;
        }
    default:
        {
            Q_ASSERT(m_bxnet);
            if (m_bxnet)
            {
                m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), tr("Unexpected error type ") + QString::number(err));
            }
            qDebug() << "This error type should never be here in onAutentificationError " << err;
            break;
        }
    }
}

void PreferencesWidget::startUploadingNonGrabedPath(const QString& path)
{
    if (path.isEmpty())
    {
        return;
    }

    QDir dir(makeFullFileName(path));
    if (dir.exists())
    {
        startUploadingNonGrabedDirectory(path);
    }
    else
    {
        startUploadingNonGrabedImageFile(path);
    }
}

void PreferencesWidget::startUploadingNonGrabedDirectory(const QString& path)
{
    if (path.isEmpty())
    {
        return;
    }

    QDir dir(makeFullFileName(path));
    if (dir.exists())
    {
        dir.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
        foreach(const QFileInfo& fileInfo, dir.entryInfoList())
        {
            if (fileInfo.isFile())
            {
                startUploadingNonGrabedImageFile(fileInfo.absoluteFilePath());
            }
            else
            {
                startUploadingNonGrabedDirectory(fileInfo.absoluteFilePath());
            }
        }
    }
}

void PreferencesWidget::startUploadingNonGrabedImageFile(const QString& fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }

    QString fullFileName = makeFullFileName(fileName);
    QFileInfo file(fullFileName);
    if (!file.exists())
    {
        Q_ASSERT(m_bxnet);
        if (m_bxnet)
        {
            m_bxnet->warningWithLog(this, fullAppName() + tr(" Warning"), tr("File not found") + ": " + fileName);
        }
        return;
    }

    m_bxnet->addFileToUploadQueue(fullFileName);
    //add this files to the special list,because they are treated diffrent
    //NOT DELETED as Grabed images are and other diferences
    //lke they are in difrent locations where grabed files are on desktop
    m_otherFilesToUpload[file.fileName()] = fullFileName;
}

void PreferencesWidget::uploadFiles(const QStringList& files)
{
    if (files.isEmpty())
    {
        return;
    }

    bool singleFile = false;

    if (files.count() == 1)
    {
        if (files.at(0).isEmpty())
        {
            return;
        }
        QFileInfo fileInfo(files.at(0));
        singleFile = fileInfo.isFile();
    }

    if (singleFile)
    {
        foreach(const QString& file, files)
        {
            qDebug() << "startUploadingNonGrabedPath(" << file << ")";
            startUploadingNonGrabedPath(file);
        }
    }
    else
    {
        // don't upload directory or files one by one. archive them first.
        QString archiveName = "Archive";
        if (files.count() == 1)
        {
            archiveName = QFileInfo(files.at(0)).fileName();
        }

        ZipHelper* zipHelper = new ZipHelper(decorateFileName(archiveName, "zip", true), this);
        connect(zipHelper, SIGNAL(compressed(QString)), this, SLOT(onCompressed(QString)));
        zipHelper->compressPaths(files);
        // change icon, so user will know, that we do something
        m_systemTrayIcon->setIcon(m_busyIcons.at(UPLOAD_START_ICON+3)); // TODO: animation for compressing
    }
}

void PreferencesWidget::onCompressed(const QString& zipName)
{
    ZipHelper* zipHelper = qobject_cast<ZipHelper*>(sender());

    setNormalIcon();
    m_bxnet->addFileToUploadQueue(zipName);

    QFileInfo fileInfo(zipName);
    m_temporaryUploadFile[fileInfo.fileName()] = zipName;

    if (m_settings.delete_other_files)
    {
        m_zippedUploadFiles[fileInfo.fileName()] = zipHelper->paths();
    }

    zipHelper->deleteLater();
}

void PreferencesWidget::clearTextBrowser()
{
    ui->textBrowser->setText(tr("<p><b>Don't have an account?</b></p>")
                           + tr("<p><a href=\"http://www.box.net/ref/simpleshare\">Register for a free 5GB account</a> or <a a href=\"http://www.box.net\">learn more</a></p>"));
}

void PreferencesWidget::onLogout()
{
    //ui->details_label->setText(tr("You are not logged in"));
    clearTextBrowser();

    ui->userEmailLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginPushButton->setEnabled(true);
    ui->userEmailLineEdit->setFocus();

    ui->loggedWidget->setVisible(false);
    ui->loginWidget_2->setVisible(true);
    ui->registerWidget->setVisible(true);
    ui->viewFolderWidget->setVisible(false);
    ui->instructionWidget->setVisible(false);

    onChangeUserIcon(QPixmap());
    onUpdateAccountInfo();

    m_blockFolderModifications = true;
    m_folders.clear();
    ui->foldersComboBox->clear();
    //ui->foldersComboBox->setEditText(DEFAULT_UPLOAD_FOLDER);

    ////ui->foldersComboBox->setEnabled(false);

    m_blockFolderModifications = false;
}

void PreferencesWidget::onLogin()
{
    //check the folder Grabs if is not there create it
    //m_bxnet->CreateFolder(UPLOAD_FOLDER);

    ui->userEmailLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginPushButton->setEnabled(true);

    notify(tr("Box SimpleShare"), tr("Login Successful\nuser: ") + m_bxnet->userName(), SystemTrayIcon_::UserIcon);

    //ui->details_label->setText(tr("You are now logged in as\n"+
    //                              Global::getGlobal()->userSession->login.toAscii()));
    //ui->textBrowser->setText(tr(MSG_LOGINED +
    //                            Global::getGlobal()->userSession->login.toAscii()));
    ui->registerWidget->setVisible(false);
    //ui->logout_pushButton->setVisible(true);
    ui->lbUsername->setText(m_bxnet->userName());
    ui->loggedWidget->setVisible(true);
    ui->viewFolderWidget->setVisible(true);
    ui->loginWidget_2->setVisible(false);
    ui->instructionWidget->setVisible(true);

    onUpdateAccountInfo();
    m_settings.lastUserName = m_bxnet->userName();

    ui->foldersComboBox->setEditText(m_bxnet->uploadFolder());

    //m_bxnet->createBookmark("test", "www.example.com");

    close(); // hide main window
}

void PreferencesWidget::onUpdateAccountInfo()
{   
    QString usedInfo;
    if (m_bxnet->authentificated())
    {
        double spaceAmount  = m_bxnet->spaceAmount()/1073741824.0;  // 1024*1024*1024
        double spaceUsed    = m_bxnet->spaceUsed()/1073741824.0;    // 1024*1024*1024
        double percentUsed  = (spaceUsed / spaceAmount) * 100.0;

        usedInfo.sprintf("%4.2f%% ( %4.2f GB) of %4.2f GB used", percentUsed, spaceUsed, spaceAmount);

        //updateFoldersList(); // update information about folders
    }

    ui->lbUsedInfo->setText(usedInfo);
}

void PreferencesWidget::updateUserStorageInfo()
{
    if (m_bxnet && m_bxnet->authentificated() && isConnectedToNetwork())
    {
        m_bxnet->getAccountInfo(true); // silent (don't show error messages to user)

        m_bxnet->readTree(0, true, true);
    }
}

void PreferencesWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_updateTimerId)
    {
        updateUserStorageInfo();
    }
    else
    {
        QWidget::timerEvent(event);
    }
}

void PreferencesWidget::notifyUrlUploaded(const QString& link, const QString& fileName, bool clipboard)
{
    QString message;
    if (!fileName.isEmpty())
    {
        message += tr("File \"") + QFileInfo(fileName).fileName() + "\" uploaded\n";
    }    
    message += "URL " + link;
    if (clipboard)
    {
        message += tr(" has been pasted to your clipboard");
    }

    stopUploadAnimation();
    notify(tr("Upload complete"), message, SystemTrayIcon_::UserIcon);
}

void PreferencesWidget::onShareSuccessfull(const QString& fileName, const QString& link)
{
    if (link.isEmpty())
    {
        return;
    }

    if (m_facebookList.contains(fileName))
    {
        m_facebookList.removeOne(fileName);
        shareOnFacebook(link);
    }
    if (m_twitterList.contains(fileName))
    {
        m_twitterList.removeOne(fileName);
        shareOnTweeter(link);
    }

    if (m_settings.copyUrlToClipboard)
    {
        QApplication::clipboard()->setText(link);

#ifdef UI_SHORTEN_URL
        if (m_settings.urlShortener)
        {
            Q_ASSERT(m_urlShortener);
            if (m_urlShortener)
            {
                m_urlShortener->shorten(link, fileName);
            }
        }
        else
#endif
        {
            notifyUrlUploaded(link, fileName);
            stopUploadAnimation();
        }
    }
    else
    {
        notifyUrlUploaded(link, fileName, false);
        stopUploadAnimation();
    }
    readRecentActionsFromSettings(); // TODO: insertRecentAction(?, link);

    updateUserStorageInfo();
}

#ifdef UI_SHORTEN_URL

void PreferencesWidget::onCantShorten(const QString& message)
{
    //TODO: Is that really need to show warnings from Shorten APIs?
    notify(tr("Warning"), message);
    stopUploadAnimation();
}

void PreferencesWidget::onShortenLink(const QString& link, const QString& fileName)
{
    if (!link.isEmpty())
    {
        QApplication::clipboard()->setText(link);
        notifyUrlUploaded(link, fileName);
        stopUploadAnimation();
    }
}

#endif

void PreferencesWidget::onShareFailed(BxNet::RESPONSE_STATUS status)
{
    Q_UNUSED(status);

    stopUploadAnimation();
    setNormalIcon();
}

void PreferencesWidget::onNotLoggedIn()
{
    ui->userEmailLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginPushButton->setEnabled(true);

    if (isConnectedToNetwork())
    {
        //showOptions(); // show dialog to user for logging on
        notify(tr("Network error"), tr("You are not logged in"), SystemTrayIcon_::Critical);
#ifdef UI_EXTERNAL_AUTH
        login();
#else
        showOptions(); // show dialog to user for logging on
#endif
    }
    else
    {
        notify(tr("Network error"), tr("Disconnected from network"), SystemTrayIcon_::Critical);
    }
}

void PreferencesWidget::onNetworkError(QNetworkReply::NetworkError error, const QString& errorString)
{
    Q_UNUSED(errorString);

    ui->userEmailLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginPushButton->setEnabled(true);

    switch(error)
    {
    case QNetworkReply::NoError:
        // do nothing;
        break;
    default:
        const QString str = "A network error #" + QString::number(error) + " occured.\n" + errorString;
        qDebug() << str;
/*
        Q_ASSERT(m_bxnet);
        if (m_bxnet)
        {
            if (m_bxnet->authentificated())
            {
                if (error == 3)
                {
                    // usally this error occured at startup. hide it for now
                    qDebug() << "network error #3: host not found. this is never should be";
                }
                else
                {
                    //m_bxnet->warningWithLog(this, fullAppName() + tr(" Error" ), str);
                    notify(tr("Network error" ), str, SystemTrayIcon_::Critical);
                }
            }
        }
*/
        break;
    }

    setNormalIcon();
}

//TODO: ???????

void PreferencesWidget::onCreateFolderError(BxNet::RESPONSE_STATUS status)
{
    if (status == BxNet::not_logged_in)
    {
        //the user was not logged in
        onNotLoggedIn();
    }
    else
    {
        if(status == BxNet::network_error)
        {
            //we faield to create the folder and the networking is down
            //try again in a few seconds
            QTimer::singleShot(5000, this, SLOT(initializeBoxNet()));
        }
        else
        {
            if(m_bxnet->authentificated())
            {
                notify(tr("Error"), tr("Error creating uploads folder"), SystemTrayIcon_::Critical);
            }
        }
    }
}

void PreferencesWidget::onErrorOpenUrl(QString url)
{
    QString message=tr("Because of a strange problem the application could not open a browser, navigate manualy to the link below (we placed the link into the clipboard too)\n");
    if(m_settings.copyUrlToClipboard)
    {
        QApplication::clipboard()->setText(url);
    }
    QMessageBox::warning(this, fullAppName() + tr(" Warning"), message + url);
}

void PreferencesWidget::loadSettings()
{
    //readSettings doesn't need becoause they already loaded in Settings::Settings() constructor
    //m_settings.readSettings(); // sync
    //load the settings current saved
    ui->deleteAfterUploadCheckBox->setChecked(m_settings.delete_file);
    ui->deleteOtherCheckBox->setChecked(m_settings.delete_other_files);
    ui->copyUrlCheckBox->setChecked(m_settings.copyUrlToClipboard);
    ui->disableAutoUploadCheckBox->setChecked(!m_settings.automaticUpload);
    //ui->LaunchOnClickCheckBox->setChecked(m_settings.launchEditorOnClick);
#ifdef UI_SHORTEN_URL
    ui->urlShortenerCheckBox->setChecked(m_settings.urlShortener);
#endif
    ui->startAtLoginCheckBox->setChecked(m_settings.startAtLogin);
    ui->muteCheckBox->setChecked(m_settings.mute);
    //ui->sslCheckBox->setChecked(m_settings.https);
    ui->editorCheckBox->setChecked(m_settings.editor);
    //ui->invertedCheckBox->setChecked(m_settings.invertedSelection);
    //ui->cursorCheckBox->setChecked(m_settings.captureCursor);
    //ui->formatComboBox->setCurrentIndex(m_settings.suffix == "png" ? 0 : 1);
    //ui->qualitySlider->setValue(m_settings.quality);
    //ui->delaySpinBox->setValue(m_settings.delay);
    // shortcuts
    if (!m_settings.fullScreenKey.isEmpty() && !m_settings.fullScreenKey.isNull())
    {
        m_fullScreenShortcut->setShortcut(m_settings.fullScreenKey);
    }
    if (!m_settings.regionKey.isEmpty() && !m_settings.regionKey.isNull())
    {
        m_regionShortcut->setShortcut(m_settings.regionKey);
    }
    if (!m_settings.activeWindowKey.isEmpty() && !m_settings.activeWindowKey.isNull())
    {
        m_activeWinShortcut->setShortcut(m_settings.activeWindowKey);
    }
    if (!m_settings.screencastKey.isEmpty() && !m_settings.screencastKey.isNull())
    {
        m_screencastShortcut->setShortcut(m_settings.screencastKey);
    }
    if (!m_settings.uploadKey.isEmpty() && !m_settings.uploadKey.isNull())
    {
        m_uploadShortcut->setShortcut(m_settings.uploadKey);
    }
    //if (!m_settings.repeatKey.isEmpty() && !m_settings.repeatKey.isNull())
    //{
    //    m_repeatShortcut->setShortcut(m_settings.repeatKey);
    //}
    if (!m_settings.annotateKey.isEmpty() && !m_settings.annotateKey.isNull())
    {
        m_annotateShortcut->setShortcut(m_settings.annotateKey);
    }
    updateShortcutsControls();

    updateAutoStartup(); // update on first run (can be used for installer)

    // load proxy settings
    //ui->proxyLineEdit->setText(m_settings.proxy);
}

void PreferencesWidget::saveSettings()
{
    m_settings.delete_file          = ui->deleteAfterUploadCheckBox->isChecked();
    m_settings.delete_other_files   = ui->deleteOtherCheckBox->isChecked();
    m_settings.copyUrlToClipboard   = ui->copyUrlCheckBox->isChecked();
    m_settings.automaticUpload      = !ui->disableAutoUploadCheckBox->isChecked();
    m_settings.startAtLogin         = ui->startAtLoginCheckBox->isChecked();
    //m_settings.launchEditorOnClick  = ui->LaunchOnClickCheckBox->isChecked();
#ifdef UI_SHORTEN_URL
    m_settings.urlShortener         = ui->urlShortenerCheckBox->isChecked();
#endif
    m_settings.mute                 = ui->muteCheckBox->isChecked();
    //m_settings.https                = ui->sslCheckBox->isChecked();
    m_settings.editor               = ui->editorCheckBox->isChecked();
    //m_settings.invertedSelection    = ui->invertedCheckBox->isChecked();
    //m_settings.captureCursor        = ui->cursorCheckBox->isChecked();
    //m_settings.suffix               = ui->formatComboBox->currentIndex() == 0 ? "png" : "jpg";
    //m_settings.quality              = ui->qualitySlider->value();
    //m_settings.delay                = ui->delaySpinBox->value();

    m_settings.fullScreenKey    = m_fullScreenShortcut->shortcut().toString();
    m_settings.regionKey        = m_regionShortcut->shortcut().toString();
    m_settings.activeWindowKey  = m_activeWinShortcut->shortcut().toString();
    m_settings.screencastKey    = m_screencastShortcut->shortcut().toString();
    m_settings.uploadKey        = m_uploadShortcut->shortcut().toString();
    //m_settings.repeatKey        = m_repeatShortcut->shortcut().toString();
    m_settings.annotateKey      = m_annotateShortcut->shortcut().toString();

    //m_settings.proxy            = ui->proxyLineEdit->text();

    m_settings.writeSettings(); // sync
    updateAutoStartup();
}

void PreferencesWidget::updateAutoStartup()
{
    if(m_settings.startAtLogin)
    {
        createStartupItem();
    }
    else
    {
        removeStartupItem();
    }
}

void PreferencesWidget::createStartupItem()
{
#ifdef Q_OS_LINUX
    // TODO: should be $XDG_CONFIG_HOME

    QDir dir(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    if (!QFileInfo(dir.path() + "/.config/autostart/box-simpleshare.desktop").exists())
    {
        if (dir.mkpath(".config/autostart"))
        {
            QFile::copy("/usr/share/applications/box-simpleshare.desktop", dir.path() + "/.config/autostart/box-simpleshare.desktop");
        }
    }

#endif

#ifdef Q_WS_WIN
    //set the registy value in windows to launch this application
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    const QString appPath = QDir::toNativeSeparators(QApplication::applicationFilePath());
    settings.setValue("SimpleShare", "\"" + appPath + "\"");
#endif

#ifdef Q_WS_MAC
        CFURLRef itemURL = CFURLCreateWithString(NULL, CFSTR("file:////Applications/SimpleCapture.app"), NULL);

        LSSharedFileListRef loginListRef = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);

        if (loginListRef)
        {
                // Insert the item at the bottom of Login Items list.
                LSSharedFileListItemRef loginItemRef = LSSharedFileListInsertItemURL(loginListRef,
                                                                                     kLSSharedFileListItemLast,
                                                                                     NULL,
                                                                                     NULL,
                                                                                     itemURL,
                                                                                     NULL,
                                                                                     NULL);
                if (loginItemRef)
                {
                        CFRelease(loginItemRef);
                }
                CFRelease(loginListRef);
        }
#endif


}

void PreferencesWidget::removeStartupItem()
{
#ifdef Q_OS_LINUX
    // TODO: should be $XDG_CONFIG_HOME

    QDir dir(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    QFile::remove(dir.path() + "/.config/autostart/box-simpleshare.desktop");
#endif


#ifdef Q_WS_WIN
    //set the registy value in windows to launch this application
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    const QString appPath = QDir::toNativeSeparators(QApplication::applicationFilePath());
    settings.setValue("SimpleShare", "");
#endif

#ifdef Q_WS_MAC
        // This will retrieve the path for the application
        // For example, /Applications/test.app
        CFURLRef itemURL = CFURLCreateWithString(NULL, CFSTR("file:////Applications/SimpleCapture.app"), NULL);

        // Create a reference to the shared file list.
        LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL,
                                                                kLSSharedFileListSessionLoginItems, NULL);

        if (loginItems)
        {
                UInt32 seedValue;
                //Retrieve the list of Login Items and cast them to
                // a NSArray so that it will be easier to iterate.
                CFArrayRef loginItemsArray = LSSharedFileListCopySnapshot(loginItems, &seedValue);
                int i = 0;
                for(i; i < CFArrayGetCount(loginItemsArray); i++)
                {
                        LSSharedFileListItemRef itemRef = (LSSharedFileListItemRef)CFArrayGetValueAtIndex(loginItemsArray, i);
                        //Resolve the item with URL
                        if (LSSharedFileListItemResolve(itemRef, 0, &itemURL, NULL) == noErr) {
                                CFStringRef urlPath = CFURLCopyPath(itemURL);
                                if (CFStringCompare(urlPath, CFSTR("/Applications/SimpleCapture.app/"), 0) == kCFCompareEqualTo){
                                        LSSharedFileListItemRemove(loginItems, itemRef);
                                }
                        }
                }
                CFRelease(loginItemsArray);
        }
#endif
}

void PreferencesWidget::beginCaptureFullScreenVideo()
{
    m_wantToExit = false;

    startCaptureVideo(QRect());
}

void PreferencesWidget::beginCaptureRegionVideo()
{
    m_wantToExit = false;

    if (!m_grabMutex.tryLock())
    {
        qDebug() << "prevent multiple region grabbing";
        return;
    }
    try
    {
        if(!hideApplication())
        {
            return;
        }

        QRect rect = Graber::doSelectRect(true, m_settings.invertedSelection);

        if (!rect.isEmpty())
        {
            const QRect geometry=Graber::getVirtualDesktopGeometry();
            rect.translate(geometry.left(), geometry.top());

            startCaptureVideo(rect);
        }
    }
    catch(...)
    {
        // do nothing for now
    }
    m_grabMutex.unlock();
}

void PreferencesWidget::on_actionVideo_triggered()
{
    if (m_videoGraber.isActive())
    {
        endCaptureVideo();
    }
    else
    {
        beginCaptureRegionVideo();
    }
}

void PreferencesWidget::startCaptureVideo(QRect rect)
{
    endCaptureVideo();

    //m_videoGraber.setMute(m_settings.mute);
    if (!m_videoGraber.start(rect, decorateFileName("Screencast", "flv", true), !m_settings.mute))
    {
        qDebug() << "can't start video capture";
#ifdef Q_OS_LINUX
        notify(tr("Error" ), tr("can't start video capture.\n Check that ffmpeg installed."));
#else
        notify(tr("Error" ), tr("can't start video capture."), SystemTrayIcon_::Critical);
#endif
        return;
    }

    if (m_bxnet && m_bxnet->maxUploadFileSizeLimit())
    {
        m_videoGraber.setMaxSize(qRound(m_bxnet->maxUploadFileSizeLimit() * 0.95));
    }
    else
    {
        m_videoGraber.setMaxSize(0); // no limit in offline mode
    }
}

void PreferencesWidget::endCaptureVideo()
{
    if (m_videoGraber.isActive())
    {
        m_videoGraber.stop();
    }
}

void PreferencesWidget::muteVideo(bool muted)
{
    Q_UNUSED(muted);
    //m_settings.mute = muted;
    //m_videoGraber.setMute(m_settings.mute);
}

void PreferencesWidget::onStartScreencast(QString fileName)
{
    Q_UNUSED(fileName);

    if (m_recordingFrame == NULL)
    {
        m_recordingFrame = new RecordingFrame(1, m_settings.mute);
    }

    m_recordingFrame->setInnerRect(m_videoGraber.rect());
    connect(m_recordingFrame.data(), SIGNAL(stopRecording()), this, SLOT(endCaptureVideo()));
    connect(m_recordingFrame.data(), SIGNAL(mute(bool)), this, SLOT(muteVideo(bool)));
    m_recordingFrame->show();

    ui->actionVideo->setText(tr("Stop recording"));
}

void PreferencesWidget::onStopScreencast(QString fileName)
{
    QFileInfo fileInfo(fileName);

    if (fileInfo.exists() && fileInfo.size() > 0)
    {
        //if (!m_settings.delete_file) // always copy file to desktop
        {
            // if we don't delete file from temp location. move it to desktop
            const QString folder = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);

            QString newFilePath = folder + "/" + fileInfo.fileName();
            if (renameFile(fileName, newFilePath))
            {
                fileName = newFilePath;
            }
        }

        if (m_settings.automaticUpload)
        {
            m_bxnet->addFileToUploadQueue(fileName);
            // TODO: refactor this
            //add this files to the special list,because they are treated diffrent
            //NOT DELETED as Grabed images are and other diferences
            //lke they are in difrent locations where grabed files are on desktop
            //m_otherFilesToUpload[fileInfo.fileName()] = m_currentVideoFileName;
        }
    }

    if (m_recordingFrame.data())
    {
        m_recordingFrame->close();
    }

    ui->actionVideo->setText(tr("Record video"));
}

void PreferencesWidget::onMaximumSizeReached()
{
    notify(tr("Warning"), tr("Screencast stopped. Maximum filesize reached"), SystemTrayIcon_::UserIcon);
    endCaptureVideo();
}

QString PreferencesWidget::decorateFileName(const QString& prefix, const QString& ext, bool tempLocation) const
{
    const QString folder = QDesktopServices::storageLocation(tempLocation ? QDesktopServices::TempLocation : QDesktopServices::DesktopLocation);
    QDateTime currentTime = QDateTime::currentDateTime();
    QString sufix = currentTime.date().toString("yyyy-MM-dd") + " at " + currentTime.time().toString("h.mm.ss A");

    QString result = folder + "/" + prefix + " " + sufix + "." + ext;

    int number = 0;
    while (QFileInfo(result).exists() && number < 100)
    {
        ++number;
        result = folder + "/" + prefix + " " + sufix + "-" + QString::number(number) +  "." + ext;
    }

    return result;
}

QString PreferencesWidget::makeFullFileName(const QString& fileName) const
{
    QString fullFileName;
    QFileInfo file(fileName);
    if (file.exists())
    {
        return file.absoluteFilePath();
    }

    if (!file.exists())
    {
        QFileInfo file2(QDir::currentPath() + "/" + fileName);
        if (file2.exists())
        {
            return file2.absoluteFilePath();
        }
    }
    return fileName;
}

// shortcuts

void PreferencesWidget::acceptShortcutFullScreen(const QKeySequence& shortcut)
{
    if (!m_fullScreenShortcut->setShortcut(shortcut))
    {
        ui->fullScreenKeyWidget->clearKeySequence();
    }
    saveSettings();
}

void PreferencesWidget::acceptShortcutRegion(const QKeySequence& shortcut)
{
    if (!m_regionShortcut->setShortcut(shortcut))
    {
        ui->regionKeyWidget->clearKeySequence();
    }
    saveSettings();
}

void PreferencesWidget::acceptShortcutScreencast(const QKeySequence& shortcut)
{
    if (!m_screencastShortcut->setShortcut(shortcut))
    {
        ui->screencastKeyWidget->clearKeySequence();
    }
    saveSettings();
}

void PreferencesWidget::acceptShortcutUpload(const QKeySequence& shortcut)
{
    if (!m_uploadShortcut->setShortcut(shortcut))
    {
        ui->uploadKeyWidget->clearKeySequence();
    }
    saveSettings();
}

void PreferencesWidget::acceptShortcutWindow(const QKeySequence& shortcut)
{
    if (!m_activeWinShortcut->setShortcut(shortcut))
    {
        ui->windowKeyWidget->clearKeySequence();
    }
    saveSettings();
}

//void PreferencesWidget::acceptShortcutRepeat(const QKeySequence& shortcut)
//{
//      if (!m_repeatShortcut->setShortcut(shortcut))
//      {
//      //  ui->repeatWidget->clearKeySequence();
//      }
//      saveSettings();
//}

void PreferencesWidget::acceptShortcutAnnotate(const QKeySequence& shortcut)
{
    if (!m_annotateShortcut->setShortcut(shortcut))
    {
        ui->annotateKeyWidget->clearKeySequence();
    }
    saveSettings();
}

void PreferencesWidget::startShortcutRecording()
{
    m_disableShortcutsForEditing = true;

    //m_fullScreenShortcut->setEnabled(false);
    //m_regionShortcut->setEnabled(false);
    //m_activeWinShortcut->setEnabled(false);
    //m_screencastShortcut->setEnabled(false);
    //m_uploadShortcut->setEnabled(false);
    //m_repeatShortcut->setEnabled(false);
}

void PreferencesWidget::endShortcutRecording()
{
    m_disableShortcutsForEditing = false;

    //m_fullScreenShortcut->setEnabled();
    //m_regionShortcut->setEnabled();
    //m_activeWinShortcut->setEnabled();
    //m_screencastShortcut->setEnabled();
    //m_uploadShortcut->setEnabled();
    //m_repeatShortcut->setEnabled();
}

void PreferencesWidget::onAddUploadToLog(const QString &fileName, const QString &link)
{
    m_settings.addToRecent(fileName, link);
}

// updates

void PreferencesWidget::checkUpdates()
{
    if (m_updateManager == NULL)
    {
        m_updateManager = new BasicUpdater(this);
        connect(m_updateManager, SIGNAL(updateAvailable(QString, QString)), this, SLOT(onUpdatesAvailable(QString, QString)));
    }
    m_updateManager->checkForUpdates();
}

void PreferencesWidget::onUpdatesAvailable(const QString& version, const QString& link)
{
    QString message = tr("A newer version %1 of this application is available for download.\n"
               "current version is %2\n"
               "Do you really want to update?").arg(version).arg(QApplication::applicationVersion());

    QMessageBox messageBox(this);

    messageBox.setText(tr("Box SimpleShare update available"));
    messageBox.setInformativeText(message);
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::Yes);
    messageBox.setIconPixmap(QIcon(":/res/icon.ico").pixmap(64));
    messageBox.setButtonText( QMessageBox::Yes, "Update now" );
    messageBox.setButtonText( QMessageBox::No, "Update later" );

    centrateWidget(&messageBox);

    switch (messageBox.exec())
    {
    case QMessageBox::Yes:
        {
            try
            {
                QDesktopServices::openUrl(QUrl(link));
                qApp->quit(); // quit becoaus of updater
            }
            catch(...)
            {
                QMessageBox::warning(this, fullAppName() + tr("Update error"), tr("Invalid update link recived from server"));
            }
            break;
        }
    case QMessageBox::No:
        // do nothing for now
        break;
    case QMessageBox::Cancel:
        // do nothing for now
        break;
    }
}


void PreferencesWidget::onUploadFilesFromShell()
{
#ifdef Q_OS_WIN
    bool wasTriggered = false;

    wchar_t* buff = NULL;
    static HMODULE s_shellutils = LoadLibraryW(L"shellutils.dll");
    if (s_shellutils)
    {
        static SelectedFilesFunc     f_SelectedFiles        = reinterpret_cast<SelectedFilesFunc>(GetProcAddress(s_shellutils, "SelectedFiles"));
        static FreeSelectedFilesFunc f_FreeSelectedFiles    = reinterpret_cast<FreeSelectedFilesFunc>(GetProcAddress(s_shellutils, "FreeSelectedFiles"));
        if (f_SelectedFiles && f_FreeSelectedFiles)
        {
            buff = f_SelectedFiles();

            if (buff)
            {
                QString files = QString::fromWCharArray(buff);

                f_FreeSelectedFiles(buff);

                qDebug() << "Upload from Shell " << files;
                uploadFiles(files.split(";"));
                wasTriggered = true;
            }
        }
    }

    if (buff == NULL)
    {
        QString activeTitle = activeWindowTitle();

        if (activeTitle.contains("Microsoft Word"))
        {
            QString tmpFileName = uploadFromOffice("Word.Application");
            qDebug() << "Upload ActiveDocument from Word.Application" << tmpFileName;
            m_bxnet->addFileToUploadQueue(tmpFileName);
            m_readonlyFilesToUpload[QFileInfo(tmpFileName).fileName()] = tmpFileName;
            wasTriggered = true;
        }

        if (activeTitle.contains("Microsoft Excel"))
        {
            QString tmpFileName = uploadFromOffice("Excel.Application");
            qDebug() << "Upload ActiveDocument from Excel.Application" << tmpFileName;
            m_bxnet->addFileToUploadQueue(tmpFileName);
            m_readonlyFilesToUpload[QFileInfo(tmpFileName).fileName()] = tmpFileName;
            wasTriggered = true;
        }
        /*
        if (activeTitle.contains("Microsoft PowerPoint"))
        {
            QString tmpFileName = uploadFromOffice("PowerPoint.Application");
            qDebug() << "Upload ActiveDocument from PowerPoint.Application" << tmpFileName;
            m_bxnet->addFileToUploadQueue(tmpFileName);
            m_readonlyFilesToUpload[file.fileName()] = fullFileName;
                wasTriggered = true;
        }
        */
    }

    if (!wasTriggered)
    {
        onUploadFile();
    }
#endif
}


void PreferencesWidget::on_defaultsShortcutsPushButton_clicked()
{
    resetShortcutsToDefaults();
}

QPoint PreferencesWidget::tooTipPos() const
{
    QPoint pos;
    if (isVisible())
    {
        QWidget* widget = focusWidget();
        if (widget)
        {
            pos = widget->mapToGlobal(widget->rect().topLeft());
        }
        else
        {
            pos = QCursor::pos();
        }
    }
    else
    {
        pos = QCursor::pos();
    }

    return pos;
}

void PreferencesWidget::shortcutInUse()
{    
    QToolTip::showText(tooTipPos(), tr("this key sequence already in use"), this);
}

void PreferencesWidget::keyNotSupported()
{
    QToolTip::showText(tooTipPos(), tr("key not supported"), this);
}

void PreferencesWidget::shareOnFacebook(const QString& link)
{
    QDesktopServices::openUrl(QUrl("http://www.facebook.com/sharer.php?u=" + link));
    /*
    QWebView* webView = new QWebView();
    if (webView)
    {
        webView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
        QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
        webView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
        webView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);

        webView->setAttribute(Qt::WA_DeleteOnClose, true);

        webView->load(QUrl("http://www.facebook.com/sharer.php?u="+link));

        showOptions();
        webView->show();
    }
    */
}


void PreferencesWidget::shareOnTweeter(const QString& link)
{
    QDesktopServices::openUrl(QUrl("http://twitter.com/share?url=" + link));
}

void PreferencesWidget::onFileSizeLimit(const QString& fileName)
{
    QString message = tr("Error upload file %1.\nFile size limit %2 exceeded.")
            .arg(fileName)
            .arg(sizeToHumanReadableString(m_bxnet->maxUploadFileSizeLimit()));
    notify(tr("Error"), message, SystemTrayIcon_::Critical);
    QMessageBox::warning(this, fullAppName() + tr(" Error"), message);
}

void PreferencesWidget::onCustomContextMenuRequested(const QPoint& pos)
{
    contextMenu()->popup(mapToGlobal(pos));
}

void PreferencesWidget::onUploadProgress(int progress, qint64 uploaded, qint64 total)
{
    Q_UNUSED(uploaded);
    Q_UNUSED(total);

    if (progress == -1)
    {
        m_uploadPercent = progress;
        m_systemTrayIcon->setToolTip("");
    }
    else
    {
        m_uploadPercent = progress;

        if (m_systemTrayIcon)
        {
            QString tooltip = tr("Uploading %1 [%2\%]").arg(QFileInfo(m_bxnet->currentUpload()).fileName()).arg(progress);
            m_systemTrayIcon->setToolTip(tooltip);
        }
    }
}

bool PreferencesWidget::isShortcutsEditing()
{
    return isVisible() && currentIndex() == 2 && m_disableShortcutsForEditing;
}

void PreferencesWidget::onQuit()
{
    if (m_forceQuit)
    {
        qDebug() << "recieved force quit signal, finishing";
        qApp->quit();
        return;
    }

    if (m_recordingFrame && m_recordingFrame->isVisible())
    {
        m_recordingFrame->close();
    }

    if (m_editor && m_editor->isVisible())
    {
        m_editor->close();
        if (m_editor->isVisible())
        {
            return;
        }
    }

    hideApplication(true);

    if (m_bxnet && (m_bxnet->uploadingQueueLength() > 0 || !m_bxnet->currentUpload().isEmpty()))
    {
        //if (m_settings.automaticUpload)
        {

            QString message = tr("There some items in uploading queue. Force quit or wait for all uploads?");

            QMessageBox messageBox(this);

            messageBox.setWindowTitle(fullAppName());
            messageBox.setText(tr("About to Quit"));
            messageBox.setInformativeText(message);

            messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Retry);

            messageBox.setDefaultButton(QMessageBox::Cancel);

            messageBox.setIconPixmap(QIcon(":/res/icon.ico").pixmap(64));
            messageBox.setButtonText(QMessageBox::Retry, tr("Wait and Quit"));
            messageBox.setButtonText(QMessageBox::Ok, tr("Force Quit"));
            messageBox.setButtonText(QMessageBox::Cancel, tr("Continue working"));

            centrateWidget(&messageBox);

            switch (messageBox.exec())
            {
            case QMessageBox::Retry:
                m_wantToExit = true;
                onQueueEmpty();
                break;
            case QMessageBox::Cancel:
                if (m_bxnet && !m_bxnet->authentificated())
                {
                    showOptions();
                }
                break;
            case QMessageBox::Ok:
                qApp->quit();
                break;
            }

        }
    }
    else
    {
        qApp->quit();
    }

}

void PreferencesWidget::onQueueEmpty()
{
    if (m_wantToExit && m_bxnet && m_bxnet->uploadingQueueLength() == 0 && m_bxnet->currentUpload().isEmpty())
    {
        onQuit();
    }
}

void PreferencesWidget::onDisconnected()
{
    setNormalIcon();
    notify(tr("Network error"), tr("Disconnected from network"), SystemTrayIcon_::Critical);
}

void PreferencesWidget::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
    if (event->key() == Qt::Key_Enter)
    {
        ui->okpushButton->click();
    }
}

//// default folder support

void PreferencesWidget::updateFoldersList(const QString folderName, const QString folderId)
{   
    Q_UNUSED(folderId);
    Q_UNUSED(folderName);

    if (m_folders.isEmpty())
    {
        if (!m_bxnet->uploadFolderId().isEmpty())
        {
            QString folder = m_bxnet->uploadFolder();
            if (folder.isEmpty())
            {
                folder = QDir::separator();
            }
            m_folders[m_bxnet->uploadFolderId()] = folder;
        }
        ui->foldersComboBox->clear();
        ui->foldersComboBox->addItems(m_folders.values());
        ui->foldersComboBox->setCurrentIndex(0);
    }
    m_bxnet->readTree(0, true, true);
    //disconnect(m_bxnet, SIGNAL(folderCreated(QString, QString)), this, SLOT(updateFoldersList(QString, QString)));
    //connect(m_bxnet, SIGNAL(folderCreated(QString, QString)), this, SLOT(addFolderToList(QString, QString)));
}

void PreferencesWidget::treeFile(QString id, QString parentId, QString name, QString link)
{

}

void PreferencesWidget::treeFolder(QString id, QString parentId, QString name, QString link)
{
    if (!m_folders.contains(id))
    {
        if (!parentId.isEmpty() && m_folders.contains(parentId))
        {
            if (parentId == "0")
            {
                m_folders[id] = name;
            }
            else
            {
                m_folders[id] = m_folders[parentId] + QDir::separator() + name;
            }
        }
        else
        {
            m_folders[id] = QDir::separator();
        }
    }
}

void PreferencesWidget::treeFinished()
{
    QStringList folders = m_folders.values();
    folders.removeAll("\\");
    folders.removeAll(DEFAULT_UPLOAD_FOLDER);
    folders.sort();
    folders.insert(0, DEFAULT_UPLOAD_FOLDER);
    folders.insert(0, "\\");
    m_blockFolderModifications = true;
    ui->foldersComboBox->clear();
    ui->foldersComboBox->addItems(folders);
    ui->foldersComboBox->setCurrentIndex(folders.indexOf(m_bxnet->uploadFolder()));
    m_blockFolderModifications = false;
    ui->foldersComboBox->setEnabled(true);
}

void PreferencesWidget::folderTextChanged(QString folderName)
{
    if (m_blockFolderModifications || !ui->foldersComboBox->isEnabled())
    {
        return;
    }

    if (m_folders.values().contains(folderName))
    {
        m_bxnet->setUploadFolder(folderName, m_folders.keys().at(m_folders.values().indexOf(folderName)));
    }
    else
    {

        m_bxnet->setUploadFolder(folderName);
    }
}

void PreferencesWidget::folderTextFinished()
{
    if (m_blockFolderModifications)
    {
        return;
    }

    const QString folderName = ui->foldersComboBox->currentText();
    if (!folderName.isEmpty())
    {
        folderTextChanged(folderName);
    }
}

void PreferencesWidget::messageClicked()
{
    //qDebug() << Q_FUNC_INFO;
    //runEditor(m_);
}

void PreferencesWidget::currentTabChanged(int index)
{
    if (index == 2)
    {
        // remove "enter shortcut..." text in focused element:
        ui->defaultsShortcutsPushButton->setFocus();
    }
}
