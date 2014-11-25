/* imageeditorview.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "imageeditorview.h"
#include "qgraphicsview.h"

ImageEditorView::ImageEditorView(QWidget *parent)
    : QGraphicsView(parent)
{

}

ImageEditorView::ImageEditorView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{

}

void ImageEditorView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);

    if (scene())
    {
        scene()->invalidate();
    }
}

void ImageEditorView::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);

    if (scene())
    {
        scene()->invalidate();
    }
}
