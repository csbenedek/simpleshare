/* messagedialog.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class MessageDialog;
}

class MessageDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MessageDialog(QWidget *parent = 0);
    ~MessageDialog();

    static int error(QWidget *parent, const QString &title, const QString& text);
    inline static int error(QWidget *parent, const QString &text) { return error(parent, "", text); }
    
private slots:
    void on_textBrowser_anchorClicked(const QUrl &arg1);

private:
    Ui::MessageDialog *ui;
};

#endif // MESSAGEDIALOG_H
