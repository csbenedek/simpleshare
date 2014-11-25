/* webloginform.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */


#include "webloginform.h"
#include "ui_webloginform.h"
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QString>
#include "utils.h"

WebLoginForm::WebLoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebLoginForm),
    m_timerId(-1)
{
    setWindowFlags(windowFlags());
                    //|   Qt::Tool)
                    //& ~(Qt::WindowMaximizeButtonHint
                    //    | Qt::WindowMinimizeButtonHint));

    m_response = BxNet::unknown_status;

    ui->setupUi(this);

    connectUi();

    centrateWidget(this);
    hide();
}

WebLoginForm::~WebLoginForm()
{
    if (m_timerId != -1)
    {
        m_timerId = -1;
        killTimer(m_timerId);
    }

    delete ui;
}

void WebLoginForm::connectUi()
{
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
}

void WebLoginForm::closeEvent(QCloseEvent *event)
{
    if (m_response == BxNet::unknown_status)
    {
        m_response = BxNet::sign_on_canceled;
        emit onAuthError(m_response);
    }
    event->accept();
}

void WebLoginForm::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId)
    {
        if (m_timerId != -1)
        {
            killTimer(m_timerId);
            m_timerId = -1;
        }

        if (m_response == BxNet::unknown_status)
        {
            m_response = BxNet::sign_on_timeout;
            emit onAuthError(m_response);
        }
    }
}

void WebLoginForm::onUrlChanged(QUrl url)
{
    if (m_timerId != -1)
    {
        killTimer(m_timerId);
        m_timerId = -1;
    }

    if ( strncasecmp(url.toString().toStdString().c_str(), OAUTH2_REDIRECT_URL, strlen(OAUTH2_REDIRECT_URL)) == 0 )
    {
        QString error = url.queryItemValue("error");
        if ( !error.isEmpty() )
        {
            //invalid_request	The request is missing a required parameter, includes an invalid parameter value, includes a parameter more than once, or is otherwise malformed.
            //unsupported_response_type	The authorization server does not support obtaining an authorization code using this method.
            //Check the value of the code param in your request.
            //access_denied	The resource owner or authorization server denied the request.
            //server_error	The device the user is trying to log in from is not authorized to access the user’s account.
            //temporarily_unavailable


            if ( error != "access_denied" )
            {
                QString error_description = url.queryItemValue("error_description");
                error_description.replace('+', ' ');
                QMessageBox::critical(this, "OAuth2 Error", error + ": " + error_description);
            }
            m_response = BxNet::not_logged_in;
            emit onAuthError(m_response);
            return;
        }

        QString state = url.queryItemValue("state");
        if (m_state.compare(state) != 0)
        {
            m_response = BxNet::wrong_auth_token;
            emit onAuthError(m_response);
            return;
        }
        QString oauth2_code = url.queryItemValue("code");
        if ( !oauth2_code.isEmpty() )
        {
            m_response = BxNet::logged;
            emit onAuthSuccess(m_response, oauth2_code);
            securelyErase(m_state);
            securelyErase(state);
            securelyErase(oauth2_code);
            return;
        }
        else
        {
            m_response = BxNet::wrong_auth_token;
            emit onAuthError(m_response);
            return;
        }
    }
    else if ( strncasecmp(url.toString().toStdString().c_str(), OAUTH2_AUTHORIZE_URL, strlen(OAUTH2_AUTHORIZE_URL)) == 0 )
    {
        m_timerId = startTimer(30000); // 30 sec. timeout
    }   
}

void WebLoginForm::goToUrl(const QString& url)
{
    m_response = BxNet::unknown_status;

    if (m_timerId != -1)
    {
        killTimer(m_timerId);
        m_timerId = -1;
    }

    ui->webView->setHtml(QString(), QUrl()); // clear web
    ui->webView->load(QUrl(url));
    ui->webView->show();
    //show();

}

void WebLoginForm::goToOAuth2LoginUrl()
{
    //https://app.box.com/api/oauth2/authorize?response_type=code&client_id=MY_CLIENT_ID&state=security_token%3DKnhMJatFipTAnM0nHlZA

    m_state = "security_token%3DKnhMJatFipTAnM0nHlZA";

    QString request = "https://app.box.com/api/oauth2/authorize?"
                      "response_type=code&client_id=" OAUTH2_CLIENT_ID "&"
                      "redirect_uri=" OAUTH2_REDIRECT_URL "&"
                      "state=" + m_state;

    goToUrl(request);
    securelyErase(request);
}

void WebLoginForm::onLoadFinished(bool ok)
{
    if ( m_response == BxNet::unknown_status && !isVisible() )
    {
        emit beginOAuthLogin();
        show();
        activateWindow();
        raise();
    }

    if (m_response == BxNet::not_a_sso_user)
    {
        emit onAuthError(m_response);
        return;
    }

    if (!ok) // && (m_url.left(g_boxNetString.length()) == g_boxNetString))
    {
        if (m_timerId != -1)
        {
            killTimer(m_timerId);
            m_timerId = -1;
        }

        if (m_response == BxNet::unknown_status)
        {
            m_response = BxNet::sign_on_canceled;
            emit onAuthError(m_response);
        }
        return;
    }
}
