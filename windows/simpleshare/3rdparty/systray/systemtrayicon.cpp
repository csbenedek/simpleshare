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

#include "systemtrayicon.h"
#include "systemtrayicon_p.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include "qmenu.h"
#include "qevent.h"
#include "qpoint.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qpainterpath.h"
#include "qpainter.h"
#include "qstyle.h"
#include "qgridlayout.h"
#include "qapplication.h"
#include "qdesktopwidget.h"
#include "qbitmap.h"
#include "qapplication.h"

#include <QTimer>

SystemTrayIcon::SystemTrayIcon(QObject *parent)
: QObject(*new SystemTrayIconPrivate(), parent)
{
}

SystemTrayIcon::SystemTrayIcon(const QIcon &icon, QObject *parent)
: QObject(*new SystemTrayIconPrivate(), parent)
{
    setIcon(icon);
}

SystemTrayIcon::~SystemTrayIcon()
{
    Q_D(SystemTrayIcon);
    d->remove_sys();
}

#ifndef QT_NO_MENU

void SystemTrayIcon::setContextMenu(QMenu *menu)
{
    Q_D(SystemTrayIcon);
    d->menu = menu;
    d->updateMenu_sys();
}

QMenu* SystemTrayIcon::contextMenu() const
{
    Q_D(const SystemTrayIcon);
    return d->menu;
}

#endif // QT_NO_MENU

void SystemTrayIcon::setIcon(const QIcon &icon)
{
    Q_D(SystemTrayIcon);
    d->icon = icon;
    d->updateIcon_sys();
}

QIcon SystemTrayIcon::icon() const
{
    Q_D(const SystemTrayIcon);
    return d->icon;
}

void SystemTrayIcon::setToolTip(const QString &tooltip)
{
    Q_D(SystemTrayIcon);
    d->toolTip = tooltip;
    d->updateToolTip_sys();
}

QString SystemTrayIcon::toolTip() const
{
    Q_D(const SystemTrayIcon);
    return d->toolTip;
}

QRect SystemTrayIcon::geometry() const
{
    Q_D(const SystemTrayIcon);
    if (!d->visible)
        return QRect();
    return d->geometry_sys();
}

void SystemTrayIcon::setVisible(bool visible)
{
    Q_D(SystemTrayIcon);
    if (visible == d->visible)
        return;
    if (d->icon.isNull() && visible)
        qWarning("SystemTrayIcon::setVisible: No Icon set");
    d->visible = visible;
    if (d->visible)
        d->install_sys();
    else
        d->remove_sys();
}

bool SystemTrayIcon::isVisible() const
{
    Q_D(const SystemTrayIcon);
    return d->visible;
}

bool SystemTrayIcon::event(QEvent *e)
{
#if defined(Q_WS_X11)
    if (e->type() == QEvent::ToolTip) {
        Q_D(SystemTrayIcon);
        return d->sys->deliverToolTipEvent(e);
    }
#endif
    return QObject::event(e);
}

bool SystemTrayIcon::isSystemTrayAvailable()
{
    return SystemTrayIconPrivate::isSystemTrayAvailable_sys();
}

bool SystemTrayIcon::supportsMessages()
{
    return SystemTrayIconPrivate::supportsMessages_sys();
}

void SystemTrayIcon::showMessage(const QString& title, const QString& msg,
                            SystemTrayIcon::MessageIcon icon, int msecs, QIcon pixmap)
{
    emit aboutToShowMessage();
    qApp->processEvents();

    if (!pixmap.isNull())
    {
        icon = SystemTrayIcon::UserIcon;
    }
    Q_D(SystemTrayIcon);
    if (d->visible)
    {

        d->showMessage_sys(title, msg, icon, msecs, pixmap);
    }
}


void qtsystray_sendActivated(SystemTrayIcon *i, int r)
{
    emit i->activated((QSystemTrayIcon::ActivationReason)r);
}

#endif // QT_NO_SYSTEMTRAYICON
