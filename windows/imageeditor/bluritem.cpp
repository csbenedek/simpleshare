/* bluritem.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "bluritem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "itemutils.h"
#include "imageeditorscene.h"
#include "utils.h"
#include "qpixmapfilter_p.h"
#include <QDebug>

GraphicsBlurItem::GraphicsBlurItem(QGraphicsItem* parent)
    : GraphicsRectItem(parent)
{

}

GraphicsBlurItem::GraphicsBlurItem(const QRectF& rect, QGraphicsItem* parent)
    : GraphicsRectItem(rect, parent)
{

}

GraphicsBlurItem::GraphicsBlurItem(qreal x, qreal y, qreal width, qreal height, QGraphicsItem* parent)
    : GraphicsRectItem(x, y, width, height, parent)
{

}

int GraphicsBlurItem::type() const
{
    return Type;
}

void GraphicsBlurItem::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem* options,
                             QWidget* widget)
{
    Q_UNUSED(widget);

    if (rect().isEmpty() || rect().width() < 2 || rect().height() < 2)
    {
        return;
    }

    if (scene())
    {
        ImageEditorScene* sc = dynamic_cast<ImageEditorScene*>(scene());
        if (sc)
        {
            QRectF r = mapRectToScene(rect().normalized()).intersect(sc->sceneRect());
            QRectF realRect = r;

            int dx = qRound(r.left()) % 2;
            int dy = r.top() > 0 ? qRound(r.top()) % 2 : 0;

            int dX = qMin(r.left() - sc->sceneRect().left(), 12.0 - dx);
            int dY = qMin(sc->sceneRect().bottom() - r.bottom(), 12.0);

            r.setTop(r.top() - dy);
            r.setLeft(r.left() - dX);
            r.setBottom(r.bottom() + dY);

            QPixmap buff = sc->backgroundPixmap().copy(r.left(), r.top(), r.width(), r.height());
            QPainter buffPainter(&buff);
            buffPainter.setBrush(painter->brush());
            buffPainter.setPen(painter->pen());
            buffPainter.setFont(painter->font());

            foreach(QGraphicsItem* item, sc->items(Qt::AscendingOrder))
            {
                if (item == this)
                {
                    break;
                }
                if (item != this && item->type() != GraphicsBlurItem::Type)
                {
                    QRectF r2 = item->mapRectFromScene(r);
                    if (!r.intersect(item->mapRectToScene(item->boundingRect())).isEmpty())
                    {
                        buffPainter.translate(-r2.left(), -r2.top());
                        buffPainter.setOpacity(item->opacity());
                        QStyleOptionGraphicsItem tempOptions;// = *options;
                        tempOptions.state &= ~(QStyle::State_Selected | QStyle::State_HasFocus);
                        item->paint(&buffPainter, &tempOptions);
                        buffPainter.translate(r2.left(), r2.top());
                        buffPainter.setOpacity(2.0);
                    }
                }
            }

            QPixmapBlurFilter filter;
            filter.setRadius(7.0);
            filter.setBlurHints(QGraphicsBlurEffect::PerformanceHint);

            QPixmap tempBuff(r.width(), r.height());
            QPainter tempPainter(&tempBuff);

            filter.draw(&tempPainter, QPointF(0,0), buff, buff.rect());
            tempPainter.end();

            painter->drawPixmap(mapRectFromScene(realRect), tempBuff, QRectF(dX, dy, tempBuff.width() - dX - 1, tempBuff.height() - dY - dy - 1));
        }
    }

    //painter->drawRect(rect());

    if (options->state & QStyle::State_Selected)
    {
        foreach(QRectF rect, selectionMarks())
        {
            drawSelectionMark(painter, rect);
        }
    }
}

QPainterPath GraphicsBlurItem::opaqueArea() const
{
    return QPainterPath();
}
