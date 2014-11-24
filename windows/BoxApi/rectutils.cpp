/* rectutils.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */


#include "rectutils.h"
#include <QWidget>

QRect inflateRect(const QRect rect, int dx, int dy)
{
    return QRect(rect.x() - dx,
                 rect.y() - dx,
                 rect.width() + 2*dx,
                 rect.height() + 2*dy);
}

QRect deflateRect(const QRect rect, int dx, int dy)
{
    return QRect(rect.x() + dx,
                 rect.y() + dx,
                 rect.width() - 2*dx,
                 rect.height() - 2*dy);
}

QRectF inflateRect(const QRectF& rect, qreal dx, qreal dy)
{
    return QRectF(rect.x() - dx,
                  rect.y() - dx,
                  rect.width() + 2*dx,
                  rect.height() + 2*dy);
}

QRectF deflateRect(const QRectF& rect, qreal dx, qreal dy)
{
    return QRectF(rect.x() + dx,
                  rect.y() + dx,
                  rect.width() - 2*dx,
                  rect.height() - 2*dy);
}

int alignSize(int size, int delta, int min)
{
    // dimesions should be aligned
    int div = size % delta;
    size -= div;
    size += 8;
    if (size < min)
    {
        size = min;
    }
    return size;
}

void centrateWidget(QWidget *widget)
{
    QDesktopWidget* desktop = QApplication::desktop();
    Q_ASSERT(desktop);
    if (desktop == NULL)
    {
        return;
    }
    QRect rect = desktop->screenGeometry(desktop->primaryScreen());

    int screenWidth = rect.width(); // get width of screen
    int screenHeight = rect.height(); // get height of screen

    QSize windowSize = widget->size(); // size of our application window
    int width = windowSize.width();
    int height = windowSize.height();

    // little computations
    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    // move window to desired coordinates
    widget->move(x, y);
}
