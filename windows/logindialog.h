#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "bxnet.h"
#include "tooltipwidget.h"


namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LoginDialog(BxNet* bxNet, QWidget *parent = 0);
    ~LoginDialog();


    bool isWelcomePage();
    void boxShowWelcome();

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void hideEvent(QHideEvent *);
    virtual void moveEvent(QMoveEvent *);
    virtual bool event(QEvent * event);

protected:
    void init();
    void connectUi();

    void disableUi();
    void enableUi();

    bool comparePasswords();
    void openForgotPasswordUrl();

    void showInplaceLoginError(const QString& errorToolTip);
    void clearErrors();
    void hideErrorToolTips();


private slots:
    void login();
    void ssoLogin();
    void moreLoginOptions();
    void boxLoginOptions();
    void backFromSignUp();
    void signUpOptions();
    void signUp();
    void onAuthFailed(BxNet::RESPONSE_STATUS err);
    void onLogin();   
    void currentChanged(int index);
    void clearSignUpForm();
    void newUserFailed(BxNet::RESPONSE_STATUS err);
    void onNewUserCreated();

    void onSignUpEmailEditingFinished();
    void onPasswordEditingFinished();

    void on_emailLineEdit_returnPressed();
    void on_userNameLineEdit_returnPressed();
    void on_passwordLineEdit_returnPressed();
    void on_emailLineEdit_2_returnPressed();
    void on_passwordLineEdit_2_returnPressed();
    void on_confirmPasswordLineEdit_returnPressed();
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::LoginDialog *ui;

    BxNet* m_bxnet;

    int     m_prevPage;
    bool    m_sso;
    bool    m_canClose;

    QPointer<ToolTipWidget>  m_toolTipWidget;
};

#endif // LOGINDIALOG_H
