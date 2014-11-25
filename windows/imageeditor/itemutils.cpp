/* itemutils.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "itemutils.h"
#include "utils.h"
#include <QPen>
#include <QPainter>
#include <QDebug>


void drawSelectionMark(QPainter* painter, QRectF rect)
{
    static QPixmap s_pointPixmap(":/res/point_manipulator.png");

    if (s_pointPixmap.isNull())
    {
        QPainter::RenderHints hints = painter->renderHints();
        painter->setRenderHint(QPainter::Antialiasing, false);

        qreal w = qMin(rect.width(), rect.height()) / 5.0;

        static QPen s_pen1(Qt::black, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        static QPen s_pen2(Qt::white, w+1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);

        painter->setPen(s_pen1);
        painter->drawRect(rect);
        rect = deflateRect(rect, 1 + w/2, 1 + w/2);

        painter->setPen(s_pen2);
        painter->drawRect(rect);
        rect = deflateRect(rect, w/2, w/2);

        painter->setPen(s_pen1);
        painter->drawRect(rect);

        painter->setRenderHints(hints);
    }
    else
    {
        QPointF p = rect.topLeft();
        p.setX(rect.left() + (rect.width() - s_pointPixmap.rect().width()) / 2.0);
        p.setY(rect.top() + (rect.height() - s_pointPixmap.rect().height()) / 2.0);
        painter->drawPixmap(p, s_pointPixmap);
    }
}

void drawSelectionMark(QPainter* painter, QPointF pos, qreal size)
{
    QRectF rect (pos.x() - size / 2.0, pos.y() - size / 2.0, size, size);
    drawSelectionMark(painter, rect);
}

int hitTest(const QList<QRectF>& list, QPointF pos, qreal border)
{
    for(int i = 0; i < list.count(); ++i)
    {
        if (deflateRect(list.at(i), border, border).contains(pos))
        {
            return i;
        }
    }
    return -1;
}
