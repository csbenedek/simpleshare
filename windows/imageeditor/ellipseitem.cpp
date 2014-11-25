/* ellipseitem.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "ellipseitem.h"
#include "itemutils.h"
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QPainter>
#include "utils.h"

GraphicsEllipseItem::GraphicsEllipseItem(QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent)
    , m_markerIndex(-1)
    , m_isEmpty(false)
    , m_shadow(true)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

GraphicsEllipseItem::GraphicsEllipseItem(const QRectF& rect, QGraphicsItem *parent)
    : QGraphicsEllipseItem(rect, parent)
    , m_markerIndex(-1)
    , m_isEmpty(false)
    , m_shadow(true)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

GraphicsEllipseItem::GraphicsEllipseItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent)
    : QGraphicsEllipseItem(x, y, width, height, parent)
    , m_markerIndex(-1)
    , m_isEmpty(false)
    , m_shadow(true)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

int GraphicsEllipseItem::type() const
{
    return Type;
}

QList<QRectF> GraphicsEllipseItem::selectionMarks() const
{
    // copypasted from GraphicsRectItem
    return QList<QRectF> ()
            << QRectF(rect().bottomRight().x() - DEFAULT_MARKER_SIZE/2, rect().bottomRight().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().bottomLeft().x() - DEFAULT_MARKER_SIZE/2, rect().bottomLeft().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().topRight().x() - DEFAULT_MARKER_SIZE/2, rect().topRight().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().topLeft().x() - DEFAULT_MARKER_SIZE/2, rect().topLeft().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE);


    /* H V Dots

    qreal x = (rect().right() + rect().left()) / 2.0;
    qreal y = (rect().bottom() + rect().top()) / 2.0;

    return QList<QRectF> ()
            << QRectF(rect().right() - DEFAULT_MARKER_SIZE/2, y - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().left() - DEFAULT_MARKER_SIZE/2, y - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(x - DEFAULT_MARKER_SIZE/2, rect().top() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)            
            << QRectF(x - DEFAULT_MARKER_SIZE/2, rect().bottom() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE);

            */

}

QRectF GraphicsEllipseItem::boundingRect() const
{
    QRectF rect = QGraphicsEllipseItem::boundingRect();

    const QPointF p1 = rect.topLeft() - QPointF(DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE/2);
    const QPointF p2 = rect.bottomRight() + QPointF(DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE/2);

    rect.setTopLeft(p1);
    rect.setBottomRight(p2);

    return rect;
}

QPainterPath GraphicsEllipseItem::shape() const
{
    QPainterPath path = QGraphicsEllipseItem::shape();

    if (isEmpty())
    {
        qreal delta = pen().widthF();
        QPainterPath path2;
        path2.addEllipse(deflateRect(rect(), delta, delta));

        path = path.subtracted(path2);

    }

    if (isSelected())
    {
        foreach(QRectF rect, selectionMarks())
        {
            path.addRect(rect);
        }
    }

    return path;
}

void GraphicsEllipseItem::paint(QPainter *painter,
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

    qreal delta = pen().widthF() / 2.0;

    if (m_shadow)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(s_shadowColor, pen().widthF()));
        QRectF r = deflateRect(rect(), delta, delta);
        r.translate(s_shadowOffset, s_shadowOffset);
        painter->drawEllipse(r);
    }

    if (isEmpty())
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(pen());
        painter->drawEllipse(deflateRect(rect(), delta, delta));
    }
    else
    {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(brush());
        painter->drawEllipse(rect());
        painter->restore();
    }

    if (scene())
    {
        painter->setClipping(false);
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

void GraphicsEllipseItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (isSelected())
    {
        m_markerIndex = -1;
        QList<QRectF> markers = selectionMarks();
        m_startPoint = event->pos();

        m_markerIndex = hitTest(selectionMarks(), event->pos());

        if (m_markerIndex != -1)
        {
            setData(ItemStateResizing, true);
            event->accept();
        }
    }

    if (!event->isAccepted())
    {
        QGraphicsEllipseItem::mousePressEvent(event);
    }
}

void GraphicsEllipseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_markerIndex != -1)
    {
        m_markerIndex = -1;
        setData(ItemStateResizing, false);
        event->accept();
    }
    else
    {
        QGraphicsEllipseItem::mouseReleaseEvent(event);
    }
}

void GraphicsEllipseItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_markerIndex != -1)
    {
        /* H V Dots
        QRectF currentRect = rect();
        QPointF delta = event->pos() - m_startPoint;
        switch (m_markerIndex)
        {
        case 0: // right
            currentRect.setBottomRight(currentRect.bottomRight() + QPointF(delta.x(), 0));
            break;
        case 1: // left
            currentRect.setTopLeft(currentRect.topLeft() + QPointF(delta.x(), 0));
            break;
        case 2: // bottom
            currentRect.setTop(currentRect.top() + delta.y());
            break;
        case 3: // top
            currentRect.setBottom(currentRect.bottom() + delta.y());
            break;
        }
        prepareGeometryChange();
        if (currentRect.left() > currentRect.right())
        {
            m_markerIndex ^= 1;
        }
        if (currentRect.top() > currentRect.bottom())
        {
            if (m_markerIndex == 2)
            {
                m_markerIndex = 3;
            }
            else if (m_markerIndex == 3)
            {
                m_markerIndex = 2;
            }
        }
        setRect(currentRect.normalized());
        m_startPoint = event->pos();
        */
        // Copypasted from GraphicsRectITem
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
        setRect(currentRect.normalized());
        m_startPoint = event->scenePos();
    }
    else
    {
        QGraphicsEllipseItem::mouseMoveEvent(event);
    }
}

QVariant GraphicsEllipseItem::itemChange(
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

void GraphicsEllipseItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
}

void GraphicsEllipseItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    unsetCursor();
}

void GraphicsEllipseItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected())
    {
         return;
    }

    m_markerIndex = hitTest(selectionMarks(), event->pos(), 0.0);

    /* H V Dots
    switch(m_markerIndex)
    {
    case 0:
    case 1:
        setCursor(Qt::SizeHorCursor);
        break;
    case 2:
    case 3:
        setCursor(Qt::SizeVerCursor);
        break;
    default:
        setCursor(Qt::ArrowCursor);
    }
    */

    // copypasted from GraphicsRectItem
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

bool GraphicsEllipseItem::isEmpty() const
{
    return m_isEmpty;
}

void GraphicsEllipseItem::setEmpty(bool value)
{
    m_isEmpty = value;
    if (m_isEmpty)
    {
        setBrush(Qt::NoBrush);
    }
    update();
}

bool GraphicsEllipseItem::isShadowEnabled() const
{
    return m_shadow;
}

void GraphicsEllipseItem::setShadowEnabled(bool enabled)
{
    m_shadow = enabled;
    update();
}
