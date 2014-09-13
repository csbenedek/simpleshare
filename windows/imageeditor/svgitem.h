#ifndef GRAPHICSSVGITEM_H
#define GRAPHICSSVGITEM_H

#include "rectitem.h"
#include <QtSvg/QSvgRenderer>

class GraphicsSvgItem : public GraphicsRectItem
{
public:
    enum { Type = UserType + 1009 };

    explicit GraphicsSvgItem(QGraphicsItem* parent = 0);
    explicit GraphicsSvgItem(const QString& fileName, QGraphicsItem* parent = 0);
    explicit GraphicsSvgItem(const QString& fileName, const QRectF& rect, QGraphicsItem* parent = 0);
    explicit GraphicsSvgItem(const QString& fileName, qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent = 0);

    virtual int	type() const;
    bool isShadowEnabled() const;
    void setShadowEnabled(bool shadow);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    QSvgRenderer    m_svg;
    bool            m_shadow;
};

#endif // GRAPHICSSVGITEM_H


