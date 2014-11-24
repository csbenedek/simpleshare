/* rectutils.h
 * Rect helpers and widget centrate helper
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */


#ifndef RECTUTILS_H
#define RECTUTILS_H

#include <QRect>

class QWidget;

QRect  inflateRect(const QRect   rect, int   dx, int   dy);
QRect  deflateRect(const QRect   rect, int   dx, int   dy);
QRectF inflateRect(const QRectF& rect, qreal dx, qreal dy);
QRectF deflateRect(const QRectF& rect, qreal dx, qreal dy);

int    alignSize(int size, int delta, int min);

// Widget helpers

void centrateWidget(QWidget* widget);

#endif // RECTUTILS_H
