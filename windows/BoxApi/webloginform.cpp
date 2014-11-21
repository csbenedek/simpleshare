#include "webloginform.h"
#include "ui_webloginform.h"
#include <QDebug>
#include <QCloseEvent>
#include "utils.h"

// OAuth2

#define OAUTH2_CLIENT_ID      BOXNET_API_KEY

#define OAUTH2_AUTHORIZE_URL       "https://api.box.com/oauth2/authorize?"
#define OAUTH2_AUTHORIZE_CODE_URL  OAUTH2_AUTH_AUTHORIZE_URL "response_type=code&client_id=%s&redirect_uri=%s"

#define OAUTH2_REDIRECT_URL        "https://app.box.com/oauth2/logged_in"

#define OAUTH2_AUTH_TOKEN_URL "https://api.box.com/oauth2/token"


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
