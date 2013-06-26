#include "graphicssvgitem.h"
#include "graphicsitemutils.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>

GraphicsSvgItem::GraphicsSvgItem(QGraphicsItem *parent)
    : GraphicsRectItem(parent)
    , m_shadow(true)
{

}

GraphicsSvgItem::GraphicsSvgItem(const QString& fileName, QGraphicsItem *parent)
    : GraphicsRectItem(parent)
    , m_svg(fileName)
    , m_shadow(true)
{

}

GraphicsSvgItem::GraphicsSvgItem(const QString& fileName, const QRectF& rect, QGraphicsItem* parent)
    : GraphicsRectItem(rect, parent)
    , m_svg(fileName)
    , m_shadow(true)
{

}

GraphicsSvgItem::GraphicsSvgItem(const QString& fileName, qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent)
    : GraphicsRectItem(x, y, width, height, parent)
    , m_svg(fileName)
    , m_shadow(true)
{

}

int GraphicsSvgItem::type() const
{
    return Type;
}


void GraphicsSvgItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem* options,
                               QWidget* widget)
{
    Q_UNUSED(widget);

    if (rect().isEmpty())
    {
        return;
    }

    if (scene())
    {
        painter->setClipping(true);
        painter->setClipRect(mapRectFromScene(scene()->sceneRect()));
    }

    if (m_svg.isValid())
    {
        painter->setRenderHint(QPainter::Antialiasing, true);
        m_svg.render(painter, rect());
    }

    if (scene())
    {
        painter->setClipping(false);
    }

    if (options->state & QStyle::State_Selected)
    {
        foreach(QRectF rect, selectionMarks())
        {
            drawSelectionMark(painter, rect);
        }
    }
}


bool GraphicsSvgItem::isShadowEnabled() const
{
    return m_shadow;
}

void GraphicsSvgItem::setShadowEnabled(bool shadow)
{
    m_shadow = shadow;
    update();
}
