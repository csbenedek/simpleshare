#include "webloginform.h"
#include "ui_webloginform.h"
#include <QDebug>

const QString g_authString = "/auth/";
const QString g_authorizedString = "?authorized=1";

WebLoginForm::WebLoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebLoginForm)
{
    ui->setupUi(this);

    connectUi();
}

WebLoginForm::~WebLoginForm()
{
    delete ui;
}

void WebLoginForm::connectUi()
{
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
}

void WebLoginForm::onUrlChanged(QUrl url)
{
    qDebug() << Q_FUNC_INFO << url;
    const QString urlString = url.toString();
    if (urlString.contains(g_authorizedString))
    {
        const int beginIndex = urlString.indexOf(g_authString) + g_authString.length();
        const int endIndex = urlString.indexOf(g_authorizedString);
        const QString ticketId = urlString.mid(beginIndex, endIndex - beginIndex);
        emit onTicketId(ticketId);
    }
}

void WebLoginForm::goToUrl(const QString& url)
{
    ui->webView->load(QUrl(url));
    ui->webView->show();
    show();
}
