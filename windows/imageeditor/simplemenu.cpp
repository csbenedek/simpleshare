/* simplemenu.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "simplemenu.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include "simplebutton.h"

// SimpleMenu

SimpleMenu::SimpleMenu(QToolButton* button, QWidget *parent)
    : QMenu(parent)
    , m_button(button)
{
    setCursor(Qt::PointingHandCursor);
    setMouseTracking(true);
}

void SimpleMenu::mouseReleaseEvent(QMouseEvent* e)
{
    if (m_button && m_button->popupMode() == QToolButton::InstantPopup
            && m_button->rect().contains(m_button->mapFromGlobal(e->globalPos())))
    {
        if (m_button->defaultAction())
        {
            m_button->defaultAction()->activate(QAction::Trigger);
        }
    }
    else
    {
        QMenu::mouseReleaseEvent(e);
    }
    hide();
}

// SimpleMenuEx

SimpleMenuEx::SimpleMenuEx(QToolButton* button, QWidget *parent)
    : SimpleMenu(button, parent)
    , m_columnCount(1)
    , m_itemSize(14, 12)
{
    setCursor(Qt::PointingHandCursor);

    setMouseTracking(true);
    //addAction(new QAction());
}

QAction* SimpleMenuEx::actionAt(const QPoint& point) const
{
    if (!rect().contains(point))
    {
        return NULL;
    }

    int x = (point.x() - 1) / m_itemSize.width();
    int y = (point.y() - 1) / m_itemSize.height();

    int index = y*m_columnCount + x;

    if (index >= 0 && index < m_items.count())
    {
        return m_items.at(index);
    }

    return NULL;
}

void SimpleMenuEx::showEvent(QShowEvent* e)
{
    if (isVisible())
    {
        //grabMouse();
    }
    e->accept();
}

void SimpleMenuEx::mouseMoveEvent(QMouseEvent* e)
{
    QAction* action = actionAt(e->pos());
    if (m_currentItem != action)
    {
        m_currentItem = action;
        update();
    }
}

void SimpleMenuEx::mouseReleaseEvent(QMouseEvent* e)
{
    QAction* action = actionAt(e->pos());
    if (action)
    {
        m_currentItem = NULL;
        action->activate(QAction::Trigger);
    }
    else
    {
        if (m_button && m_button->popupMode() == QToolButton::InstantPopup
                && m_button->rect().contains(m_button->mapFromGlobal(e->globalPos())))
        {
            if (m_button->defaultAction())
            {
                m_button->defaultAction()->activate(QAction::Trigger);
            }
        }
    }
    e->accept();
    hide();
}

void SimpleMenuEx::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    static QPen s_pen(Qt::black, 1);

    QPainter painter(this);

    QSize size = m_itemSize;

    int row = 0;

    int x = 1;
    int y = 1;

    foreach(QAction* action, m_items)
    {
        QIcon icon = action->icon();

        if (m_currentItem == action)
        {
            painter.setBrush(QColor(0x1a1a1a));
        }
        else
        {
            painter.setBrush(QColor(0x343434));
        }

        painter.setPen(Qt::NoPen);
        painter.drawRect(x, y, size.width(), size.height());
        painter.setBrush(Qt::NoBrush);

        QPixmap pixmap = icon.pixmap(m_itemSize);
        QSize sz = pixmap.size();

        int posX = x + (size.width() - sz.width()) / 2.0;
        int posY = y + (size.height() - sz.height()) / 2.0;

        painter.drawPixmap(posX, posY, pixmap);

        if (m_currentItem == action)
        {
            painter.setPen(s_pen);
            painter.drawRect(x,y,size.width()-1, size.height()-1);
        }

        x += size.width();
        ++row;
        if (row == m_columnCount)
        {
            row = 0;
            x = 1;
            y += size.height();
        }
    }

    painter.setPen(s_pen);
    painter.drawRect(QRect(0,0, rect().width()-1, rect().height()-1));
}

void SimpleMenuEx::setColumnCount(int count)
{
    Q_ASSERT(m_columnCount > 0);
    m_columnCount = count;

    updateGeometry();
}

int SimpleMenuEx::columnCount() const
{
    return m_columnCount;
}

void SimpleMenuEx::setItemSize(QSize size)
{
    Q_ASSERT(!size.isEmpty());
    m_itemSize = size;

    updateGeometry();
}

QSize SimpleMenuEx::itemSize() const
{
    return m_itemSize;
}

void SimpleMenuEx::updateGeometry()
{
    int w = m_itemSize.width() * m_columnCount + 2;
    int h = m_itemSize.height() * (m_items.count() / m_columnCount) + 2;
    setFixedSize(w, h);
}

QAction* SimpleMenuEx::addAction(const QString &text)
{
    return addAction(QIcon(), text, NULL, NULL, 0);
}

QAction* SimpleMenuEx::addAction(const QIcon &icon, const QString &text)
{
    Q_UNUSED(icon);

    return addAction(QIcon(), text, NULL, NULL, 0);
}

QAction* SimpleMenuEx::addAction(const QString &text, const QObject *receiver, const char* member, const QKeySequence &shortcut)
{
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    Q_UNUSED(shortcut);

    return addAction(QIcon(), text, NULL, NULL, 0);
}

QAction* SimpleMenuEx::addAction(const QIcon &icon, const QString &text, const QObject *receiver, const char* member, const QKeySequence &shortcut)
{
    Q_UNUSED(shortcut);

    QAction* action = new QAction(icon, text, this);
    connect(action, SIGNAL(triggered()), receiver, member);

    m_items.push_back(action);

    return action;
}
