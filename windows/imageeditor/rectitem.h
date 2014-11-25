/* rectitem.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef GRAPHICSRECTITEM_H
#define GRAPHICSRECTITEM_H

#include <QGraphicsRectItem>
#include "itemutils.h"

class GraphicsRectItem: public QGraphicsRectItem
{
public:
    enum { Type = UserType + 1002 };

    explicit GraphicsRectItem(QGraphicsItem* parent = 0);
    explicit GraphicsRectItem(const QRectF& rect, QGraphicsItem* parent = 0);
    explicit GraphicsRectItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0);

    virtual int	type() const;
    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;

    bool isEmpty() const;
    bool isClip() const;
    void setEmpty(bool value = true);
    bool isShadowEnabled() const;
    void setShadowEnabled(bool shadow);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual QList<QRectF> selectionMarks() const;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

protected:
    int         m_markerIndex;
    QPointF     m_startPoint;
    bool        m_clip;

private:
    bool        m_isEmpty;
    bool        m_shadow;
};

#endif //GRAPHICSRECTITEM_H
