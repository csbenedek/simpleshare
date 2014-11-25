/* editablelabel.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "editablelabel.h"
#include <QPainter>

EditableLabel::EditableLabel(QWidget *parent)
    : QLineEdit(parent)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(endEdit()));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

EditableLabel::EditableLabel(const QString& text, QWidget* parent)
    : QLineEdit(text, parent)
{
    connect(this, SIGNAL(editingFinished()), this, SLOT(endEdit()));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

EditableLabel::~EditableLabel()
{

}

void EditableLabel::onSelectionChanged()
{
    if (isReadOnly())
    {
        setSelection(0,0);
    }
}

void EditableLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
    QLineEdit::mouseDoubleClickEvent(event);

    beginEdit();
}

void EditableLabel::beginEdit()
{
    setReadOnly(false);
    selectAll();
}

void EditableLabel::endEdit()
{
    setReadOnly(true);
}

void EditableLabel::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);

    QPainter painter(this);

    //palette().color(QPalette::Normal, QPalette::Background)
    painter.setPen(QColor(51,51,51));

    QPoint p1 = rect().topLeft();
    QPoint p2 = rect().topRight();

    painter.drawLine(p1, p2);
}
