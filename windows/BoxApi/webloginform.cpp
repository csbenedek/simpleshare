#include "webloginform.h"
#include "ui_webloginform.h"
#include <QDebug>
#include <QCloseEvent>
#include "utils.h"

// OAuth2

#define OAUTH2_CLIENT_ID      BOXNET_API_KEY

#define OAUTH2_AUTHORIZE_URL       "https://api.box.com/oauth2/authorize?"
#define OAUTH2_AUTHORIZE_CODE_URL  OAUTH2_AUTH_AUTHORIZE_URL "response_type=code&client_id=%s&redirect_uri=%s"

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

    m_url = url.toString();
    /*if (m_url.right(g_strError.length()) == g_strError)
    {
        m_response = BxNet::not_a_sso_user;
        return;
    }

    if (m_url.right(g_authorizedString.length()) == g_authorizedString)
    {
        const int beginIndex = m_url.indexOf(g_authString) + g_authString.length();
        const int endIndex = m_url.indexOf(g_authorizedString);
        const QString oauth2_code = m_url.mid(beginIndex, endIndex - beginIndex);
        m_response = BxNet::logged;
        emit onAuthSuccess(m_response, oauth2_code);
        return;
    }
*/
    if ( strncasecmp(m_url.toStdString().c_str(), OAUTH2_AUTHORIZE_URL, strlen(OAUTH2_AUTHORIZE_URL)) )
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

    const QString request = "https://app.box.com/api/oauth2/authorize?"
                      "response_type=code&client_id=" OAUTH2_CLIENT_ID "&"
                      "redirect_uri=https://app.box.com/api/oauth2/logged_in&"
                      "state=security_token%3DKnhMJatFipTAnM0nHlZA";

    goToUrl(request);
}

void WebLoginForm::onLoadFinished(bool ok)
{
    if (m_response == BxNet::unknown_status && !isVisible()
            && !m_url.isEmpty() && (m_url != "about:blank")
            /*&& (m_url.left(g_boxNetServicesString.length()) != g_boxNetServicesString)
            && (m_url.left(g_boxNetString.length()) != g_boxNetString) */
            )
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
