#ifndef WEBLOGINFORM_H
#define WEBLOGINFORM_H

#include <QWidget>
#include <QUrl>

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
    void onTicketId(const QString& ticketId);

private:
    void connectUi();

private slots:
    void onUrlChanged(QUrl url);

private:
    Ui::WebLoginForm *ui;
};

#endif // WEBLOGINFORM_H
