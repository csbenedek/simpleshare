#include "webloginform.h"
#include "ui_webloginform.h"
#include <QDebug>
#include <QCloseEvent>
#include "utils.h"


const QString g_boxNetServicesString = "https://sso.services.box.net/";
const QString g_boxNetString = "https://www.box.com/";
const QString g_authString = "/auth/";
const QString g_authorizedString = "?authorized=1";
const QString g_strError = "/error/1";

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
        emit onSSOError(m_response);
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
            emit onSSOError(m_response);
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
    if (m_url.right(g_strError.length()) == g_strError)
    {
        m_response = BxNet::not_a_sso_user;
        return;
    }

    if (m_url.right(g_authorizedString.length()) == g_authorizedString)
    {
        const int beginIndex = m_url.indexOf(g_authString) + g_authString.length();
        const int endIndex = m_url.indexOf(g_authorizedString);
        const QString ticketId = m_url.mid(beginIndex, endIndex - beginIndex);
        emit onSSOTicketId(ticketId);
        m_response = BxNet::logged;
        return;
    }

    if (m_url.left(g_boxNetString.length()) == g_boxNetString)
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

void WebLoginForm::onLoadFinished(bool ok)
{
    if (m_response == BxNet::unknown_status && !isVisible()
            && !m_url.isEmpty() && (m_url != "about:blank")
            && (m_url.left(g_boxNetServicesString.length()) != g_boxNetServicesString)
            && (m_url.left(g_boxNetString.length()) != g_boxNetString))
    {
        emit beginSSO();
        show();
        activateWindow();
        raise();
    }

    if (m_response == BxNet::not_a_sso_user)
    {
        emit onSSOError(m_response);
        return;
    }

    if (!ok && (m_url.left(g_boxNetString.length()) == g_boxNetString))
    {
        if (m_timerId != -1)
        {
            killTimer(m_timerId);
            m_timerId = -1;
        }

        if (m_response == BxNet::unknown_status)
        {
            m_response = BxNet::sign_on_canceled;
            emit onSSOError(m_response);
        }
        return;
    }
}
