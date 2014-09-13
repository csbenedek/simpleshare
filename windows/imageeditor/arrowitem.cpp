#include <QtGui>

#include "arrowitem.h"
#include "itemutils.h"
#include <math.h>

const qreal Pi = 3.14;

GraphicsArrowItem::GraphicsArrowItem(QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_arrowSize(16.0)
    , m_markerIndex(-1)
    , m_shadow(true)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setData(ItemStateResizing, false);
}

GraphicsArrowItem::GraphicsArrowItem(const QLineF& line, QGraphicsItem* parent)
    : QGraphicsLineItem(line, parent)
    , m_arrowSize(16.0)
    , m_markerIndex(-1)
    , m_shadow(true)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setData(ItemStateResizing, false);
}

GraphicsArrowItem::GraphicsArrowItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem* parent)
    : QGraphicsLineItem(x1, y1, x2, y2, parent)
    , m_arrowSize(16.0)
    , m_markerIndex(-1)
    , m_shadow(true)
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setData(ItemStateResizing, false);
}

int GraphicsArrowItem::type() const
{
    return Type;
}

double GraphicsArrowItem::arrowSize() const
{
    return m_arrowSize;
}

void GraphicsArrowItem::setArrowSize(double size)
{
    m_arrowSize = size;
    prepareGeometryChange();
    update();
}

QRectF GraphicsArrowItem::boundingRect() const
{
    qreal extra = (pen().width() + 20 + m_arrowSize) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QList<QRectF> GraphicsArrowItem::selectionMarks() const
{
    // point p2() have priority on p1()
    return QList<QRectF> ()
            << QRectF(line().p2().x() - DEFAULT_MARKER_SIZE/2, line().p2().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(line().p1().x() - DEFAULT_MARKER_SIZE/2, line().p1().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE);
}

QPainterPath GraphicsArrowItem::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(m_arrowHead);

    if (isSelected())
    {
        foreach(QRectF rect, selectionMarks())
        {
            path.addRect(rect);
        }
    }

    return path;
}

void GraphicsArrowItem::paint(QPainter *painter,
    const QStyleOptionGraphicsItem* options,
    QWidget* widget)
{
    Q_UNUSED(widget);

    if (scene())
    {
        painter->setClipping(true);
        painter->setClipRect(mapRectFromScene(scene()->sceneRect()));
    }

    m_arrowHead.clear();

    if (line().length() > 0)
    {
        double angle = ::acos(line().dx() / line().length());
        if (line().dy() >= 0)
        {
            angle = (Pi * 2) - angle;
        }

        QPainter::RenderHints hints = painter->renderHints();
        painter->setRenderHint(QPainter::Antialiasing, true);

        if (arrowSize() > 0.0)
        {
            QPointF arrowP1 = line().p2() + QPointF(sin(angle - Pi / 3) * m_arrowSize,
                                                    cos(angle - Pi / 3) * m_arrowSize);
            QPointF arrowP2 = line().p2() + QPointF(sin(angle - Pi + Pi / 3) * m_arrowSize,
                                                    cos(angle - Pi + Pi / 3) * m_arrowSize);

            m_arrowHead << line().p2() << arrowP1 << arrowP2;
        }
        else
        {
            m_arrowHead.clear();
        }

        QPointF p1 = line().p1();
        QPointF p2 = line().p2() + QPointF(sin(angle - Pi / 2) * m_arrowSize,
                                           cos(angle - Pi / 2) * m_arrowSize);

       if (m_shadow)
       {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QBrush(s_shadowColor));
            painter->translate(s_shadowOffset, s_shadowOffset);
            painter->drawPolygon(m_arrowHead);
            painter->setPen(QPen(s_shadowColor, pen().widthF()));
            painter->drawLine(p1, p2);
            painter->translate(-s_shadowOffset, -s_shadowOffset);
        }

        if (!m_arrowHead.isEmpty())
        {

            painter->setPen(Qt::NoPen);
            painter->setBrush(QBrush(pen().color()));
            painter->drawPolygon(m_arrowHead);
        }


        painter->setPen(pen());
        painter->drawLine(p1, p2);

        painter->setRenderHints(hints);
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

void GraphicsArrowItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (isSelected())
    {
        m_markerIndex = -1;
        QList<QRectF> markers = selectionMarks();
        m_startPoint = event->pos();

        for(int i = 0; i < markers.count(); ++i)
        {
            if (markers.at(i).contains(m_startPoint))
            {
                m_markerIndex = i;
                setData(ItemStateResizing, true);
                event->accept();
                break;
            }
        }
    }

    if (!event->isAccepted())
    {
        QGraphicsItem::mousePressEvent(event);
    }
}

void GraphicsArrowItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_markerIndex != -1)
    {
        m_markerIndex = -1;
        setData(ItemStateResizing, false);
        event->accept();
    }
    else
    {
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

void GraphicsArrowItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_markerIndex != -1)
    {
        QLineF currentLine = line();
        QPointF delta = event->pos() - m_startPoint;
        switch (m_markerIndex)
        {
        case 0:
            currentLine.setP2(currentLine.p2() + delta);
            break;
        case 1:
            currentLine.setP1(currentLine.p1() + delta);
            break;
        }
        prepareGeometryChange();
        setLine(currentLine);
        m_startPoint = event->pos();
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

QVariant GraphicsArrowItem::itemChange(
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

void GraphicsArrowItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
}

void GraphicsArrowItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    unsetCursor();
}

void GraphicsArrowItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected())
    {
         return;
    }

    m_markerIndex = hitTest(selectionMarks(), event->pos(), 2.0);

    switch(m_markerIndex)
    {
    case 0:
    case 1:
        setCursor(Qt::SizeFDiagCursor);
        break;
    default:
        //setCursor(Qt::ArrowCursor);
        unsetCursor();
        break;
    }
}

bool GraphicsArrowItem::isShadowEnabled() const
{
    return m_shadow;
}

void GraphicsArrowItem::setShadowEnabled(bool shadow)
{
    m_shadow = shadow;
    update();
}
