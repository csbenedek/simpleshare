#include "logindialog.h"
#include "ui_logindialog.h"
#include <QSettings>
#include <QCloseEvent>
#include "utils.h"
#include <QDesktopServices>
#include "preferenceswidget.h"
#include "messagedialog.h"



extern PreferencesWidget* g_preferencesWidget;

LoginDialog::LoginDialog(BxNet* bxnet, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginDialog)
    , m_bxnet(bxnet)
    , m_sso(false)
    , m_canClose(true)
{
    ui->setupUi(this);
    ui->passwordLineEdit->setShadowBackground(false);
    ui->passwordLineEdit->setClearButton(false);
    ui->passwordLineEdit_2->setShadowBackground(false);
    ui->passwordLineEdit_2->setClearButton(false);
    ui->confirmPasswordLineEdit->setShadowBackground(false);
    ui->confirmPasswordLineEdit->setClearButton(false);
    ui->emailLineEdit_2->setShadowBackground(false);
    ui->emailLineEdit_2->setClearButton(false);

    init();

    connectUi();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::closeEvent(QCloseEvent *event)
{
    if (m_canClose)
    {
        deleteLater();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void LoginDialog::init()
{
    const QColor bgColor = palette().background().color();
    ui->textBrowser->setHtml(tr("<p style=\"margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; color:#666; font-family:\'Arial\'; font-size:10pt; font-weight:400; font-style:normal;\">Create a free account. To get more storage<br><a href=\"http://www.box.com/pricing\"><span style=\" text-decoration: none; color:#1a74ba;\">Learn more.</span></a></p>"));
    ui->textBrowser->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue()));
    //ui->forgotTextBrowser_2->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue()));

    QSettings settings;
    const QString lastUserName = settings.value("options/lastUserName", "").toString();
    ui->userNameLineEdit->setText(lastUserName);
    const bool isSSO = settings.value("options/lastSSO", false).toBool();
    ui->stackedWidget->setCurrentIndex( isSSO ? 1 : 0 );
    if (isSSO)
    {
        ui->emailLineEdit->setText(lastUserName);
    }
}

void LoginDialog::connectUi()
{
    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));

    connect(ui->loginPushButton, SIGNAL(clicked()), this, SLOT(login()));
    connect(ui->ssoLoginPushButton, SIGNAL(clicked()), this, SLOT(ssoLogin()));
    connect(ui->signUpPushButton_3, SIGNAL(clicked()), this, SLOT(signUp()));
    connect(ui->moreLoginOptionsPushButton, SIGNAL(clicked()), this, SLOT(moreLoginOptions()));
    connect(ui->boxLoginOptionsPushButton, SIGNAL(clicked()), this, SLOT(boxLoginOptions()));
    connect(ui->backPushButton, SIGNAL(clicked()), this, SLOT(backFromSignUp()));
    connect(ui->signPushButton, SIGNAL(clicked()), this, SLOT(signUpOptions()));
    connect(ui->signPushButton_2, SIGNAL(clicked()), this, SLOT(signUpOptions()));

    connect(ui->emailLineEdit_2, SIGNAL(editingFinished()), this, SLOT(onSignUpEmailEditingFinished()));
    connect(ui->confirmPasswordLineEdit, SIGNAL(editingFinished()), this, SLOT(onPasswordEditingFinished()));
    connect(ui->passwordLineEdit_2, SIGNAL(editingFinished()), this, SLOT(onPasswordEditingFinished()));

    connect(m_bxnet, SIGNAL(authFailed(BxNet::RESPONSE_STATUS)), this, SLOT(onAuthFailed(BxNet::RESPONSE_STATUS)));
    connect(m_bxnet, SIGNAL(authCompleted()), this, SLOT(onLogin()));
    connect(m_bxnet, SIGNAL(newUserFailed(BxNet::RESPONSE_STATUS)), this, SLOT(newUserFailed(BxNet::RESPONSE_STATUS)));
    connect(m_bxnet, SIGNAL(newUserSucceded()), this, SLOT(onNewUserCreated()));
    //connect(m_bxnet, SIGNAL(notLoggedIn()), this, SLOT(onNotLoggedIn()));

    connect(m_bxnet, SIGNAL(beginSSO()), this, SLOT(hide()));
}

void LoginDialog::disableUi()
{
    m_canClose = false;
    setCursor(Qt::WaitCursor);

    //ui->stackedWidget->setEnabled(false);

    ui->loginPushButton->setEnabled(false);
    ui->userNameLineEdit->setEnabled(false);
    ui->passwordLineEdit->setEnabled(false);

    ui->ssoLoginPushButton->setEnabled(false);
    ui->emailLineEdit->setEnabled(false);

    ui->emailLineEdit_2->setEnabled(false);
    ui->passwordLineEdit_2->setEnabled(false);
    ui->confirmPasswordLineEdit->setEnabled(false);
    ui->signUpPushButton_3->setEnabled(false);
    ui->backPushButton->setEnabled(false);
}

void LoginDialog::enableUi()
{
    m_canClose = true;
    unsetCursor();

    //ui->stackedWidget->setEnabled(true);

    ui->loginPushButton->setEnabled(true);
    ui->userNameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);

    ui->ssoLoginPushButton->setEnabled(true);
    ui->emailLineEdit->setEnabled(true);

    ui->emailLineEdit_2->setEnabled(true);
    ui->passwordLineEdit_2->setEnabled(true);
    ui->confirmPasswordLineEdit->setEnabled(true);
    ui->signUpPushButton_3->setEnabled(true);
    ui->backPushButton->setEnabled(true);
}

void LoginDialog::onAuthFailed(BxNet::RESPONSE_STATUS err)
{
    ui->emailLineEdit->selectAll();
    enableUi();
    show();

    //the user has not logged in in the browser

    //QString title;
    QString message;

    switch(err)
    {
    case BxNet::network_error:
        qDebug() << Q_FUNC_INFO << "Network error";
        //message = tr("Network error");// will be processed in networkerror handler
        break;
    case BxNet::not_a_sso_user:
        message = tr("<b>Single sign-on not enabled</b><br>Single sign-on is not enabled for your account.<br>Check your email address and try again.");
        break;
    case BxNet::sign_on_canceled:
        message = tr("Single Sign On Canceled");
        break;
    case BxNet::sign_on_timeout:
        message = tr("Single Sign On Timeout");
        break;
        // other types processed elsewhere
    case BxNet::auth_attempts_for_ip_limit_reached:
        message = tr("Auth attempts for ip limit reached");
        break;
    case BxNet::login_limit:
        message = tr("Login limit reached");
        break;
    case BxNet::invalid_login:
    default:
        //bool isEmailRegistered = m_bxnet->isEmailExists(ui->userNameLineEdit->text());
        //if (!isEmailRegistered)
        message = tr("<b>Invalid login details</b><br>Having trouble logging in?<br>you may want to <a href=\"http://www.box.com/reset\">reset your password</a><br>Resetting your password? Make sure you <b>click the link</b> in the email first</li></ul>");
        break;
    }

    if (!message.isEmpty())
    {
        if (ui->stackedWidget->currentIndex() == 1)
        {
            ui->emailLineEdit->setFocus();
        }
        else
        {
            ui->stackedWidget->setCurrentIndex(0);
            if (ui->userNameLineEdit->text().isEmpty())
            {
                ui->userNameLineEdit->setFocus();
            }
            else
            {
                ui->passwordLineEdit->setFocus();
            }
        }

        QString htmlMessage = "<p style=\"font: 8pt \"MS Sans Serif\"\">" + message + "</p>";

        MessageDialog::error(this, htmlMessage);
    }
}

void LoginDialog::onLogin()
{
    clearErrors();
    enableUi();

    QSettings settings;
    QString userName = m_bxnet->userName();
    settings.setValue("options/lastUserName", userName);
    settings.setValue("options/lastSSO", m_sso);

    if (ui->stackedWidget->currentIndex() >= 2) // show welcome page for sign up.
    {
        boxShowWelcome();
        show();
        raise();
    }
    else
    {
        close();
    }
}

void LoginDialog::login()
{
    clearErrors();
    Q_ASSERT(m_bxnet);
    if (m_bxnet)
    {
        int isError = false;
        QWidget* wantedFocus = NULL;

        if (ui->passwordLineEdit->text().isEmpty())
        {
            ui->passwordLineEdit->setErrorMode(true, tr("empty password"));
            wantedFocus = ui->passwordLineEdit;
            isError = true;
        }
        else if (ui->passwordLineEdit->text().length() < 6)
        {
            ui->passwordLineEdit->setErrorMode(true, tr("password too short"));
            wantedFocus = ui->passwordLineEdit;
            isError = true;
        }

        if (ui->userNameLineEdit->text().isEmpty())
        {
            ui->userNameLineEdit->setErrorMode(true, tr("empty user name"));
            wantedFocus = ui->userNameLineEdit;
            isError = true;
        }

        if (!isError)
        {

            m_bxnet->directLogin(
                        ui->userNameLineEdit->text(),
                        ui->passwordLineEdit->text());

            ui->passwordLineEdit->setText(QString());
            disableUi();

        }
        else
        {
            wantedFocus->setFocus();
        }

    }
}

void LoginDialog::ssoLogin()
{
    clearErrors();
    Q_ASSERT(m_bxnet);
    if (m_bxnet && !ui->emailLineEdit->text().isEmpty())
    {
        m_sso = true;
        m_bxnet->ssoLogin(ui->emailLineEdit->text());
        disableUi();
    }
}

void LoginDialog::moreLoginOptions()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void LoginDialog::boxLoginOptions()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void LoginDialog::signUpOptions()
{
    m_prevPage = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(2);
}

void LoginDialog::backFromSignUp()
{
    if (m_prevPage == 1)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void LoginDialog::currentChanged(int index)
{
    clearErrors();
    if (index == 0)
    {
        if (ui->userNameLineEdit->text().isEmpty())
        {
            ui->userNameLineEdit->setText(ui->emailLineEdit->text());
        }
    }
    else if (index == 1)
    {
        if (ui->emailLineEdit->text().isEmpty())
        {
            ui->emailLineEdit->setText(ui->userNameLineEdit->text());
        }
    }
    else if (index == 2)
    {
        clearSignUpForm();
    }
}

void LoginDialog::on_emailLineEdit_returnPressed()
{
    ssoLogin();
}

void LoginDialog::on_userNameLineEdit_returnPressed()
{
    if (!ui->userNameLineEdit->text().isEmpty())
    {
        ui->passwordLineEdit->setFocus();
    }

}

void LoginDialog::on_passwordLineEdit_returnPressed()
{
    if (!ui->passwordLineEdit->text().isEmpty())
    {
        login();
    }
}

void LoginDialog::clearSignUpForm()
{
    clearErrors();
    foreach(QObject* widget, ui->signUpWidget->children())
    {
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);
        if (lineEdit)
        {
            lineEdit->setText("");
        }
    }
}

void LoginDialog::newUserFailed(BxNet::RESPONSE_STATUS err)
{
    enableUi();

    QString message;

    switch(err)
    {
    case BxNet::network_error:
        message = tr("Network error");
        break;
    case BxNet::email_invalid:
        message = tr("Invalid e-mail");
        break;
    case BxNet::email_already_registered:
        message = tr("E-mail already registered");
        break;
    case BxNet::e_register:
        message = tr("Generic error registering new user");
        break;
    default:
        break;
    }

    if (!message.isEmpty())
    {
        ui->emailLineEdit_2->selectAll();
        ui->emailLineEdit_2->setFocus();
        ui->stackedWidget->setCurrentIndex(2);
        //QMessageBox::warning(this, fullAppName() + tr(" Error" ), message);
        MessageDialog::error(this, message);
    }
    else
    {
        clearSignUpForm();
    }
}

void LoginDialog::onNewUserCreated()
{
    //enableUi(); //enabled in onLogin
    clearSignUpForm();
}

void LoginDialog::signUp()
{    
    clearErrors();

    const QString email = ui->emailLineEdit_2->text();

    if (email.isEmpty() || !email.contains("@") || !email.contains(".")) // TODO: regexp here
    {
        ui->emailLineEdit_2->setErrorMode(true, tr("Please enter a valid email address"));
        ui->emailLineEdit_2->setFocus();
        return;
    }

    QString password = ui->passwordLineEdit_2->text();   
    QString confirm = ui->confirmPasswordLineEdit->text();
    if (password != confirm)
    {
        ui->passwordLineEdit_2->setErrorMode(true, tr("Your passwords do not match"));
        ui->confirmPasswordLineEdit->setErrorMode(true);
        ui->passwordLineEdit_2->setFocus();
        //QMessageBox::warning(this, fullAppName() + tr(" Error" ), tr("Passwords differ"));
        return;
    }
    if (password.length() < 6)
    {
        securelyErase(password);
        ui->passwordLineEdit_2->setErrorMode(true, tr("Please enter a password with at least 6 characters"));
        //QMessageBox::warning(this, fullAppName() + tr(" Error" ), tr("Password too short"));
        ui->passwordLineEdit_2->setFocus();
        return;
    }
    securelyErase(confirm);

    Q_ASSERT(m_bxnet);
    if (m_bxnet)
    {
        disableUi();

        m_bxnet->newUser(email, password);
    }
    securelyErase(password);
}

void LoginDialog::on_emailLineEdit_2_returnPressed()
{
    if (!ui->emailLineEdit_2->text().isEmpty())
    {
        ui->passwordLineEdit_2->setFocus();
    }
}

void LoginDialog::on_passwordLineEdit_2_returnPressed()
{
    if (!ui->passwordLineEdit_2->text().isEmpty())
    {
        ui->passwordLineEdit_2->setFocus();
    }
}

void LoginDialog::on_confirmPasswordLineEdit_returnPressed()
{
    if (!ui->confirmPasswordLineEdit->text().isEmpty())
    {
        signUp();
    }
}

bool LoginDialog::comparePasswords()
{
    return (ui->passwordLineEdit_2->text() == ui->confirmPasswordLineEdit->text());
}


void LoginDialog::boxShowWelcome()
{
    ui->stackedWidget->setCurrentIndex(3);
}

bool LoginDialog::isWelcomePage()
{
    return ui->stackedWidget->currentIndex() == 3;
}

void LoginDialog::openForgotPasswordUrl()
{
    QDesktopServices::openUrl(QUrl("http://www.box.com/reset"));
}

void LoginDialog::on_pushButton_3_clicked()
{
    close();
    g_preferencesWidget->showOptions(0);
}

void LoginDialog::on_pushButton_clicked()
{
    openForgotPasswordUrl();
}

void LoginDialog::on_pushButton_2_clicked()
{
    openForgotPasswordUrl();
}

void LoginDialog::showInplaceLoginError(const QString& errorToolTip)
{
    if (ui->stackedWidget->currentIndex() == 0)
    {
        ui->userNameLineEdit->setErrorMode(true, errorToolTip);
        ui->passwordLineEdit->setErrorMode(true);
    }
    else if (ui->stackedWidget->currentIndex() == 1)
    {
        ui->emailLineEdit->setErrorMode(true, errorToolTip);
    }
    else if (ui->stackedWidget->currentIndex() == 2)
    {
        ui->emailLineEdit_2->setErrorMode(true, errorToolTip);
    }
}


void LoginDialog::clearErrors()
{
    ui->userNameLineEdit->setErrorMode(false);
    ui->passwordLineEdit->setErrorMode(false);
    ui->emailLineEdit->setErrorMode(false);
    ui->emailLineEdit_2->setErrorMode(false);
    ui->passwordLineEdit_2->setErrorMode(false);
    ui->confirmPasswordLineEdit->setErrorMode(false);
}

void LoginDialog::hideErrorToolTips()
{
    ui->userNameLineEdit->hideErrorToolTip();
    ui->passwordLineEdit->hideErrorToolTip();
    ui->emailLineEdit->hideErrorToolTip();
    ui->emailLineEdit_2->hideErrorToolTip();
    ui->passwordLineEdit_2->hideErrorToolTip();
    ui->confirmPasswordLineEdit->hideErrorToolTip();
}

void LoginDialog::hideEvent(QHideEvent *)
{
    hideErrorToolTips();
    qApp->setActiveWindow(g_preferencesWidget);
}

void LoginDialog::moveEvent(QMoveEvent *)
{
    hideErrorToolTips();
}

bool LoginDialog::event(QEvent * event)
{
    switch(event->type())
    {
    case QEvent::WindowDeactivate:
    case QEvent::FocusOut:
        //qDebug() << Q_FUNC_INFO << "event->type() = " << event->type() << "clear errors";
        hideErrorToolTips();
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void LoginDialog::onSignUpEmailEditingFinished()
{
    const QString email = ui->emailLineEdit_2->text();

    if (email.isEmpty() || !email.contains("@") || !email.contains(".")) // TODO: regexp here
    {
        ui->emailLineEdit_2->setErrorMode(true, tr("Please enter a valid email address"));
    }
}

void LoginDialog::onPasswordEditingFinished()
{
    TextInputWidget* inputWidget = qobject_cast<TextInputWidget*>(sender());
    TextInputWidget* confirmWidget = ui->confirmPasswordLineEdit;
    if (inputWidget == confirmWidget)
    {
        confirmWidget = ui->passwordLineEdit_2;
    }

    if (inputWidget->text().length() < 6)
    {
        inputWidget->setErrorMode(true, tr("Please enter a password with at least 6 characters"));
        if (confirmWidget)
        {
            confirmWidget->hideErrorToolTip();
        }
        return;
    }

    if (inputWidget->text() != confirmWidget->text())
    {
        inputWidget->setErrorMode(true, tr("Your passwords do not match"));
        confirmWidget->setErrorMode(true);
    }
}
