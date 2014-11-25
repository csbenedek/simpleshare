/* webviewex.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef WEBVIEWEX_H
#define WEBVIEWEX_H

#include <QWebView>

class WebViewEx : public QWebView
{
    Q_OBJECT
public:
    explicit WebViewEx(QWidget *parent = 0);

    void click(const QString &selectorQuery);

signals:

public slots:

};

#endif // WEBVIEWEX_H
