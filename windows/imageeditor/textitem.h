/* textitem.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef GRAPHICSTEXTITEM_H
#define GRAPHICSTEXTITEM_H

#include "rectitem.h"
#include <QFont>
#include <QString>
#include <QTextEdit>
#include <QGraphicsTextItem>

class GraphicsTextItem: public QGraphicsTextItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 1003 };

    explicit GraphicsTextItem(QGraphicsItem *parent = 0);
    explicit GraphicsTextItem(const QString& text, const QRectF& rect, QGraphicsItem* parent = 0);

    virtual int	type() const;
    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;
    virtual bool contains(const QPointF& point) const;

    QRectF rect() const;
    QRectF realRect() const;
    void setRect(const QRectF& rect);

    QBrush brush() const;
    void setBrush(const QBrush& brush);
    QPen pen() const;
    void setPen(const QPen& pen);
    bool isClip() const;

signals:
    void lostFocus(GraphicsTextItem* item);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual QList<QRectF> selectionMarks() const;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);

private:
    QSizeF      m_size;
    int         m_markerIndex;
    QPointF     m_startPoint;
    QBrush      m_brush;
    QPen        m_pen;
    bool        m_shadow;
    bool        m_clip;
};

#endif // GRAPHICSTEXTITEM_H
