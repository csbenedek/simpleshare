#include "penitem.h"
#include "itemutils.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QBitmap>
#include "utils.h"

GraphicsPenItem::GraphicsPenItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_maskRegion(NULL)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    //setCursor(Qt::ArrowCursor);
}

GraphicsPenItem::GraphicsPenItem(const QPointF& point, QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_maskRegion(NULL)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    addPoint(point);
    //setCursor(Qt::ArrowCursor);
}

GraphicsPenItem::~GraphicsPenItem()
{
    delete m_maskRegion;
}

int GraphicsPenItem::type() const
{
    return Type;
}

QPen GraphicsPenItem::pen() const
{
    return m_pen;
}

void GraphicsPenItem::setPen(const QPen& pen)
{
    m_pen = pen;
    createCachedImage();
    update();
}

void GraphicsPenItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem* options,
                               QWidget* widget)
{
    Q_UNUSED(widget);

    // drawing here
    if (m_points.count() > 0)
    {
        if (scene())
        {
            painter->setClipping(true);
            painter->setClipRect(mapRectFromScene(scene()->sceneRect()));
        }

        painter->setRenderHint(QPainter::Antialiasing, true);

        if (m_cachedImage.isNull())
        {
            painter->setPen(pen());
            if (m_points.count() > 2)
            {
                painter->drawPolyline(m_points.data(), m_points.count());
            }
            if (m_points.count() == 1)
            {
                painter->drawLine(m_points.at(0)-QPointF(0.1,0.1), m_points.at(0)+QPointF(0.1,0.1));
            }
        }
        else
        {
            painter->drawPixmap(qRound(rect().left()), qRound(rect().top()), m_cachedImage);
        }

        if (scene())
        {
            painter->setClipping(false);
        }

        if (options->state & QStyle::State_Selected)
        {
            painter->setRenderHint(QPainter::Antialiasing, false);
            //painter->setPen(s_selectionColor);
            //painter->drawRect(rect());

            foreach(QRectF rect, selectionMarks())
            {
                drawSelectionMark(painter, rect);
            }
        }

    }
}

QRectF GraphicsPenItem::rect() const
{
    qreal delta = pen().widthF() / 2.0;
    return inflateRect(m_rect, delta, delta);
}

QRectF GraphicsPenItem::boundingRect() const
{
    qreal delta = DEFAULT_MARKER_SIZE / 2;
    return inflateRect(rect(), delta, delta);
}

void GraphicsPenItem::finishAddingPoints()
{
    createCachedImage();
}

void GraphicsPenItem::addPoint(const QPointF& point)
{
    QPointF localPoint = mapFromScene(point);

    if (m_points.count() == 0)
    {
        m_rect.setTopLeft(localPoint);
        m_rect.setBottomRight(localPoint);
    }
    if (m_points.count() > 0 && m_points.last() == localPoint)
    {
        return; // already in list
    }

    if (m_rect.left() > localPoint.x())
    {
        prepareGeometryChange();
        m_rect.setLeft(localPoint.x());
    }
    if (m_rect.right() < localPoint.x())
    {
        prepareGeometryChange();
        m_rect.setRight(localPoint.x());
    }
    if (m_rect.top() > localPoint.y())
    {
        prepareGeometryChange();
        m_rect.setTop(localPoint.y());
    }
    if (m_rect.bottom() < localPoint.y())
    {
        prepareGeometryChange();
        m_rect.setBottom(localPoint.y());
    }

    m_points.push_back(localPoint);

    update();
}

void GraphicsPenItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
}

void GraphicsPenItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void GraphicsPenItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

QList<QRectF> GraphicsPenItem::selectionMarks() const
{
    // point p2() have priority on p1()
    return QList<QRectF> ()
            << QRectF(rect().bottomRight().x() - DEFAULT_MARKER_SIZE/2, rect().bottomRight().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().bottomLeft().x() - DEFAULT_MARKER_SIZE/2, rect().bottomLeft().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().topRight().x() - DEFAULT_MARKER_SIZE/2, rect().topRight().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE)
            << QRectF(rect().topLeft().x() - DEFAULT_MARKER_SIZE/2, rect().topLeft().y() - DEFAULT_MARKER_SIZE/2, DEFAULT_MARKER_SIZE, DEFAULT_MARKER_SIZE);

}

bool GraphicsPenItem::contains(const QPointF& point) const
{
    if (m_maskRegion)
    {
        if (m_maskRegion->contains((point - rect().topLeft()).toPoint()))
        {
            return true;
        }
    }
    else
    {
        //TODO: use length to line here.
        qreal delta = pen().widthF() / 2.0;
        QPointF offset = rect().topLeft();

        foreach(const QPointF& p, m_points)
        {
            qreal dist = (p - point).manhattanLength();

            if (dist < delta)
            {
                return true;
            }
        }
    }

    if (isSelected())
    {
        foreach(QRectF rect, selectionMarks())
        {
            if (rect.contains(point))
            {
                return true;
            }
        }
    }

    return false;
}

QPainterPath GraphicsPenItem::shape() const
{
    QPainterPath path = QGraphicsItem::shape();

    if (m_maskRegion)
    {
        path.addRegion(*m_maskRegion);
    }

    /*
    QPainterPath path;
    QPointF delta(pen().widthF() / 2.0, pen().widthF() / 2.0);
    QPointF offset = rect().topLeft();
    foreach(const QPointF& point, m_points)
    {
        path.addEllipse(QRectF(point - delta - offset,
                               point + delta - offset));
    }

    */

    if (isSelected())
    {
        foreach(QRectF rect, selectionMarks())
        {
            path.addRect(rect);
        }
    }

    return path;
}

void GraphicsPenItem::createCachedImage()
{
    if (m_maskRegion)
    {
        delete m_maskRegion;
        m_maskRegion = NULL;
    }
    m_cachedImage = QPixmap();

    if (m_points.count() > 1)
    {
        QSizeF sz = rect().size();
        m_cachedImage = QPixmap(sz.width(), sz.height());

        m_cachedImage.fill(Qt::transparent);
        QPainter painter(&m_cachedImage);

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.translate(-rect().left(), -rect().top());

        painter.setPen(pen());
        if (m_points.count() > 2)
        {
            painter.drawPolyline(m_points.data(), m_points.count());
        }
        if (m_points.count() == 1)
        {
            painter.drawLine(m_points.at(0)-QPointF(0.1,0.1), m_points.at(0)+QPointF(0.1,0.1));
        }

        painter.end();

        m_maskRegion = new QRegion(m_cachedImage.createHeuristicMask());
    }
}
