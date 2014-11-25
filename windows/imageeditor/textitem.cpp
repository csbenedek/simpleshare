/* textitem.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "textitem.h"
#include "itemutils.h"
#include <QtGui>
#include "utils.h"

GraphicsTextItem::GraphicsTextItem(QGraphicsItem *parent)
    : QGraphicsTextItem(parent)
    , m_shadow(false)
    , m_clip(false)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);   
}

GraphicsTextItem::GraphicsTextItem(const QString& text, const QRectF& rect, QGraphicsItem *parent)
    : QGraphicsTextItem(text, parent)
    , m_shadow(false)
    , m_clip(false)
{
    setRect(rect);
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
}

int GraphicsTextItem::type() const
{
    return Type;
}

void GraphicsTextItem::focusOutEvent(QFocusEvent *event)
{
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    setTextCursor(cursor);

    setTextInteractionFlags(Qt::NoTextInteraction);
    emit lostFocus(this);
    QGraphicsTextItem::focusOutEvent(event);
}

void GraphicsTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsTextItem::mouseDoubleClickEvent(event);

    if (textInteractionFlags() == Qt::NoTextInteraction)
    {
        scene()->clearSelection();
        setSelected(true);

        setTextInteractionFlags(Qt::TextEditorInteraction);
        setFocus();

        QTextCursor cursor = textCursor();
        cursor.select(QTextCursor::Document);
        setTextCursor(cursor);

    }
    event->accept();
}

void GraphicsTextItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem* options,
                             QWidget* widget)
{
    if (rect().isEmpty())
    {
        return;
    }

    QStyleOptionGraphicsItem tempOptions = *options;
    tempOptions.state &= ~(QStyle::State_Selected | QStyle::State_HasFocus);

    QRectF rect = QGraphicsTextItem::boundingRect();
    rect.setWidth(m_size.width());
    rect.setHeight(m_size.height());

    painter->save();

    if (scene())
    {
        painter->setClipping(true);
        painter->setClipRect(mapRectFromScene(scene()->sceneRect()));
    }

    painter->setBrush(brush());
    painter->setPen(QPen(pen()));

    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundedRect(rect, 5.0, 5.0);

    /*
    if (m_shadow)
    {
        QRectF r = rect;
        r.translate(s_shadowOffset, s_shadowOffset);
        painter->setBrush(s_shadowColor);
        painter->drawRoundedRect(rect, 4.0, 4.0);
    }
    */

    painter->setClipRect(rect.intersect(mapRectFromScene(scene()->sceneRect())));
    QGraphicsTextItem::paint(painter, &tempOptions, widget);

    painter->restore();

    if (scene())
    {
        painter->setClipping(false);
    }

    if (options->state & (QStyle::State_Selected | QStyle::State_HasFocus))
    {
        foreach(QRectF rect, selectionMarks())
        {
            drawSelectionMark(painter, rect);
        }
    }
}

QRectF GraphicsTextItem::rect() const
{
    QRectF rect = QGraphicsTextItem::boundingRect();
    rect.setWidth(m_size.width());
    rect.setHeight(m_size.height());
    return rect;
}

QRectF GraphicsTextItem::realRect() const
{
    QRectF rect;
    rect.setTopLeft(pos());
    rect.setWidth(m_size.width());
    rect.setHeight(m_size.height());
    return rect;
}

void GraphicsTextItem::setRect(const QRectF& rect)
{
    m_size = rect.size();
    setPos(rect.topLeft());
    setTextWidth(m_size.width());
    update();
}


QList<QRectF> GraphicsTextItem::selectionMarks() const
{
    return QList<QRectF> ()
            << QRectF(rect().bottomRight().x() - DEFAULT_MARKER_SIZE/2, rect().bottomRight().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().bottomLeft().x() - DEFAULT_MARKER_SIZE/2, rect().bottomLeft().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().topRight().x() - DEFAULT_MARKER_SIZE/2, rect().topRight().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().topLeft().x() - DEFAULT_MARKER_SIZE/2, rect().topLeft().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE);

}

QRectF GraphicsTextItem::boundingRect() const
{
    return inflateRect(rect(), DEFAULT_MARKER_SIZE/2.0 + 4, DEFAULT_MARKER_SIZE/2.0 + 4);
}

QPainterPath GraphicsTextItem::shape() const
{
    QPainterPath path;// = QGraphicsTextItem::shape();

    path.addRect(rect());

    if (isSelected())
    {
        foreach(QRectF rect, selectionMarks())
        {
            path.addRect(rect);
        }
    }

    return path;
}

bool GraphicsTextItem::contains(const QPointF& point) const
{
    bool result = QGraphicsTextItem::contains(point);
    if (!result)
    {
        result = boundingRect().contains(point);
    }
    return result;
}

void GraphicsTextItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (isSelected())
    {
        m_markerIndex = -1;
        QList<QRectF> markers = selectionMarks();
        m_startPoint = event->scenePos();

        m_markerIndex = hitTest(selectionMarks(), event->pos());

        if (m_markerIndex != -1)
        {
            setData(ItemStateResizing, true);
            event->accept();
        }
    }

    if (!event->isAccepted())
    {
        QGraphicsTextItem::mousePressEvent(event);
    }
}

void GraphicsTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_markerIndex != -1)
    {
        m_markerIndex = -1;
        setData(ItemStateResizing, false);
        event->accept();
    }
    else
    {
        QGraphicsTextItem::mouseReleaseEvent(event);
    }
}

void GraphicsTextItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_markerIndex != -1)
    {
        QRectF currentRect = QRectF(pos(), m_size);
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
        setRect(currentRect);
        m_startPoint = event->scenePos();
    }
    else
    {
        QGraphicsTextItem::mouseMoveEvent(event);
    }
}

QVariant GraphicsTextItem::itemChange(
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

void GraphicsTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
}

void GraphicsTextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    unsetCursor();
}

void GraphicsTextItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected())
    {
         return;
    }

    m_markerIndex = hitTest(selectionMarks(), event->pos(), 0.0);

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
        unsetCursor();
        //setCursor(Qt::ArrowCursor);
        break;
    }
}

QBrush GraphicsTextItem::brush() const
{
    return m_brush;
}

void GraphicsTextItem::setBrush(const QBrush& brush)
{
    m_brush = brush;
    update();
}

QPen GraphicsTextItem::pen() const
{
    return m_pen;
}

void GraphicsTextItem::setPen(const QPen& pen)
{
    m_pen = pen;
    update();
}

bool GraphicsTextItem::isClip() const
{
    return m_clip;
}
