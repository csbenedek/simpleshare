/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QtCore/qobject.h>
#include <QSystemTrayIcon>

#ifndef QT_NO_SYSTEMTRAYICON

#include <QtGui/qicon.h>

QT_BEGIN_HEADER

class SystemTrayIconPrivate;

class QMenu;
class QEvent;
class QWheelEvent;
class QMouseEvent;
class QPoint;

class SystemTrayIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible DESIGNABLE false)

public:
    SystemTrayIcon(QObject *parent = 0);
    SystemTrayIcon(const QIcon &icon, QObject *parent = 0);
    ~SystemTrayIcon();

#ifndef QT_NO_MENU
    void setContextMenu(QMenu *menu);
    QMenu *contextMenu() const;
#endif

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QString toolTip() const;
    void setToolTip(const QString &tip);

    static bool isSystemTrayAvailable();
    static bool supportsMessages();

    enum MessageIcon { NoIcon, Information, Warning, Critical, UserIcon };
    void showMessage(const QString &title, const QString &msg,
                     MessageIcon icon, int msecs, QIcon pixmap);

    QRect geometry() const;
    bool isVisible() const;

public Q_SLOTS:
    void setVisible(bool visible);
    inline void show() { setVisible(true); }
    inline void hide() { setVisible(false); }

Q_SIGNALS:
    void activated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void aboutToShowMessage();

protected:
    bool event(QEvent *event);   

private:
    Q_DISABLE_COPY(SystemTrayIcon)
    Q_DECLARE_PRIVATE(SystemTrayIcon)

    friend class SystemTrayIconSys;    
    friend void qtsystray_sendActivated(SystemTrayIcon *, int);
};

QT_END_HEADER

#endif // QT_NO_SYSTEMTRAYICON
#endif // SystemTrayIcon_H
