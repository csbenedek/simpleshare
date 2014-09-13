#include "rectitem.h"
#include "itemutils.h"
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QPainter>
#include "utils.h"

GraphicsRectItem::GraphicsRectItem(QGraphicsItem *parent)
    : QGraphicsRectItem(parent)
    , m_markerIndex(-1)
    , m_clip(false)
    , m_isEmpty(false)
    , m_shadow(true)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setData(ItemStateResizing, false);
}

GraphicsRectItem::GraphicsRectItem(const QRectF& rect, QGraphicsItem *parent)
    : QGraphicsRectItem(rect, parent)
    , m_markerIndex(-1)
    , m_clip(false)
    , m_isEmpty(false)
    , m_shadow(true)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setData(ItemStateResizing, false);
}

GraphicsRectItem::GraphicsRectItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, width, height, parent)
    , m_markerIndex(-1)
    , m_clip(false)
    , m_isEmpty(false)
    , m_shadow(true)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setData(ItemStateResizing, false);
}

int GraphicsRectItem::type() const
{
    return Type;
}

QList<QRectF> GraphicsRectItem::selectionMarks() const
{
    // point p2() have priority on p1()
    return QList<QRectF> ()
            << QRectF(rect().bottomRight().x() - DEFAULT_MARKER_SIZE/2, rect().bottomRight().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().bottomLeft().x() - DEFAULT_MARKER_SIZE/2, rect().bottomLeft().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().topRight().x() - DEFAULT_MARKER_SIZE/2, rect().topRight().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().topLeft().x() - DEFAULT_MARKER_SIZE/2, rect().topLeft().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE);

}

QRectF GraphicsRectItem::boundingRect() const
{
    QRectF rect = QGraphicsRectItem::boundingRect();

    const QPointF p1 = rect.topLeft() - QPointF(DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE/2);
    const QPointF p2 = rect.bottomRight() + QPointF(DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE/2);

    rect.setTopLeft(p1);
    rect.setBottomRight(p2);

    return rect;
}

QPainterPath GraphicsRectItem::shape() const
{
    QPainterPath path = QGraphicsRectItem::shape();

    if (isSelected())
    {
        foreach(QRectF rect, selectionMarks())
        {
            QPainterPath path2;
            path2.addRect(rect);

            path = path.united(path2);
        }
    }

    if (isEmpty())
    {
        qreal delta = pen().widthF();

        QPainterPath path2;
        path2.addRect(deflateRect(rect(), delta, delta));

        path = path.subtracted(path2);
    }

    return path;
}

void GraphicsRectItem::paint(QPainter *painter,
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

    painter->setRenderHint(QPainter::Antialiasing, true);

    if (m_shadow)
    {
        QRectF r = rect();
        r.translate(s_shadowOffset, s_shadowOffset);

        if (isEmpty())
        {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(s_shadowColor, pen().widthF(), pen().style(), pen().capStyle(), pen().joinStyle()));
            painter->drawRect(r);
        }
        else
        {
            painter->setBrush(s_shadowColor);
            painter->setPen(Qt::NoPen);
            qreal delta = pen().widthF() / 2.0;
            painter->drawRect(deflateRect(r, delta, delta));
        }

    }

    painter->setBrush(Qt::NoBrush);
    painter->setPen(pen());
    if (isEmpty())
    {
        painter->drawRect(rect());
    }
    else
    {
        qreal delta = pen().widthF() / 2.0;
        painter->fillRect(deflateRect(rect(), delta, delta), brush());
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

void GraphicsRectItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_markerIndex = -1;
    if (isSelected())
    {
        m_startPoint = event->scenePos();

        m_markerIndex = ::hitTest(selectionMarks(), event->pos());
        if (m_markerIndex != -1)
        {
            setData(ItemStateResizing, true);
            event->accept();
        }
    }

    if (!event->isAccepted())
    {
        QGraphicsRectItem::mousePressEvent(event);
    }
}

void GraphicsRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_markerIndex != -1)
    {
        setCursor(Qt::ArrowCursor);
        setData(ItemStateResizing, false);
        m_markerIndex = -1;
        event->accept();
    }
    else
    {
        QGraphicsRectItem::mouseReleaseEvent(event);
    }
}

void GraphicsRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_markerIndex != -1)
    {
        QRectF currentRect = rect();
        QPointF delta = event->scenePos() - m_startPoint;
        switch (m_markerIndex)
        {
        case 0: // bottomRight
            currentRect.setBottomRight(currentRect.bottomRight() + delta);
            break;
        case 1: // bottomLeft
            currentRect.setBottomLeft(currentRect.bottomLeft() + delta);
            break;
        case 2: // topRight
            currentRect.setTopRight(currentRect.topRight() + delta);
            break;
        case 3: // topLeft
            currentRect.setTopLeft(currentRect.topLeft() + delta);
            break;
        }
        prepareGeometryChange();
        if (currentRect.left() > currentRect.right())
        {
            m_markerIndex ^= 1;
        }
        if (currentRect.top() > currentRect.bottom())
        {
            m_markerIndex ^= 2;
        }

        currentRect = currentRect.normalized();
        if (m_clip && scene())
        {
            currentRect = currentRect.intersect(mapRectFromScene(scene()->sceneRect()));
        }
        setRect(currentRect );
        m_startPoint = event->scenePos();
    }
    else
    {
        QGraphicsRectItem::mouseMoveEvent(event);
    }
}

QVariant GraphicsRectItem::itemChange(
    GraphicsItemChange change,
    const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        if (value.toBool())
        {
            setAcceptHoverEvents(true);
        }
        else
        {
            setAcceptHoverEvents(false);
        }
    }
    return value;
}

void GraphicsRectItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
}

void GraphicsRectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    unsetCursor();
}

void GraphicsRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{        
    if (!isSelected())
    {
         return;
    }

    m_markerIndex = ::hitTest(selectionMarks(), event->pos(), 0.0);

    switch(m_markerIndex)
    {
    case 0:
    case 3:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case 1:
    case 2:
        setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        //setCursor(Qt::ArrowCursor);
        unsetCursor();
        break;
    }
}

bool GraphicsRectItem::isEmpty() const
{
    return m_isEmpty;
}

void GraphicsRectItem::setEmpty(bool value)
{
    m_isEmpty = value;
    if (m_isEmpty)
    {
        setBrush(Qt::NoBrush);
    }
    update();
}

bool GraphicsRectItem::isShadowEnabled() const
{
    return m_shadow;
}

void GraphicsRectItem::setShadowEnabled(bool shadow)
{
    m_shadow = shadow;
    update();
}

bool GraphicsRectItem::isClip() const
{
    return m_clip;
}
