#include "cropitem.h"
#include "itemutils.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include "utils.h"

GraphicsCropItem::GraphicsCropItem(QGraphicsItem *parent)
    : GraphicsRectItem(parent)
    , m_moving(false)
{
    m_clip = true;
    setFlag(QGraphicsItem::ItemClipsToShape, false);
    initColors();
}

GraphicsCropItem::GraphicsCropItem(const QRectF& rect, QGraphicsItem *parent)
    : GraphicsRectItem(rect, parent)
    , m_moving(false)
{
    m_clip = true;
    setFlag(QGraphicsItem::ItemClipsToShape, false);
    initColors();
}

GraphicsCropItem::GraphicsCropItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent)
    : GraphicsRectItem(x, y, width, height, parent)
    , m_moving(false)
{
    m_clip = true;
    setFlag(QGraphicsItem::ItemClipsToShape, false);
    initColors();
}

GraphicsCropItem::~GraphicsCropItem()
{
    ungrabMouse();
}

void GraphicsCropItem::initColors()
{
    setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    setBrush(Qt::black);
}

int GraphicsCropItem::type() const
{
    return Type;
}
void GraphicsCropItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem* options,
                             QWidget* widget)
{
    Q_UNUSED(widget);

    QColor color = brush().color();
    color.setAlpha(127);

    QRectF r = scene()->sceneRect();

    if (r.top() < rect().left())
    {
        painter->fillRect(QRectF(
                         r.topLeft(),
                         QSizeF(qMin(r.right(), rect().left()), r.bottom()-1)), color);
    }
    if (rect().top() > r.top() && rect().left() < r.right())
    {
        painter->fillRect(QRectF(
                             QPointF(qMin(r.right(), rect().left()), r.top()),
                             QSizeF(r.right() - rect().left() - 1, rect().top() - r.top())), color);
    }

    if (r.bottom() > rect().bottom() && r.right() > rect().left())
    {
       painter->fillRect(QRectF(
                         QPointF(qMin(r.right(), rect().left()), qMin(r.bottom(), rect().bottom())),
                         QSizeF(r.right() - rect().left()-1, r.bottom() - rect().bottom()-1)), color);
    }

    if (r.right() > rect().right())
    {
        painter->fillRect(QRectF(
                     rect().topRight(),
                     QSizeF(r.right() - rect().right()-1, qMin(rect().height(), r.bottom()-1))), color);
    }

    painter->setRenderHint(QPainter::Antialiasing, false);
    QPen pen1(Qt::white, 1);
    painter->setPen(pen1);
    painter->drawRect(deflateRect(rect(), 1, 1));
    painter->drawRect(deflateRect(rect(), 2, 2));
    QRectF bounds = rect();
    QPen pen(Qt::black, 1);
    painter->setPen(pen);
    painter->drawRect(bounds);
    int count = 4;
    for(int i = count; i >= 0; --i)
    {
        color.setAlpha(qRound(i*64/count));
        bounds = inflateRect(bounds, 1, 1);
        QPen pen(color, 1);
        painter->setPen(pen);
        painter->drawRect(bounds);
    }

    if (options->state & QStyle::State_Selected)
    {
        foreach(QRectF rect, selectionMarks())
        {
            drawSelectionMark(painter, rect);
        }
    }
}

QRectF GraphicsCropItem::boundingRect() const
{
    QRectF r = GraphicsRectItem::boundingRect();
    if (scene())
    {
        QRectF rect = scene()->sceneRect();
        rect.unite(r);
        return rect;
    }
    else
    {
        return GraphicsRectItem::boundingRect();
    }
}

