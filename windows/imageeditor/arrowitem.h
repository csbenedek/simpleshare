/* arrowitem.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef GRAPHICSARROWITEM_H
#define GRAPHICSARROWITEM_H

#include <QGraphicsLineItem>

class GraphicsArrowItem : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 1001 };

    explicit GraphicsArrowItem(QGraphicsItem* parent = 0);
    explicit GraphicsArrowItem(const QLineF& line, QGraphicsItem* parent = 0);
    explicit GraphicsArrowItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem* parent = 0);

    virtual int type() const;
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;

    double arrowSize() const;
    void setArrowSize(double size);
    bool isShadowEnabled() const;
    void setShadowEnabled(bool enabled = true);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

private:
    QList<QRectF> selectionMarks() const;

private:
    QPolygonF   m_arrowHead;
    double      m_arrowSize;

    int         m_markerIndex;
    QPointF     m_startPoint;

    bool        m_shadow;
};

#endif // GRAPHICSARROWITEM_H
