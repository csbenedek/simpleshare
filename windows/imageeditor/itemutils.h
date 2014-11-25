/* itemutils.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef GRAPHICSITEMUTILS_H
#define GRAPHICSITEMUTILS_H

#include <QGraphicsItem>

static const int ItemStateResizing  = 1;

static const QColor s_shadowColor(0, 0, 0, 127);
static const int s_shadowOffset = 2;
static const int s_selectionColor = Qt::black;


#define DEFAULT_MARKER_SIZE (16.0)

void drawSelectionMark(QPainter* painter, QRectF rect);
void drawSelectionMark(QPainter* painter, QPointF rect, qreal size = DEFAULT_MARKER_SIZE);
int hitTest(const QList<QRectF>& list, QPointF pos, qreal border = 0.0);

#endif // GRAPHICSITEMUTILS_H
