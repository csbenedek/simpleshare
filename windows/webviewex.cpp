/* webviewex.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "webviewex.h"
#include <QMouseEvent>
#include <QWebView>
#include <QWebElement>
#include <QWebFrame>
#include <QCoreApplication>

WebViewEx::WebViewEx(QWidget *parent) :
    QWebView(parent)
{
}

void WebViewEx::click(const QString &selectorQuery)
{
    QWebElement el = this->page()->mainFrame()->findFirstElement(selectorQuery);
    if (el.isNull())
    {
        return;
    }

    el.setFocus();

    QMouseEvent pressEvent(QMouseEvent::MouseButtonPress, el.geometry().center(),
                    Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &pressEvent);

    QMouseEvent releaseEvent(QMouseEvent::MouseButtonRelease,
                             el.geometry().center(), Qt::LeftButton,
                             Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &releaseEvent);
}
