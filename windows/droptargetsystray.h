/* droptargetsystray.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef DROPTARGETSYSTRAY_H
#define DROPTARGETSYSTRAY_H

#include "config.h"
#include "abstractdroptargetwidget.h"
#include <QPixmap>

#ifdef UI_SYSTRAY_ICON
#include "3rdparty/systray/systemtrayicon.h"
#define SystemTrayIcon_ SystemTrayIcon
#else
#include <QSystemTrayIcon>
#define SystemTrayIcon_ QSystemTrayIcon
#endif


class DropTargetSysTray: public AbstractDropTargetWidget
{
    Q_OBJECT
public:
    explicit DropTargetSysTray(SystemTrayIcon_* sysTrayIcon, QWidget* parent = 0);
    ~DropTargetSysTray();

protected:
    virtual void resizeEvent(QResizeEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void showEvent(QShowEvent* event);
    //virtual void dragMoveEvent(QDragMoveEvent* event);
    virtual void dragLeaveEvent(QDragLeaveEvent* event);
    virtual void dropEvent(QDropEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
    void updateRegion(QSize size);
    void init();

private slots:
    void onCustomContextMenuRequested(const QPoint& pos);
    void onAboutToShowSysTrayMessage();
    void onTimeout();
    void onShowAndTimeout();

private:
    SystemTrayIcon_* m_sysTrayIcon;
    QTimer* m_timer;
    QPixmap m_bkgPixmap;
};

#endif // DROPTARGETSYSTRAY_H
