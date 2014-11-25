/* abstractdroptargetwidget.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef ABSTRACTDROPTARGETWIDGET_H
#define ABSTRACTDROPTARGETWIDGET_H

#include <QWidget>
#include <QMenu>

class AbstractDropTargetWidget: public QWidget
{
    Q_OBJECT
public:
    explicit AbstractDropTargetWidget(QWidget* parent = 0);
    ~AbstractDropTargetWidget();

    void setContextMenu(QMenu* menu);
    QMenu* contextMenu() const;

protected:
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dropEvent(QDropEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

protected slots:
    void onCustomContextMenuRequested(const QPoint& pos);

private:
    QMenu*  m_contextMenu;
};

#endif // ABSTRACTDROPTARGETWIDGET_H
