/* webloginform.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef WEBLOGINFORM_H
#define WEBLOGINFORM_H

#include <QWidget>
#include <QUrl>
#include <QTimer>
#include "bxnet.h"

namespace Ui {
    class WebLoginForm;
}

//    const QString lastUserName = settings.value("options/lastUserName", "").toString();
//    const bool isSSO = settings.value("options/lastSSO", false).toBool();


//    connect(m_bxnet, SIGNAL(authFailed(BxNet::RESPONSE_STATUS)), this, SLOT(onAuthFailed(BxNet::RESPONSE_STATUS)));
//    connect(m_bxnet, SIGNAL(authCompleted()), this, SLOT(onLogin()));
//    connect(m_bxnet, SIGNAL(newUserFailed(BxNet::RESPONSE_STATUS)), this, SLOT(newUserFailed(BxNet::RESPONSE_STATUS)));
//    connect(m_bxnet, SIGNAL(newUserSucceded()), this, SLOT(onNewUserCreated()));
  
//  //connect(m_bxnet, SIGNAL(notLoggedIn()), this, SLOT(onNotLoggedIn()));

//    connect(m_bxnet, SIGNAL(beginSSO()), this, SLOT(hide()));
/*
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

*/

class WebLoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit WebLoginForm(QWidget *parent = 0);
    ~WebLoginForm();

    void goToOAuth2LoginUrl();

signals:
    void onAuthError(BxNet::RESPONSE_STATUS err);
    void onAuthSuccess(BxNet::RESPONSE_STATUS err, const QString& code);
    void beginOAuthLogin();

protected:
    virtual void goToUrl(const QString& url);

    virtual void closeEvent(QCloseEvent *event);
    virtual void timerEvent(QTimerEvent *event);

private:
    void connectUi();

private slots:
    void onUrlChanged(QUrl url);
    void onLoadFinished(bool ok);

private:
    Ui::WebLoginForm *ui;

    int         m_timerId;
    BxNet::RESPONSE_STATUS m_response;

    QString m_state;
};

#endif // WEBLOGINFORM_H
