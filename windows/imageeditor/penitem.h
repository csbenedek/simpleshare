#ifndef GRAPHICSPENITEM_H
#define GRAPHICSPENITEM_H

#include <QGraphicsItem>
#include <QPen>
#include <QRegion>

class GraphicsPenItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 1011 };

    explicit GraphicsPenItem(QGraphicsItem* parent = 0);
    explicit GraphicsPenItem(const QPointF& point, QGraphicsItem* parent = 0);
    virtual ~GraphicsPenItem();

    virtual int	type() const;
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual bool contains(const QPointF& point) const;

    QPen pen() const;
    void setPen(const QPen& pen);
    QRectF rect() const;

    void addPoint(const QPointF& point);
    void finishAddingPoints();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual QList<QRectF> selectionMarks() const;

    void createCachedImage();

private:
    QVector<QPointF>    m_points;
    QRectF              m_rect;
    QPen                m_pen;


    // cache
    QPixmap             m_cachedImage;
    QRegion*             m_maskRegion;
};

#endif // GRAPHICSPENITEM_H


