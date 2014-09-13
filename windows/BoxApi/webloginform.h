#ifndef WEBLOGINFORM_H
#define WEBLOGINFORM_H

#include <QWidget>
#include <QUrl>
#include <QTimer>
#include "bxnet.h"

namespace Ui {
    class WebLoginForm;
}

class WebLoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit WebLoginForm(QWidget *parent = 0);
    ~WebLoginForm();

    void goToUrl(const QString& url);

signals:
    void onSSOTicketId(const QString& ticketId);
    void onSSOError(BxNet::RESPONSE_STATUS err);
    void beginSSO();

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void timerEvent(QTimerEvent *event);

private:
    void connectUi();

private slots:
    void onUrlChanged(QUrl url);
    void onLoadFinished(bool ok);

private:
    Ui::WebLoginForm *ui;

    QString     m_initialUrl;
    int         m_timerId;
    BxNet::RESPONSE_STATUS m_response;
    QString     m_url;
};

#endif // WEBLOGINFORM_H
