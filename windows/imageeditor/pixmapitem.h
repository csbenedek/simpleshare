/* pixmapitem.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef GRAPHICSPIXMAPITEM_H
#define GRAPHICSPIXMAPITEM_H

#include "rectitem.h"
#include <QBitmap>

class GraphicsPixmapItem : public GraphicsRectItem
{
public:
    enum { Type = UserType + 1009 };

    explicit GraphicsPixmapItem(QGraphicsItem* parent = 0);
    explicit GraphicsPixmapItem(const QPixmap &pixmap, QGraphicsItem* parent = 0);
    explicit GraphicsPixmapItem(const QPixmap &pixmap, const QRectF& rect, QGraphicsItem* parent = 0);
    explicit GraphicsPixmapItem(const QPixmap &pixmap, qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0);

    virtual int	type() const;
    virtual bool contains(const QPointF& point) const;

    bool isShadowEnabled() const;
    void setShadowEnabled(bool shadow);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    QPixmap m_pixmap;
    QBitmap m_mask;
    bool    m_shadow;
};

#endif // GRAPHICSPIXMAPITEM_H


