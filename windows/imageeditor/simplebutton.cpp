/* simplebutton.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "simplebutton.h"
#include <QPainter>
#include <QPaintEvent>
#include <QIcon>
#include <QAction>
#include <QMenu>

SimpleButton::SimpleButton(QWidget *parent)
    : QToolButton(parent)
    , m_currentTool(false)
{

}

SimpleButton::~SimpleButton()
{

}

bool SimpleButton::isCurrentTool() const
{
    return m_currentTool;
}

void SimpleButton::setCurrentTool(bool current)
{
    m_currentTool = current;
    update();
}

void SimpleButton::paintEvent(QPaintEvent* event)
{
    //QToolButton::paintEvent(event);

    QPainter painter(this);

    if (isChecked() || isDown()
            || isCurrentTool()
            || (menu() && menu()->isVisible()))
    {
        painter.setBrush(Qt::black);
        painter.setOpacity(0.8);
        painter.drawRect(rect());
        painter.setOpacity(1.0);

        painter.setPen(Qt::black);
        painter.drawLine(rect().topLeft(), rect().topRight());
        painter.drawLine(rect().topLeft(), rect().bottomLeft());
        //painter.setPen(Qt::darkGray);
        //painter.drawLine(rect().bottomLeft(), rect().bottomRight());
        //painter.drawLine(rect().topRight(), rect().bottomRight());
    }
    else
    {
        QRect globalRect(mapToGlobal(rect().topLeft()), mapToGlobal(rect().bottomRight()));
        if (globalRect.contains(QCursor::pos())) // TODO: any easy way to find that mouse hovered
        {
            painter.setPen(Qt::darkGray);
            painter.drawLine(rect().topLeft(), rect().topRight());
            painter.drawLine(rect().topLeft(), rect().bottomLeft());
            painter.setPen(Qt::black);
            painter.drawLine(rect().bottomLeft(), rect().bottomRight());
            painter.drawLine(rect().topRight(), rect().bottomRight());
        }
    }

    QIcon img = icon();
    QPixmap pixmap = img.pixmap(size());
    if (!pixmap.isNull())
    {

        int x = (rect().width() - pixmap.width()) / 2;
        int y = (rect().height() - pixmap.height()) / 2;

        painter.drawPixmap(x, y, pixmap);

    }
    else
    {
        painter.setPen(Qt::white);
        painter.setFont(font());
        painter.drawText(rect(), Qt::AlignCenter, text());
    }

    if (menu())
    {
        static QPixmap s_dropdown(":/res/22x22_dropdown_arrow.png");

        int x = rect().width() - s_dropdown.width();
        int y = rect().height() - s_dropdown.height();

        painter.drawPixmap(x, y, s_dropdown);
    }

    event->accept();
}
