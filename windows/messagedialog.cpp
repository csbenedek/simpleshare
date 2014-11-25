/* messagedialog.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "messagedialog.h"
#include "ui_messagedialog.h"
#include "messagedialog.h"
#include <QDesktopServices>

MessageDialog::MessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);

    const QColor bgColor = palette().background().color();
    ui->textBrowser->setStyleSheet(QString("background-color: rgb(%1, %2, %3);").arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue()));

    // update version inforamtion in label
    ui->versionLabel->setText("v" + qApp->applicationVersion());
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

int MessageDialog::error(QWidget *parent, const QString &title, const QString& text)
{
    MessageDialog* msg = new MessageDialog(parent);
    if (!title.isEmpty())
    {
        msg->setWindowTitle(title);
    }
    else
    {
        msg->setWindowTitle(tr("Box SimpleShare Error"));
    }
    msg->ui->textBrowser->setHtml(text);
    msg->exec();
    int result = msg->result();
    delete msg;
    return result;
}

void MessageDialog::on_textBrowser_anchorClicked(const QUrl &arg1)
{
    close();

    QDesktopServices::openUrl(arg1);
}
