#include "staritem.h"
#include "itemutils.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <math.h>
#include <QGraphicsScene>

#define M_PI 3.14

GraphicsStarItem::GraphicsStarItem(QGraphicsItem *parent)
    : GraphicsRectItem(parent)
    , m_shadow(true)
{

}

GraphicsStarItem::GraphicsStarItem(const QRectF& rect, QGraphicsItem* parent)
    : GraphicsRectItem(rect, parent)
    , m_shadow(true)
{

}

GraphicsStarItem::GraphicsStarItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent)
    : GraphicsRectItem(x, y, width, height, parent)
    , m_shadow(true)
{

}

int GraphicsStarItem::type() const
{
    return Type;
}

QPainterPath GraphicsStarItem::shape() const
{
    QPolygonF starPolygon;

    qreal delta = pen().widthF() * 2;

    qreal w = rect().width() - pen().widthF()/2;
    qreal h = rect().height() - pen().widthF()/2;

    qreal w1 = rect().width() - pen().widthF()/2;
    qreal h1 = rect().height() - pen().widthF()/2;

    if (isEmpty())
    {
        w1 += 2*delta;
        h1 += 2*delta;
    }

    for (int i = 0; i < 10; ++i)
    {
        qreal a = (0.2 * i - 0.5) * M_PI;
        starPolygon << QPointF(rect().left() + w*0.5 + (i % 2 ? w1*0.2 : w1*0.5) * cos(a),
                               rect().top()  + h*0.55 + (i % 2 ? h1*0.2 : h1*0.5) * sin(a));
    }
    if (isEmpty())
    {
        starPolygon << starPolygon.at(0);
    }

    QPainterPath path;
    path.addPolygon(starPolygon);

    if (isEmpty())
    {
        QPolygonF starPolygon2;

        for (int i = 0; i < 10; ++i)
        {
            qreal w2 = w - 2*delta;
            qreal h2 = h - 2*delta;
            qreal a = (0.2 * i - 0.5) * M_PI;
            starPolygon2 << QPointF(rect().left() + w*0.5 + (i % 2 ? w2*0.2 : w2*0.5) * cos(a),
                                    rect().top()  + h*0.55 + (i % 2 ? h2*0.2 : h2*0.5) * sin(a));
        }
        starPolygon2 << starPolygon2.at(0);

        QPainterPath path2;
        path2.addPolygon(starPolygon2);

        path = path.subtracted(path2);
    }

    if (isSelected())
    {
        foreach(QRectF rect, selectionMarks())
        {
            path.addRect(rect);
        }
    }

    return path;
}

void GraphicsStarItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem* options,
                               QWidget* widget)
{
    Q_UNUSED(widget);

    if (rect().isEmpty())
    {
        return;
    }

    painter->save();

    if (scene())
    {
        painter->setClipping(true);
        painter->setClipRect(mapRectFromScene(scene()->sceneRect()));
    }

    painter->setRenderHint(QPainter::Antialiasing, true);

    QPolygonF starPolygon;
    qreal w = rect().width() - pen().widthF()/2;
    qreal h = rect().height() - pen().widthF()/2;
    for (int i = 0; i < 10; ++i)
    {
        qreal a = (0.2 * i - 0.5) * M_PI;
        starPolygon << QPointF(rect().left() + w*0.5 + (i % 2 ? w*0.2 : w*0.5) * cos(a),
                               rect().top()  + h*0.55 + (i % 2 ? h*0.2 : h*0.5) * sin(a));
    }
    if (isEmpty())
    {
        starPolygon << starPolygon.at(0);
    }

    if (!isEmpty())
    {
        if (m_shadow)
        {
            QPen shadowPen(pen());
            shadowPen.setColor(s_shadowColor);
            painter->setPen(shadowPen);
            painter->setBrush(s_shadowColor);
            painter->translate(s_shadowOffset, s_shadowOffset);
            painter->drawPolygon(starPolygon, Qt::WindingFill);
            painter->translate(-s_shadowOffset, -s_shadowOffset);
        }
        painter->setPen(pen());
        painter->setBrush(brush());
        painter->drawPolygon(starPolygon, Qt::WindingFill);
    }
    else
    {
        if (m_shadow)
        {
            QPen shadowPen(pen());
            shadowPen.setColor(s_shadowColor);
            painter->setPen(shadowPen);
            painter->translate(s_shadowOffset, s_shadowOffset);
            painter->drawPolyline(starPolygon);
            painter->translate(-s_shadowOffset, -s_shadowOffset);
        }
        painter->setPen(pen());
        painter->drawPolyline(starPolygon);
    }

    if (scene())
    {
        painter->setClipping(false);
    }

    painter->restore();

    if (options->state & QStyle::State_Selected)
    {
        painter->setRenderHint(QPainter::Antialiasing, false);

        //painter->setPen(Qt::black);
        //painter->drawRect(rect());

        foreach(QRectF rect, selectionMarks())
        {
            drawSelectionMark(painter, rect);
        }
    }
}

bool GraphicsStarItem::isShadowEnabled() const
{
    return m_shadow;
}

void GraphicsStarItem::setShadowEnabled(bool shadow)
{
    m_shadow = shadow;
    update();
}
