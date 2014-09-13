#include "pixmapitem.h"
#include "itemutils.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QBitmap>
#include <QGraphicsScene>

GraphicsPixmapItem::GraphicsPixmapItem(QGraphicsItem *parent)
    : GraphicsRectItem(parent)
    , m_shadow(false)
{

}

GraphicsPixmapItem::GraphicsPixmapItem(const QPixmap &pixmap, QGraphicsItem *parent)
    : GraphicsRectItem(parent)
    , m_pixmap(pixmap)
    , m_shadow(false)
{
    m_mask = m_pixmap.createHeuristicMask();
}

GraphicsPixmapItem::GraphicsPixmapItem(const QPixmap &pixmap, const QRectF& rect, QGraphicsItem* parent)
    : GraphicsRectItem(rect, parent)
    , m_pixmap(pixmap)
    , m_shadow(false)
{
    m_mask = m_pixmap.createHeuristicMask();
}

GraphicsPixmapItem::GraphicsPixmapItem(const QPixmap &pixmap, qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent)
    : GraphicsRectItem(x, y, width, height, parent)
    , m_pixmap(pixmap)
    , m_shadow(false)
{
    m_mask = m_pixmap.createHeuristicMask();
}

int GraphicsPixmapItem::type() const
{
    return Type;
}

void GraphicsPixmapItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem* options,
                               QWidget* widget)
{
    Q_UNUSED(widget);

    if (rect().isEmpty())
    {
        return;
    }

    if (!m_pixmap.isNull())
    {       
        if (scene())
        {
            painter->setClipping(true);
            painter->setClipRect(mapRectFromScene(scene()->sceneRect()));
        }

        painter->setRenderHint(QPainter::Antialiasing, true);

        if (m_shadow)
        {
            painter->setOpacity(0.5);
            QRectF r = rect();
            r.translate(s_shadowOffset, s_shadowOffset);
            painter->drawPixmap(r, m_mask, m_pixmap.rect());
            painter->setOpacity(1.0);
        }

        {
            painter->drawPixmap(rect(), m_pixmap, m_pixmap.rect());
        }

        if (scene())
        {
            painter->setClipping(false);
        }

    }

    if (options->state & QStyle::State_Selected)
    {
        painter->setRenderHint(QPainter::Antialiasing, false);
        //painter->setPen(Qt::black);
        //painter->drawRect(rect());

        foreach(QRectF rect, selectionMarks())
        {
            drawSelectionMark(painter, rect);
        }
    }
}

bool GraphicsPixmapItem::isShadowEnabled() const
{
    return m_shadow;
}

void GraphicsPixmapItem::setShadowEnabled(bool shadow)
{
    m_shadow = shadow;
    update();
}


bool GraphicsPixmapItem::contains(const QPointF& point) const
{
    return GraphicsRectItem::contains(point);

    if (rect().width() == 0 || rect().height() == 0)
    {
        return false;
    }

    qreal x = point.x() * m_pixmap.width() / rect().width();
    qreal y = point.y() * m_pixmap.height() / rect().height();

    QRegion region(m_mask);

    return region.contains(QPoint(x, y));
}
