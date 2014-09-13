#ifndef GRAPHICSELLOPSEITEM_H
#define GRAPHICSELLOPSEITEM_H

#include <QGraphicsRectItem>

class GraphicsEllipseItem: public QGraphicsEllipseItem
{
public:
    enum { Type = UserType + 1004 };

    explicit GraphicsEllipseItem(QGraphicsItem* parent = 0);
    explicit GraphicsEllipseItem(const QRectF& rect, QGraphicsItem* parent = 0);
    explicit GraphicsEllipseItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0);

    virtual int	type() const;
    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;

    bool isEmpty() const;
    void setEmpty(bool value = true);

    bool isShadowEnabled() const;
    void setShadowEnabled(bool enabled = true);

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

private:
    int         m_markerIndex;
    QPointF     m_startPoint;
    bool        m_isEmpty;

    bool        m_shadow;
};

#endif //GRAPHICSELLOPSEITEM_H
