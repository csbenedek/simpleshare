#ifndef GRAPHICSSTARITEM_H
#define GRAPHICSSTARITEM_H

#include "rectitem.h"

class GraphicsStarItem : public GraphicsRectItem
{
public:
    enum { Type = UserType + 1010 };

    explicit GraphicsStarItem(QGraphicsItem* parent = 0);
    explicit GraphicsStarItem(const QRectF& rect, QGraphicsItem* parent = 0);
    explicit GraphicsStarItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0);

    virtual int	type() const;
    virtual QPainterPath shape() const;
    bool isShadowEnabled() const;
    void setShadowEnabled(bool shadow = true);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    bool    m_shadow;
};

#endif // GRAPHICSSTARITEM_H


