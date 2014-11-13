#ifndef Widget_H
#define Widget_H

#include <QTabWidget>

#define UI_SYSTRAY_ICON

#include <QSystemTrayIcon>

#ifdef UI_SYSTRAY_ICON
#include "systemtray_ext/systemtrayicon_ext.h"
#define SystemTrayIcon_ SystemTrayIconExt
#else
#define SystemTrayIcon_ QSystemTrayIcon
#endif

namespace Ui {
    class Widget;
}

class Widget : public QTabWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void onClick();

private:
    Ui::Widget *ui;

    // system tray menu
    QIcon               m_Icon;
    SystemTrayIcon_*    m_systemTrayIcon;
};

#endif // WIDGET_H
