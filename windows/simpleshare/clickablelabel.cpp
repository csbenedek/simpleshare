#include "clickablelabel.h"
#include <QPainter>

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel(parent)
{

}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
    //QLabel::mouseDoubleClickEvent(event);

    emit doubleclicked();
}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    QLabel::mousePressEvent(event);

    emit clicked();
}
