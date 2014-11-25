/* abstractdroptargetwidget.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "abstractdroptargetwidget.h"
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QMessageBox>
#include <QTimer>
#include <QMenu>
#include "preferenceswidget.h"

// AbstractDropTargetWidget

AbstractDropTargetWidget::AbstractDropTargetWidget(QWidget *parent)
    : QWidget(parent)
    , m_contextMenu(NULL)
{
    setAcceptDrops(true);

    // on winXP tray icons small and DropTarget overlap them.
    // in this case, DropTarget must process context menu and double-click events.
    // in Vista and Win7 this is doesn't need

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
}

AbstractDropTargetWidget::~AbstractDropTargetWidget()
{

}

void AbstractDropTargetWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    PreferencesWidget* preferencesWidget = qobject_cast<PreferencesWidget*>(parent());
    Q_ASSERT(preferencesWidget);
    if (preferencesWidget)
    {
        preferencesWidget->trayClick(QSystemTrayIcon::DoubleClick);
    }
}

void AbstractDropTargetWidget::onCustomContextMenuRequested(const QPoint& pos)
{
    Q_ASSERT(m_contextMenu);
    if (contextMenu() == NULL)
    {
        return;
    }
    contextMenu()->popup(mapToGlobal(pos));
}

void AbstractDropTargetWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
}

void AbstractDropTargetWidget::dropEvent(QDropEvent* event)
{
    QList<QUrl> urls = event->mimeData()->urls();

    QStringList files;

    foreach(const QUrl& url, urls)
    {
        QString fileName = url.toLocalFile();
        if (!fileName.isNull() && !fileName.isEmpty())
        {
            files << fileName;
        }
    }

    if (!files.isEmpty())
    {
        PreferencesWidget* preferencesWidget = qobject_cast<PreferencesWidget*>(parent());
        Q_ASSERT(preferencesWidget);
        if (preferencesWidget)
        {
            preferencesWidget->uploadFiles(files);
        }
    } 
}

void AbstractDropTargetWidget::setContextMenu(QMenu* menu)
{
    m_contextMenu = menu;
}

QMenu* AbstractDropTargetWidget::contextMenu() const
{
    return m_contextMenu;
}
