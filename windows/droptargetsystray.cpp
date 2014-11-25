/* droptargetsystray.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "droptargetsystray.h"
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QMessageBox>
#include <QTimer>
#include <QMenu>
#include <QToolTip>
#include <QApplication>
#include <QPainter>
#include "applicationex.h"
#include "preferenceswidget.h"
#include "graber.h"
#include "utils.h"

// DropTargetSysTray

DropTargetSysTray::DropTargetSysTray(SystemTrayIcon_* sysTrayIcon, QWidget *parent)
    : AbstractDropTargetWidget(parent)
    , m_sysTrayIcon(sysTrayIcon)
{
    init();
}


DropTargetSysTray::~DropTargetSysTray()
{

}

void DropTargetSysTray::init()
{
    setWindowFlags(Qt::Tool
                 | Qt::WindowStaysOnTopHint
                 | Qt::FramelessWindowHint);

    const double minimumOpacity = 0.01;
    setWindowOpacity(minimumOpacity);// small, but not a 0.0
    // TODO: setStyleSheet("* { background-color: none; } ");
    //setBackgroundRole(QPalette::NoRole);
    //setMouseTracking(true);
    //setStyleSheet("background-color: rgba(255, 255, 255, 1);");

    //setAttribute(Qt::WA_NoSystemBackground);
    //setAttribute(Qt::WA_OpaquePaintEvent);
    //setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_TransparentForMouseEvents);

    setFocusPolicy(Qt::NoFocus);

    //updateRegion(size()); // don't really need to update region in constructor. it will be updated later

    // on winXP tray icons small and DropTarget overlap them.
    // in this case, DropTarget must process context menu and double-click events.
    // in Vista and Win7 this is doesn't need
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA)
    {
        setContextMenuPolicy(Qt::NoContextMenu);
    }

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_timer->setInterval(100);
    m_timer->start();

    Q_ASSERT(m_sysTrayIcon);
    if (m_sysTrayIcon)
    {
        setContextMenu(m_sysTrayIcon->contextMenu());
        connect(m_sysTrayIcon, SIGNAL(aboutToShowMessage()), this, SLOT(onAboutToShowSysTrayMessage()));
    }

    QTimer::singleShot(250, this, SLOT(onShowAndTimeout()));

}

void DropTargetSysTray::onShowAndTimeout()
{
    show();
    onTimeout();
}

void DropTargetSysTray::onAboutToShowSysTrayMessage()
{
    QTimer::singleShot(400, this, SLOT(hide()));
    QTimer::singleShot(1200, this, SLOT(show()));
}

void DropTargetSysTray::mouseDoubleClickEvent(QMouseEvent* event)
{
    // on winXP tray icons small and DropTarget overlap them.
    // in this case, DropTarget must process context menu and double-click events.
    // in Vista and Win7 this is doesn't need
    if (QSysInfo::windowsVersion() < QSysInfo::WV_VISTA)
    {
        AbstractDropTargetWidget::mouseDoubleClickEvent(event);
    }
}

void DropTargetSysTray::onCustomContextMenuRequested(const QPoint& pos)
{
    // on winXP tray icons small and DropTarget overlap them.
    // in this case, DropTarget must process context menu and double-click events.
    // in Vista and Win7 this is doesn't need
    if (QSysInfo::windowsVersion() < QSysInfo::WV_VISTA)
    {
        AbstractDropTargetWidget::onCustomContextMenuRequested(pos);
    }
}

void DropTargetSysTray::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    //dynamic_cast<ApplicationEx*>(qApp)->setSysTrayWindowTransparent(effectiveWinId());
}


void DropTargetSysTray::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.drawPixmap(rect(), m_bkgPixmap);
}

void DropTargetSysTray::resizeEvent(QResizeEvent* event)
{
    AbstractDropTargetWidget::resizeEvent(event);

    m_bkgPixmap = Graber::grabWindow(effectiveWinId(), false);
    updateRegion(event->size());
}

void DropTargetSysTray::onTimeout()
{
    if (!isVisible())
    {
        return;
    }

    Q_ASSERT(m_sysTrayIcon);
    if (m_sysTrayIcon == NULL)
    {
        return;
    }

    QRect rect;
    if (m_sysTrayIcon)
    {
        rect = m_sysTrayIcon->geometry();
    }

    if (rect.isEmpty())
    {
        hide();
        return;
    }

    m_bkgPixmap = Graber::grabWindow(effectiveWinId(), false);
    setGeometry(rect);
    show();
    raise();
    if (qApp->activeWindow() == NULL)
    {
        activateWindow();
    }
}

void DropTargetSysTray::updateRegion(QSize size)
{
    if (size.isEmpty())
    {
        clearMask();
        return;
    }

    const int delta = 4;

    QRegion maskRegion;

    const int w = size.width();
    const int h = size.height();

    maskRegion = maskRegion.unite(QRegion(0, 0, w, delta));//Top
    maskRegion = maskRegion.unite(QRegion(0, h-delta, w, delta));//Bottom
    maskRegion = maskRegion.unite(QRegion(0, 0, delta, h));//Left
    maskRegion = maskRegion.unite(QRegion(w-delta, 0, delta, h));//Right

    setMask(maskRegion);
}

void DropTargetSysTray::dragEnterEvent(QDragEnterEvent* event)
{
    AbstractDropTargetWidget::dragEnterEvent(event);

    QToolTip::showText(mapToGlobal(event->pos()), tr(""));

    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        updateRegion(QSize());
        QToolTip::showText(mapToGlobal(event->pos()), tr("Upload with SimpleShare"));
    }
}

/*
void DropTargetSysTray::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        QToolTip::showText(mapToGlobal(event->pos()), "");
        QToolTip::showText(mapToGlobal(event->pos()), tr("Share with Box"));
    }
}
*/

void DropTargetSysTray::dragLeaveEvent(QDragLeaveEvent* event)
{
    QToolTip::showText(QPoint(), "");
    updateRegion(size());
}

void DropTargetSysTray::dropEvent(QDropEvent* event)
{
    AbstractDropTargetWidget::dropEvent(event);

    QToolTip::showText(mapToGlobal(event->pos()), "");
    updateRegion(size());
}
