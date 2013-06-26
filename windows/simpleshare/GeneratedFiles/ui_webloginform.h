/********************************************************************************
** Form generated from reading UI file 'webloginform.ui'
**
** Created: Mon Jun 24 20:09:05 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WEBLOGINFORM_H
#define UI_WEBLOGINFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <webview.h>

QT_BEGIN_NAMESPACE

class Ui_WebLoginForm
{
public:
    QVBoxLayout *verticalLayout;
    WebView *webView;

    void setupUi(QWidget *WebLoginForm)
    {
        if (WebLoginForm->objectName().isEmpty())
            WebLoginForm->setObjectName(QString::fromUtf8("WebLoginForm"));
        WebLoginForm->setWindowModality(Qt::ApplicationModal);
        WebLoginForm->resize(900, 600);
        verticalLayout = new QVBoxLayout(WebLoginForm);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        webView = new WebView(WebLoginForm);
        webView->setObjectName(QString::fromUtf8("webView"));
        webView->setProperty("url", QVariant(QUrl(QString::fromUtf8("about:blank"))));

        verticalLayout->addWidget(webView);


        retranslateUi(WebLoginForm);

        QMetaObject::connectSlotsByName(WebLoginForm);
    } // setupUi

    void retranslateUi(QWidget *WebLoginForm)
    {
        WebLoginForm->setWindowTitle(QApplication::translate("WebLoginForm", "Box SimpleShare Single Sign On", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class WebLoginForm: public Ui_WebLoginForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WEBLOGINFORM_H
