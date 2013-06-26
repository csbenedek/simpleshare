/********************************************************************************
** Form generated from reading UI file 'preferenceswidget.ui'
**
** Created: Mon Jun 24 20:09:05 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCESWIDGET_H
#define UI_PREFERENCESWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <keysequencewidget.h>

QT_BEGIN_NAMESPACE

class Ui_PreferencesWidget
{
public:
    QAction *actionQuit;
    QAction *actionPreferences;
    QAction *actionFullScreen;
    QAction *actionRegion;
    QAction *actionWindow;
    QAction *actionUpload;
    QAction *actionUploads;
    QAction *actionVideo;
    QWidget *accountPage;
    QGridLayout *gridLayout_5;
    QWidget *loggedWidget;
    QGridLayout *gridLayout_3;
    QLabel *lbUserIcon;
    QVBoxLayout *verticalLayout_2;
    QLabel *lbUsername;
    QLabel *lbUsedInfo;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *logoutPushButton;
    QWidget *loginWidget_2;
    QVBoxLayout *verticalLayout_4;
    QWidget *loginWidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout_8;
    QSpacerItem *horizontalSpacer;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer_6;
    QLabel *descriptionLabel;
    QLabel *userEmailLabel;
    QLineEdit *userEmailLineEdit;
    QPushButton *loginPushButton;
    QSpacerItem *verticalSpacer_3;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QSpacerItem *horizontalSpacer_8;
    QSpacerItem *verticalSpacer_7;
    QSpacerItem *verticalSpacer_8;
    QSpacerItem *verticalSpacer_10;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *verticalSpacer_9;
    QWidget *instructionWidget;
    QGridLayout *gridLayout_13;
    QGridLayout *gridLayout_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *versionLabel;
    QSpacerItem *horizontalSpacer_7;
    QWidget *viewFolderWidget;
    QGridLayout *gridLayout_4;
    QLabel *label_6;
    QGridLayout *gridLayout_10;
    QComboBox *foldersComboBox;
    QPushButton *viewFolder_pushButton;
    QSpacerItem *horizontalSpacer_4;
    QLabel *lbFolderInfo;
    QWidget *registerWidget;
    QGridLayout *gridLayout_6;
    QTextBrowser *textBrowser;
    QSpacerItem *horizontalSpacer_12;
    QWidget *preferencesPage;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayoutPreferences;
    QLabel *uploadingLabel;
    QCheckBox *deleteAfterUploadCheckBox;
    QCheckBox *deleteOtherCheckBox;
    QCheckBox *startAtLoginCheckBox;
    QLabel *launchLabel;
    QGridLayout *gridLayout;
    QPushButton *cancelpushButton;
    QPushButton *okpushButton;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer_10;
    QCheckBox *disableAutoUploadCheckBox;
    QCheckBox *editorCheckBox;
    QCheckBox *copyUrlCheckBox;
    QLabel *linksLabel;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_4;
    QLabel *urlShortenerLabel;
    QCheckBox *urlShortenerCheckBox;
    QLabel *label;
    QCheckBox *muteCheckBox;
    QSpacerItem *verticalSpacer_6;
    QWidget *shortcutsPage;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_9;
    QLabel *label_12;
    QLabel *fullScreenLabel_2;
    KeySequenceWidget *fullScreenKeyWidget;
    QLabel *regionKwyLabel_2;
    KeySequenceWidget *regionKeyWidget;
    QSpacerItem *verticalSpacer_5;
    QLabel *label_14;
    QLabel *screencastLabel_3;
    QLabel *screencastKeyLabel_2;
    KeySequenceWidget *screencastKeyWidget;
    KeySequenceWidget *annotateKeyWidget;
    QLabel *label_11;
    QLabel *regionKwyLabel;
    KeySequenceWidget *uploadKeyWidget;
    QLabel *uploadLabel_2;
    QLabel *uploadIcon_2;
    QLabel *windowLabel_2;
    KeySequenceWidget *windowKeyWidget;
    QLabel *windowIcon_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_5;
    QToolButton *defaultsShortcutsPushButton;
    QLabel *label_2;

    void setupUi(QTabWidget *PreferencesWidget)
    {
        if (PreferencesWidget->objectName().isEmpty())
            PreferencesWidget->setObjectName(QString::fromUtf8("PreferencesWidget"));
        PreferencesWidget->setEnabled(true);
        PreferencesWidget->resize(600, 471);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PreferencesWidget->sizePolicy().hasHeightForWidth());
        PreferencesWidget->setSizePolicy(sizePolicy);
        PreferencesWidget->setMaximumSize(QSize(600, 630));
        PreferencesWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/loading_1.png"), QSize(), QIcon::Normal, QIcon::Off);
        PreferencesWidget->setWindowIcon(icon);
        PreferencesWidget->setStyleSheet(QString::fromUtf8(""));
        PreferencesWidget->setTabPosition(QTabWidget::North);
        PreferencesWidget->setTabShape(QTabWidget::Rounded);
        PreferencesWidget->setIconSize(QSize(32, 32));
        PreferencesWidget->setElideMode(Qt::ElideNone);
        PreferencesWidget->setDocumentMode(true);
        actionQuit = new QAction(PreferencesWidget);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/res/icon_quit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionQuit->setIcon(icon1);
        actionQuit->setMenuRole(QAction::TextHeuristicRole);
        actionPreferences = new QAction(PreferencesWidget);
        actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/res/icon_settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPreferences->setIcon(icon2);
        actionFullScreen = new QAction(PreferencesWidget);
        actionFullScreen->setObjectName(QString::fromUtf8("actionFullScreen"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/res/icon_capturedesktop.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFullScreen->setIcon(icon3);
        actionRegion = new QAction(PreferencesWidget);
        actionRegion->setObjectName(QString::fromUtf8("actionRegion"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/res/icon_captureregion.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionRegion->setIcon(icon4);
        actionWindow = new QAction(PreferencesWidget);
        actionWindow->setObjectName(QString::fromUtf8("actionWindow"));
        actionUpload = new QAction(PreferencesWidget);
        actionUpload->setObjectName(QString::fromUtf8("actionUpload"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/res/icon_upload.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUpload->setIcon(icon5);
        actionUploads = new QAction(PreferencesWidget);
        actionUploads->setObjectName(QString::fromUtf8("actionUploads"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/res/icon_disable-upload.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon6.addFile(QString::fromUtf8(":/res/icon_disable-upload.png"), QSize(), QIcon::Normal, QIcon::On);
        actionUploads->setIcon(icon6);
        actionVideo = new QAction(PreferencesWidget);
        actionVideo->setObjectName(QString::fromUtf8("actionVideo"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/res/icon_screencast.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionVideo->setIcon(icon7);
        accountPage = new QWidget();
        accountPage->setObjectName(QString::fromUtf8("accountPage"));
        accountPage->setAutoFillBackground(false);
        accountPage->setStyleSheet(QString::fromUtf8(""));
        gridLayout_5 = new QGridLayout(accountPage);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(9, -1, 9, -1);
        loggedWidget = new QWidget(accountPage);
        loggedWidget->setObjectName(QString::fromUtf8("loggedWidget"));
        gridLayout_3 = new QGridLayout(loggedWidget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(10, -1, 9, -1);
        lbUserIcon = new QLabel(loggedWidget);
        lbUserIcon->setObjectName(QString::fromUtf8("lbUserIcon"));
        lbUserIcon->setMinimumSize(QSize(40, 40));
        lbUserIcon->setMaximumSize(QSize(40, 40));
        lbUserIcon->setPixmap(QPixmap(QString::fromUtf8(":/res/avatar.png")));
        lbUserIcon->setScaledContents(true);

        gridLayout_3->addWidget(lbUserIcon, 0, 0, 1, 1);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        lbUsername = new QLabel(loggedWidget);
        lbUsername->setObjectName(QString::fromUtf8("lbUsername"));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        lbUsername->setFont(font);

        verticalLayout_2->addWidget(lbUsername);

        lbUsedInfo = new QLabel(loggedWidget);
        lbUsedInfo->setObjectName(QString::fromUtf8("lbUsedInfo"));
        lbUsedInfo->setEnabled(false);

        verticalLayout_2->addWidget(lbUsedInfo);


        gridLayout_3->addLayout(verticalLayout_2, 0, 1, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(230, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_3, 0, 2, 1, 1);

        logoutPushButton = new QPushButton(loggedWidget);
        logoutPushButton->setObjectName(QString::fromUtf8("logoutPushButton"));
        logoutPushButton->setMaximumSize(QSize(16777215, 16777215));
        logoutPushButton->setStyleSheet(QString::fromUtf8(""));

        gridLayout_3->addWidget(logoutPushButton, 0, 3, 1, 1);


        gridLayout_5->addWidget(loggedWidget, 2, 0, 1, 1);

        loginWidget_2 = new QWidget(accountPage);
        loginWidget_2->setObjectName(QString::fromUtf8("loginWidget_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(loginWidget_2->sizePolicy().hasHeightForWidth());
        loginWidget_2->setSizePolicy(sizePolicy1);
        loginWidget_2->setMinimumSize(QSize(0, 150));
        loginWidget_2->setMaximumSize(QSize(16777215, 150));
        verticalLayout_4 = new QVBoxLayout(loginWidget_2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        loginWidget = new QWidget(loginWidget_2);
        loginWidget->setObjectName(QString::fromUtf8("loginWidget"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(loginWidget->sizePolicy().hasHeightForWidth());
        loginWidget->setSizePolicy(sizePolicy2);
        loginWidget->setMinimumSize(QSize(511, 140));
        loginWidget->setMaximumSize(QSize(511, 140));
        loginWidget->setStyleSheet(QString::fromUtf8("#loginWidget { background-image: url(:/res/boxNetLoginArrow.png); }"));
        gridLayout_2 = new QGridLayout(loginWidget);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_8 = new QGridLayout();
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        horizontalSpacer = new QSpacerItem(140, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_8->addItem(horizontalSpacer, 1, 0, 6, 1);

        label_5 = new QLabel(loginWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setFont(font);

        gridLayout_8->addWidget(label_5, 1, 1, 1, 3);

        horizontalSpacer_6 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_6, 8, 5, 1, 1);

        descriptionLabel = new QLabel(loginWidget);
        descriptionLabel->setObjectName(QString::fromUtf8("descriptionLabel"));
        sizePolicy1.setHeightForWidth(descriptionLabel->sizePolicy().hasHeightForWidth());
        descriptionLabel->setSizePolicy(sizePolicy1);
        descriptionLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        descriptionLabel->setWordWrap(true);

        gridLayout_8->addWidget(descriptionLabel, 3, 1, 1, 4);

        userEmailLabel = new QLabel(loginWidget);
        userEmailLabel->setObjectName(QString::fromUtf8("userEmailLabel"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(userEmailLabel->sizePolicy().hasHeightForWidth());
        userEmailLabel->setSizePolicy(sizePolicy3);

        gridLayout_8->addWidget(userEmailLabel, 4, 1, 1, 1);

        userEmailLineEdit = new QLineEdit(loginWidget);
        userEmailLineEdit->setObjectName(QString::fromUtf8("userEmailLineEdit"));
        userEmailLineEdit->setEnabled(false);
        userEmailLineEdit->setMaxLength(100);

        gridLayout_8->addWidget(userEmailLineEdit, 4, 3, 1, 2);

        loginPushButton = new QPushButton(loginWidget);
        loginPushButton->setObjectName(QString::fromUtf8("loginPushButton"));
        loginPushButton->setEnabled(false);
        loginPushButton->setMaximumSize(QSize(16777215, 16777215));
        loginPushButton->setFlat(false);

        gridLayout_8->addWidget(loginPushButton, 8, 4, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_8->addItem(verticalSpacer_3, 9, 4, 1, 1);

        passwordLabel = new QLabel(loginWidget);
        passwordLabel->setObjectName(QString::fromUtf8("passwordLabel"));
        sizePolicy3.setHeightForWidth(passwordLabel->sizePolicy().hasHeightForWidth());
        passwordLabel->setSizePolicy(sizePolicy3);

        gridLayout_8->addWidget(passwordLabel, 6, 1, 1, 1);

        passwordLineEdit = new QLineEdit(loginWidget);
        passwordLineEdit->setObjectName(QString::fromUtf8("passwordLineEdit"));
        passwordLineEdit->setEnabled(false);
        passwordLineEdit->setMaxLength(100);
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        gridLayout_8->addWidget(passwordLineEdit, 6, 3, 1, 2);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_8, 8, 3, 1, 1);

        verticalSpacer_7 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer_7, 7, 3, 1, 1);

        verticalSpacer_8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer_8, 0, 1, 1, 1);

        verticalSpacer_10 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer_10, 5, 3, 1, 1);

        horizontalSpacer_9 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_9, 4, 2, 1, 1);

        verticalSpacer_9 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer_9, 2, 1, 1, 1);


        gridLayout_2->addLayout(gridLayout_8, 1, 2, 1, 1);


        verticalLayout_4->addWidget(loginWidget);


        gridLayout_5->addWidget(loginWidget_2, 0, 0, 1, 1);

        instructionWidget = new QWidget(accountPage);
        instructionWidget->setObjectName(QString::fromUtf8("instructionWidget"));
        sizePolicy1.setHeightForWidth(instructionWidget->sizePolicy().hasHeightForWidth());
        instructionWidget->setSizePolicy(sizePolicy1);
        gridLayout_13 = new QGridLayout(instructionWidget);
        gridLayout_13->setContentsMargins(0, 0, 0, 0);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        gridLayout_14 = new QGridLayout();
        gridLayout_14->setObjectName(QString::fromUtf8("gridLayout_14"));
        label_15 = new QLabel(instructionWidget);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setPixmap(QPixmap(QString::fromUtf8(":/res/image_howto_step1.png")));

        gridLayout_14->addWidget(label_15, 0, 0, 1, 1);

        label_16 = new QLabel(instructionWidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setWordWrap(false);

        gridLayout_14->addWidget(label_16, 0, 1, 1, 1);

        label_17 = new QLabel(instructionWidget);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setPixmap(QPixmap(QString::fromUtf8(":/res/image_howto_step2.png")));

        gridLayout_14->addWidget(label_17, 0, 3, 1, 1);

        label_18 = new QLabel(instructionWidget);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setWordWrap(true);

        gridLayout_14->addWidget(label_18, 0, 4, 1, 1);

        versionLabel = new QLabel(instructionWidget);
        versionLabel->setObjectName(QString::fromUtf8("versionLabel"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        versionLabel->setFont(font1);
        versionLabel->setAlignment(Qt::AlignBottom|Qt::AlignRight|Qt::AlignTrailing);

        gridLayout_14->addWidget(versionLabel, 0, 5, 1, 1);

        horizontalSpacer_7 = new QSpacerItem(6, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_14->addItem(horizontalSpacer_7, 0, 2, 1, 1);


        gridLayout_13->addLayout(gridLayout_14, 0, 0, 1, 1);


        gridLayout_5->addWidget(instructionWidget, 6, 0, 1, 1);

        viewFolderWidget = new QWidget(accountPage);
        viewFolderWidget->setObjectName(QString::fromUtf8("viewFolderWidget"));
        gridLayout_4 = new QGridLayout(viewFolderWidget);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(9, -1, 9, -1);
        label_6 = new QLabel(viewFolderWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(40);
        sizePolicy4.setVerticalStretch(40);
        sizePolicy4.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy4);
        label_6->setMinimumSize(QSize(40, 40));
        label_6->setMaximumSize(QSize(40, 40));
        label_6->setAutoFillBackground(false);
        label_6->setPixmap(QPixmap(QString::fromUtf8(":/res/icon_folder.PNG")));
        label_6->setScaledContents(true);

        gridLayout_4->addWidget(label_6, 0, 0, 1, 1);

        gridLayout_10 = new QGridLayout();
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        foldersComboBox = new QComboBox(viewFolderWidget);
        foldersComboBox->setObjectName(QString::fromUtf8("foldersComboBox"));
        foldersComboBox->setEnabled(false);
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(foldersComboBox->sizePolicy().hasHeightForWidth());
        foldersComboBox->setSizePolicy(sizePolicy5);
        foldersComboBox->setFont(font);
        foldersComboBox->setStyleSheet(QString::fromUtf8("font: bold 10pt \"MS Shell Dlg 2\";"));
        foldersComboBox->setEditable(true);
        foldersComboBox->setFrame(true);

        gridLayout_10->addWidget(foldersComboBox, 1, 0, 1, 1);

        viewFolder_pushButton = new QPushButton(viewFolderWidget);
        viewFolder_pushButton->setObjectName(QString::fromUtf8("viewFolder_pushButton"));
        viewFolder_pushButton->setMaximumSize(QSize(16777215, 16777215));

        gridLayout_10->addWidget(viewFolder_pushButton, 1, 2, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_10->addItem(horizontalSpacer_4, 1, 1, 1, 1);

        lbFolderInfo = new QLabel(viewFolderWidget);
        lbFolderInfo->setObjectName(QString::fromUtf8("lbFolderInfo"));
        lbFolderInfo->setEnabled(false);

        gridLayout_10->addWidget(lbFolderInfo, 0, 0, 1, 1);


        gridLayout_4->addLayout(gridLayout_10, 0, 1, 1, 1);


        gridLayout_5->addWidget(viewFolderWidget, 3, 0, 1, 1);

        registerWidget = new QWidget(accountPage);
        registerWidget->setObjectName(QString::fromUtf8("registerWidget"));
        registerWidget->setMinimumSize(QSize(0, 0));
        registerWidget->setMaximumSize(QSize(1000, 50));
        registerWidget->setStyleSheet(QString::fromUtf8(""));
        gridLayout_6 = new QGridLayout(registerWidget);
        gridLayout_6->setSpacing(0);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setContentsMargins(9, 0, 0, 0);
        textBrowser = new QTextBrowser(registerWidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setEnabled(true);
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(textBrowser->sizePolicy().hasHeightForWidth());
        textBrowser->setSizePolicy(sizePolicy6);
        textBrowser->setMinimumSize(QSize(511, 0));
        textBrowser->setMaximumSize(QSize(511, 16777215));
        textBrowser->setAcceptDrops(false);
        textBrowser->setStyleSheet(QString::fromUtf8("background-color: rgb(235, 235, 235);"));
        textBrowser->setFrameShape(QFrame::NoFrame);
        textBrowser->setFrameShadow(QFrame::Plain);
        textBrowser->setLineWidth(0);
        textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textBrowser->setReadOnly(true);
        textBrowser->setAcceptRichText(true);
        textBrowser->setCursorWidth(0);
        textBrowser->setOpenExternalLinks(true);
        textBrowser->setOpenLinks(true);

        gridLayout_6->addWidget(textBrowser, 0, 0, 1, 1);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(horizontalSpacer_12, 0, 1, 1, 1);


        gridLayout_5->addWidget(registerWidget, 5, 0, 1, 1);

        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/res/icon_account.png"), QSize(), QIcon::Normal, QIcon::Off);
        PreferencesWidget->addTab(accountPage, icon8, QString());
        preferencesPage = new QWidget();
        preferencesPage->setObjectName(QString::fromUtf8("preferencesPage"));
        gridLayout_7 = new QGridLayout(preferencesPage);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayoutPreferences = new QGridLayout();
        gridLayoutPreferences->setObjectName(QString::fromUtf8("gridLayoutPreferences"));
        gridLayoutPreferences->setContentsMargins(20, 0, 3, -1);
        uploadingLabel = new QLabel(preferencesPage);
        uploadingLabel->setObjectName(QString::fromUtf8("uploadingLabel"));
        uploadingLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutPreferences->addWidget(uploadingLabel, 1, 0, 1, 1);

        deleteAfterUploadCheckBox = new QCheckBox(preferencesPage);
        deleteAfterUploadCheckBox->setObjectName(QString::fromUtf8("deleteAfterUploadCheckBox"));
        deleteAfterUploadCheckBox->setEnabled(true);
        deleteAfterUploadCheckBox->setChecked(false);

        gridLayoutPreferences->addWidget(deleteAfterUploadCheckBox, 1, 1, 1, 1);

        deleteOtherCheckBox = new QCheckBox(preferencesPage);
        deleteOtherCheckBox->setObjectName(QString::fromUtf8("deleteOtherCheckBox"));
        deleteOtherCheckBox->setChecked(false);

        gridLayoutPreferences->addWidget(deleteOtherCheckBox, 2, 1, 1, 1);

        startAtLoginCheckBox = new QCheckBox(preferencesPage);
        startAtLoginCheckBox->setObjectName(QString::fromUtf8("startAtLoginCheckBox"));
        startAtLoginCheckBox->setChecked(true);

        gridLayoutPreferences->addWidget(startAtLoginCheckBox, 6, 1, 1, 1);

        launchLabel = new QLabel(preferencesPage);
        launchLabel->setObjectName(QString::fromUtf8("launchLabel"));
        launchLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutPreferences->addWidget(launchLabel, 6, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setContentsMargins(6, 6, 6, 6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        cancelpushButton = new QPushButton(preferencesPage);
        cancelpushButton->setObjectName(QString::fromUtf8("cancelpushButton"));
        cancelpushButton->setMaximumSize(QSize(92, 24));

        gridLayout->addWidget(cancelpushButton, 1, 1, 1, 1);

        okpushButton = new QPushButton(preferencesPage);
        okpushButton->setObjectName(QString::fromUtf8("okpushButton"));
        okpushButton->setMaximumSize(QSize(92, 24));
        okpushButton->setAutoDefault(false);
        okpushButton->setDefault(true);

        gridLayout->addWidget(okpushButton, 0, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 2, 1, 1, 1);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_10, 0, 0, 1, 1);


        gridLayoutPreferences->addLayout(gridLayout, 1, 3, 3, 2);

        disableAutoUploadCheckBox = new QCheckBox(preferencesPage);
        disableAutoUploadCheckBox->setObjectName(QString::fromUtf8("disableAutoUploadCheckBox"));
        disableAutoUploadCheckBox->setChecked(false);

        gridLayoutPreferences->addWidget(disableAutoUploadCheckBox, 3, 1, 1, 1);

        editorCheckBox = new QCheckBox(preferencesPage);
        editorCheckBox->setObjectName(QString::fromUtf8("editorCheckBox"));
        editorCheckBox->setEnabled(true);
        editorCheckBox->setChecked(false);

        gridLayoutPreferences->addWidget(editorCheckBox, 4, 1, 1, 1);

        copyUrlCheckBox = new QCheckBox(preferencesPage);
        copyUrlCheckBox->setObjectName(QString::fromUtf8("copyUrlCheckBox"));
        copyUrlCheckBox->setChecked(true);

        gridLayoutPreferences->addWidget(copyUrlCheckBox, 5, 1, 1, 1);

        linksLabel = new QLabel(preferencesPage);
        linksLabel->setObjectName(QString::fromUtf8("linksLabel"));
        linksLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutPreferences->addWidget(linksLabel, 5, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayoutPreferences->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayoutPreferences->addItem(verticalSpacer_4, 9, 1, 1, 1);

        urlShortenerLabel = new QLabel(preferencesPage);
        urlShortenerLabel->setObjectName(QString::fromUtf8("urlShortenerLabel"));

        gridLayoutPreferences->addWidget(urlShortenerLabel, 8, 0, 1, 1);

        urlShortenerCheckBox = new QCheckBox(preferencesPage);
        urlShortenerCheckBox->setObjectName(QString::fromUtf8("urlShortenerCheckBox"));
        urlShortenerCheckBox->setChecked(true);

        gridLayoutPreferences->addWidget(urlShortenerCheckBox, 8, 1, 1, 1);

        label = new QLabel(preferencesPage);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayoutPreferences->addWidget(label, 7, 0, 1, 1);

        muteCheckBox = new QCheckBox(preferencesPage);
        muteCheckBox->setObjectName(QString::fromUtf8("muteCheckBox"));

        gridLayoutPreferences->addWidget(muteCheckBox, 7, 1, 1, 1);

        verticalSpacer_6 = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayoutPreferences->addItem(verticalSpacer_6, 0, 1, 1, 1);


        gridLayout_7->addLayout(gridLayoutPreferences, 2, 0, 1, 1);

        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/res/icon_preferences.png"), QSize(), QIcon::Normal, QIcon::Off);
        PreferencesWidget->addTab(preferencesPage, icon9, QString());
        shortcutsPage = new QWidget();
        shortcutsPage->setObjectName(QString::fromUtf8("shortcutsPage"));
        verticalLayout = new QVBoxLayout(shortcutsPage);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout_9 = new QGridLayout();
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_9->setContentsMargins(-1, 12, -1, 12);
        label_12 = new QLabel(shortcutsPage);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setPixmap(QPixmap(QString::fromUtf8(":/res/image_hotkey_drawregion.png")));

        gridLayout_9->addWidget(label_12, 0, 2, 2, 1);

        fullScreenLabel_2 = new QLabel(shortcutsPage);
        fullScreenLabel_2->setObjectName(QString::fromUtf8("fullScreenLabel_2"));
        QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(fullScreenLabel_2->sizePolicy().hasHeightForWidth());
        fullScreenLabel_2->setSizePolicy(sizePolicy7);
        fullScreenLabel_2->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        fullScreenLabel_2->setMargin(2);

        gridLayout_9->addWidget(fullScreenLabel_2, 0, 1, 1, 1);

        fullScreenKeyWidget = new KeySequenceWidget(shortcutsPage);
        fullScreenKeyWidget->setObjectName(QString::fromUtf8("fullScreenKeyWidget"));
        fullScreenKeyWidget->setMinimumSize(QSize(70, 0));
        fullScreenKeyWidget->setFocusPolicy(Qt::StrongFocus);

        gridLayout_9->addWidget(fullScreenKeyWidget, 1, 1, 1, 1);

        regionKwyLabel_2 = new QLabel(shortcutsPage);
        regionKwyLabel_2->setObjectName(QString::fromUtf8("regionKwyLabel_2"));
        sizePolicy7.setHeightForWidth(regionKwyLabel_2->sizePolicy().hasHeightForWidth());
        regionKwyLabel_2->setSizePolicy(sizePolicy7);
        regionKwyLabel_2->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        regionKwyLabel_2->setMargin(2);

        gridLayout_9->addWidget(regionKwyLabel_2, 0, 3, 1, 1);

        regionKeyWidget = new KeySequenceWidget(shortcutsPage);
        regionKeyWidget->setObjectName(QString::fromUtf8("regionKeyWidget"));
        regionKeyWidget->setMinimumSize(QSize(70, 0));
        regionKeyWidget->setFocusPolicy(Qt::StrongFocus);

        gridLayout_9->addWidget(regionKeyWidget, 1, 3, 1, 1);

        verticalSpacer_5 = new QSpacerItem(0, 6, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_9->addItem(verticalSpacer_5, 2, 0, 1, 1);

        label_14 = new QLabel(shortcutsPage);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setPixmap(QPixmap(QString::fromUtf8(":/res/image_hotkey_fullscreen.png")));

        gridLayout_9->addWidget(label_14, 0, 0, 2, 1);

        screencastLabel_3 = new QLabel(shortcutsPage);
        screencastLabel_3->setObjectName(QString::fromUtf8("screencastLabel_3"));
        screencastLabel_3->setPixmap(QPixmap(QString::fromUtf8(":/res/image_hotkey_screencast.png")));

        gridLayout_9->addWidget(screencastLabel_3, 3, 4, 2, 1);

        screencastKeyLabel_2 = new QLabel(shortcutsPage);
        screencastKeyLabel_2->setObjectName(QString::fromUtf8("screencastKeyLabel_2"));
        sizePolicy7.setHeightForWidth(screencastKeyLabel_2->sizePolicy().hasHeightForWidth());
        screencastKeyLabel_2->setSizePolicy(sizePolicy7);
        screencastKeyLabel_2->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        screencastKeyLabel_2->setMargin(2);

        gridLayout_9->addWidget(screencastKeyLabel_2, 3, 5, 1, 1);

        screencastKeyWidget = new KeySequenceWidget(shortcutsPage);
        screencastKeyWidget->setObjectName(QString::fromUtf8("screencastKeyWidget"));
        screencastKeyWidget->setMinimumSize(QSize(70, 0));
        screencastKeyWidget->setFocusPolicy(Qt::StrongFocus);

        gridLayout_9->addWidget(screencastKeyWidget, 4, 5, 1, 1);

        annotateKeyWidget = new KeySequenceWidget(shortcutsPage);
        annotateKeyWidget->setObjectName(QString::fromUtf8("annotateKeyWidget"));
        annotateKeyWidget->setMinimumSize(QSize(70, 0));
        annotateKeyWidget->setFocusPolicy(Qt::StrongFocus);

        gridLayout_9->addWidget(annotateKeyWidget, 1, 5, 1, 1);

        label_11 = new QLabel(shortcutsPage);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setPixmap(QPixmap(QString::fromUtf8(":/res/image_hotkey_drawregion.png")));

        gridLayout_9->addWidget(label_11, 0, 4, 2, 1);

        regionKwyLabel = new QLabel(shortcutsPage);
        regionKwyLabel->setObjectName(QString::fromUtf8("regionKwyLabel"));
        sizePolicy7.setHeightForWidth(regionKwyLabel->sizePolicy().hasHeightForWidth());
        regionKwyLabel->setSizePolicy(sizePolicy7);
        regionKwyLabel->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        regionKwyLabel->setMargin(2);

        gridLayout_9->addWidget(regionKwyLabel, 0, 5, 1, 1);

        uploadKeyWidget = new KeySequenceWidget(shortcutsPage);
        uploadKeyWidget->setObjectName(QString::fromUtf8("uploadKeyWidget"));
        uploadKeyWidget->setMinimumSize(QSize(70, 0));
        uploadKeyWidget->setFocusPolicy(Qt::StrongFocus);

        gridLayout_9->addWidget(uploadKeyWidget, 4, 3, 1, 1);

        uploadLabel_2 = new QLabel(shortcutsPage);
        uploadLabel_2->setObjectName(QString::fromUtf8("uploadLabel_2"));
        sizePolicy7.setHeightForWidth(uploadLabel_2->sizePolicy().hasHeightForWidth());
        uploadLabel_2->setSizePolicy(sizePolicy7);
        uploadLabel_2->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        uploadLabel_2->setMargin(2);

        gridLayout_9->addWidget(uploadLabel_2, 3, 3, 1, 1);

        uploadIcon_2 = new QLabel(shortcutsPage);
        uploadIcon_2->setObjectName(QString::fromUtf8("uploadIcon_2"));
        uploadIcon_2->setPixmap(QPixmap(QString::fromUtf8(":/res/image_hotkey_fullscreen.png")));

        gridLayout_9->addWidget(uploadIcon_2, 3, 2, 2, 1);

        windowLabel_2 = new QLabel(shortcutsPage);
        windowLabel_2->setObjectName(QString::fromUtf8("windowLabel_2"));
        sizePolicy7.setHeightForWidth(windowLabel_2->sizePolicy().hasHeightForWidth());
        windowLabel_2->setSizePolicy(sizePolicy7);
        windowLabel_2->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        windowLabel_2->setMargin(2);

        gridLayout_9->addWidget(windowLabel_2, 3, 1, 1, 1);

        windowKeyWidget = new KeySequenceWidget(shortcutsPage);
        windowKeyWidget->setObjectName(QString::fromUtf8("windowKeyWidget"));
        windowKeyWidget->setMinimumSize(QSize(70, 0));
        windowKeyWidget->setFocusPolicy(Qt::StrongFocus);

        gridLayout_9->addWidget(windowKeyWidget, 4, 1, 1, 1);

        windowIcon_2 = new QLabel(shortcutsPage);
        windowIcon_2->setObjectName(QString::fromUtf8("windowIcon_2"));
        windowIcon_2->setPixmap(QPixmap(QString::fromUtf8(":/res/image_hotkey_drawregion.png")));

        gridLayout_9->addWidget(windowIcon_2, 3, 0, 2, 1);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_9->addItem(verticalSpacer, 5, 5, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);

        defaultsShortcutsPushButton = new QToolButton(shortcutsPage);
        defaultsShortcutsPushButton->setObjectName(QString::fromUtf8("defaultsShortcutsPushButton"));
        sizePolicy2.setHeightForWidth(defaultsShortcutsPushButton->sizePolicy().hasHeightForWidth());
        defaultsShortcutsPushButton->setSizePolicy(sizePolicy2);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/res/reset.png"), QSize(), QIcon::Normal, QIcon::Off);
        defaultsShortcutsPushButton->setIcon(icon10);
        defaultsShortcutsPushButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        defaultsShortcutsPushButton->setAutoRaise(false);

        horizontalLayout->addWidget(defaultsShortcutsPushButton);


        gridLayout_9->addLayout(horizontalLayout, 6, 0, 1, 6);

        label_2 = new QLabel(shortcutsPage);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_9->addWidget(label_2, 5, 0, 1, 5);


        verticalLayout->addLayout(gridLayout_9);

        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/res/f1.png"), QSize(), QIcon::Normal, QIcon::Off);
        PreferencesWidget->addTab(shortcutsPage, icon11, QString());
        QWidget::setTabOrder(userEmailLineEdit, passwordLineEdit);
        QWidget::setTabOrder(passwordLineEdit, loginPushButton);
        QWidget::setTabOrder(loginPushButton, logoutPushButton);
        QWidget::setTabOrder(logoutPushButton, foldersComboBox);
        QWidget::setTabOrder(foldersComboBox, viewFolder_pushButton);
        QWidget::setTabOrder(viewFolder_pushButton, textBrowser);
        QWidget::setTabOrder(textBrowser, deleteAfterUploadCheckBox);
        QWidget::setTabOrder(deleteAfterUploadCheckBox, deleteOtherCheckBox);
        QWidget::setTabOrder(deleteOtherCheckBox, disableAutoUploadCheckBox);
        QWidget::setTabOrder(disableAutoUploadCheckBox, editorCheckBox);
        QWidget::setTabOrder(editorCheckBox, copyUrlCheckBox);
        QWidget::setTabOrder(copyUrlCheckBox, startAtLoginCheckBox);
        QWidget::setTabOrder(startAtLoginCheckBox, muteCheckBox);
        QWidget::setTabOrder(muteCheckBox, urlShortenerCheckBox);
        QWidget::setTabOrder(urlShortenerCheckBox, okpushButton);
        QWidget::setTabOrder(okpushButton, cancelpushButton);
        QWidget::setTabOrder(cancelpushButton, defaultsShortcutsPushButton);
        QWidget::setTabOrder(defaultsShortcutsPushButton, fullScreenKeyWidget);
        QWidget::setTabOrder(fullScreenKeyWidget, regionKeyWidget);
        QWidget::setTabOrder(regionKeyWidget, annotateKeyWidget);
        QWidget::setTabOrder(annotateKeyWidget, windowKeyWidget);
        QWidget::setTabOrder(windowKeyWidget, uploadKeyWidget);
        QWidget::setTabOrder(uploadKeyWidget, screencastKeyWidget);

        retranslateUi(PreferencesWidget);

        PreferencesWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PreferencesWidget);
    } // setupUi

    void retranslateUi(QTabWidget *PreferencesWidget)
    {
        PreferencesWidget->setWindowTitle(QApplication::translate("PreferencesWidget", "Box SimpleShare", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("PreferencesWidget", "Quit", 0, QApplication::UnicodeUTF8));
        actionPreferences->setText(QApplication::translate("PreferencesWidget", "Preferences...", 0, QApplication::UnicodeUTF8));
        actionFullScreen->setText(QApplication::translate("PreferencesWidget", "Grab full screen", 0, QApplication::UnicodeUTF8));
        actionRegion->setText(QApplication::translate("PreferencesWidget", "Draw region", 0, QApplication::UnicodeUTF8));
        actionWindow->setText(QApplication::translate("PreferencesWidget", "Grab window", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionWindow->setToolTip(QApplication::translate("PreferencesWidget", "Grab window", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionUpload->setText(QApplication::translate("PreferencesWidget", "Upload file", 0, QApplication::UnicodeUTF8));
        actionUploads->setText(QApplication::translate("PreferencesWidget", "Enable uploads", 0, QApplication::UnicodeUTF8));
        actionVideo->setText(QApplication::translate("PreferencesWidget", "Record video", 0, QApplication::UnicodeUTF8));
        lbUserIcon->setText(QString());
        lbUsername->setText(QApplication::translate("PreferencesWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        lbUsedInfo->setText(QApplication::translate("PreferencesWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        logoutPushButton->setText(QApplication::translate("PreferencesWidget", "Logout", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("PreferencesWidget", "Log in to your Box account:", 0, QApplication::UnicodeUTF8));
        descriptionLabel->setText(QApplication::translate("PreferencesWidget", "Box SimpleShare allows you to easily take screenshots and screencasts that are uploaded directly to Box.", 0, QApplication::UnicodeUTF8));
        userEmailLabel->setText(QApplication::translate("PreferencesWidget", "User e-mail:", 0, QApplication::UnicodeUTF8));
        loginPushButton->setText(QApplication::translate("PreferencesWidget", "Log In", 0, QApplication::UnicodeUTF8));
        passwordLabel->setText(QApplication::translate("PreferencesWidget", "Password:", 0, QApplication::UnicodeUTF8));
        label_15->setText(QString());
        label_16->setText(QApplication::translate("PreferencesWidget", "Drag files to system tray\n"
"or take a screenshot using Alt+Shift+4", 0, QApplication::UnicodeUTF8));
        label_17->setText(QString());
        label_18->setText(QApplication::translate("PreferencesWidget", "Check! It's done uploading and the Link is copied to your clipboard", 0, QApplication::UnicodeUTF8));
        versionLabel->setText(QApplication::translate("PreferencesWidget", "v1.0.42", 0, QApplication::UnicodeUTF8));
        label_6->setText(QString());
        foldersComboBox->clear();
        foldersComboBox->insertItems(0, QStringList()
         << QApplication::translate("PreferencesWidget", "SimpleShare Uploads", 0, QApplication::UnicodeUTF8)
        );
        viewFolder_pushButton->setText(QApplication::translate("PreferencesWidget", "View Folder", 0, QApplication::UnicodeUTF8));
        lbFolderInfo->setText(QApplication::translate("PreferencesWidget", "The folder that your files are uploaded to:", 0, QApplication::UnicodeUTF8));
        textBrowser->setHtml(QApplication::translate("PreferencesWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
        PreferencesWidget->setTabText(PreferencesWidget->indexOf(accountPage), QApplication::translate("PreferencesWidget", "Account", 0, QApplication::UnicodeUTF8));
        uploadingLabel->setText(QApplication::translate("PreferencesWidget", "Uploading:", 0, QApplication::UnicodeUTF8));
        deleteAfterUploadCheckBox->setText(QApplication::translate("PreferencesWidget", "Delete screenshots after upload", 0, QApplication::UnicodeUTF8));
        deleteOtherCheckBox->setText(QApplication::translate("PreferencesWidget", "Delete all other file types after upload", 0, QApplication::UnicodeUTF8));
        startAtLoginCheckBox->setText(QApplication::translate("PreferencesWidget", "Launch application on startup", 0, QApplication::UnicodeUTF8));
        launchLabel->setText(QApplication::translate("PreferencesWidget", "Launch:", 0, QApplication::UnicodeUTF8));
        cancelpushButton->setText(QApplication::translate("PreferencesWidget", "Cancel", 0, QApplication::UnicodeUTF8));
        okpushButton->setText(QApplication::translate("PreferencesWidget", "Save", 0, QApplication::UnicodeUTF8));
        disableAutoUploadCheckBox->setText(QApplication::translate("PreferencesWidget", "Disable automatic uploads", 0, QApplication::UnicodeUTF8));
        editorCheckBox->setText(QApplication::translate("PreferencesWidget", "Annotate all images before uploading", 0, QApplication::UnicodeUTF8));
        copyUrlCheckBox->setText(QApplication::translate("PreferencesWidget", "Copy URL to clipboard after upload", 0, QApplication::UnicodeUTF8));
        linksLabel->setText(QApplication::translate("PreferencesWidget", "Links:", 0, QApplication::UnicodeUTF8));
        urlShortenerLabel->setText(QApplication::translate("PreferencesWidget", "Shorten URLs:", 0, QApplication::UnicodeUTF8));
        urlShortenerCheckBox->setText(QApplication::translate("PreferencesWidget", "Enable (less secure)", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PreferencesWidget", "Screencast:", 0, QApplication::UnicodeUTF8));
        muteCheckBox->setText(QApplication::translate("PreferencesWidget", "Mute Audio", 0, QApplication::UnicodeUTF8));
        PreferencesWidget->setTabText(PreferencesWidget->indexOf(preferencesPage), QApplication::translate("PreferencesWidget", "Preferences", 0, QApplication::UnicodeUTF8));
        label_12->setText(QString());
        fullScreenLabel_2->setText(QApplication::translate("PreferencesWidget", "Full Screen:", 0, QApplication::UnicodeUTF8));
        regionKwyLabel_2->setText(QApplication::translate("PreferencesWidget", "Draw Region:", 0, QApplication::UnicodeUTF8));
        label_14->setText(QString());
        screencastLabel_3->setText(QString());
        screencastKeyLabel_2->setText(QApplication::translate("PreferencesWidget", "Screencast:", 0, QApplication::UnicodeUTF8));
        label_11->setText(QString());
        regionKwyLabel->setText(QApplication::translate("PreferencesWidget", "Draw Region and Annotate:", 0, QApplication::UnicodeUTF8));
        uploadLabel_2->setText(QApplication::translate("PreferencesWidget", "Upload Files:", 0, QApplication::UnicodeUTF8));
        uploadIcon_2->setText(QString());
        windowLabel_2->setText(QApplication::translate("PreferencesWidget", "Grab Window:", 0, QApplication::UnicodeUTF8));
        windowIcon_2->setText(QString());
        defaultsShortcutsPushButton->setText(QApplication::translate("PreferencesWidget", "Reset to defaults", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PreferencesWidget", "Type shortcut or press Esc to cancel", 0, QApplication::UnicodeUTF8));
        PreferencesWidget->setTabText(PreferencesWidget->indexOf(shortcutsPage), QApplication::translate("PreferencesWidget", "Hot-keys", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PreferencesWidget: public Ui_PreferencesWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCESWIDGET_H
