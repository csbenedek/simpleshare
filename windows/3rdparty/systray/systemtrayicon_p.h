#ifndef SYSTEMTRAYICON_P_H
#define SYSTEMTRAYICON_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "systemtrayicon.h"
#include "qobject_p.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include "QtGui/qmenu.h"
#include "QtGui/qpixmap.h"
#include "QtCore/qstring.h"
#include "QtCore/qpointer.h"

class SystemTrayIconSys;
class QToolButton;
class QLabel;

class SystemTrayIconPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(SystemTrayIcon)

public:
    SystemTrayIconPrivate() : sys(0), visible(false) { }

    void install_sys();
    void remove_sys();
    void updateIcon_sys();
    void updateToolTip_sys();
    void updateMenu_sys();
    QRect geometry_sys() const;
    void showMessage_sys(const QString &msg, const QString &title, SystemTrayIcon::MessageIcon icon, int secs, QIcon pixmap);

    static bool isSystemTrayAvailable_sys();
    static bool supportsMessages_sys();

    QPointer<QMenu> menu;
    QIcon icon;
    QString toolTip;
    SystemTrayIconSys *sys;
    bool visible;
};

#endif // QT_NO_SYSTEMTRAYICON

#endif // SYSTEMTRAYICON_P_H

