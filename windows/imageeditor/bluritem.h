/* bluritem.h
 * fixup for invalid blur function in Qt.
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef GRAPHICSBLURITEM_H
#define GRAPHICSBLURITEM_H

#include "rectitem.h"
#include <QGraphicsBlurEffect>

class GraphicsBlurItem : public GraphicsRectItem
{
public:
    enum { Type = UserType + 1006 };

    explicit GraphicsBlurItem(QGraphicsItem* parent = 0);
    explicit GraphicsBlurItem(const QRectF& rect, QGraphicsItem* parent = 0);
    explicit GraphicsBlurItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0);

    virtual int	type() const;
    virtual QPainterPath opaqueArea() const;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    QGraphicsBlurEffect*  m_blurEffect;
};

#endif // GRAPHICSBLURITEM_H
