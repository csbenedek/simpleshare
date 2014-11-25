/* cropitem.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef GRAPHICSCROPITEM_H
#define GRAPHICSCROPITEM_H

#include "rectitem.h"

class GraphicsCropItem : public GraphicsRectItem
{
public:
    enum { Type = UserType + 1005 };

    explicit GraphicsCropItem(QGraphicsItem* parent = 0);
    explicit GraphicsCropItem(const QRectF& rect, QGraphicsItem* parent = 0);
    explicit GraphicsCropItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0);
    virtual ~GraphicsCropItem();

    virtual int	type() const;
    virtual QRectF boundingRect() const;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    void initColors();

protected:
    bool m_moving;
};

#endif // GRAPHICSCROPITEM_H
