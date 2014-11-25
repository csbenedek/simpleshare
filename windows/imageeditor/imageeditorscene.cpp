/* imageeditorscene.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include <QtGui>

#include "imageeditorscene.h"
#include "arrowitem.h"
#include "textitem.h"
#include "rectitem.h"
#include "ellipseitem.h"
#include "cropitem.h"
#include "bluritem.h"
#include "itemutils.h"
#include "pixmapitem.h"
#include "svgitem.h"
#include "staritem.h"
#include "penitem.h"
#include "utils.h"

ImageEditorScene::ImageEditorScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_newItem(NULL)
    , m_pressedItem(NULL)
    , m_pressedItemWasSelected(false)
    , m_number(0)
    , m_multiSelection(false)
    , m_blockSelectionModifications(false)
    , m_selectOnPress(true) // selection mode.
{
    setMode(Select);
    m_textColor = QColor(0xACC800);
    m_bgColor   = QColor(0xACC800);
    m_fgColor   = QColor(0xACC800);
    m_lineWidth = 4;
    m_penWidth  = 26;
    m_font.setFamily("Arial");
    m_font.setPointSize(12);
    //m_font.setBold(true);
}

QFont ImageEditorScene::font() const
{
    return m_font;
}

QColor ImageEditorScene::textColor() const
{
    return m_textColor;
}

QColor ImageEditorScene::bgColor() const
{
    return m_bgColor;
}

QColor ImageEditorScene::fgColor() const
{
    return m_fgColor;
}

int ImageEditorScene::lineWidth() const
{
    return m_lineWidth;
}

void ImageEditorScene::setTextColor(const QColor &color)
{
    cancelCrop();
    m_textColor = color;
    selectedItemsModified(false, false, true);
}

void ImageEditorScene::setLineWidth(int lineWidth)
{
    cancelCrop();
    m_lineWidth = lineWidth;
    selectedItemsModified(true, false, false);
}

void ImageEditorScene::setFgColor(const QColor &color)
{
    cancelCrop();
    m_fgColor = color;
    selectedItemsModified(true, false, false);
}

void ImageEditorScene::setBgColor(const QColor &color)
{
    cancelCrop();
    m_bgColor = color;
    selectedItemsModified(false, true, false);
}

void ImageEditorScene::setFont(const QFont &font)
{
    cancelCrop();
    m_font = font;
    selectedItemsModified(false, false, true);
}

bool ImageEditorScene::isSelectionModificationsBlocked() const
{
    return m_blockSelectionModifications;
}

void ImageEditorScene::blockSelectionModifications(bool block)
{
    m_blockSelectionModifications = block;
}

void ImageEditorScene::selectedItemsModified(bool penChanged, bool brushChanged, bool fontChanged)
{
    if (m_blockSelectionModifications)
    {
        return;
    }

    // update selected items:
    foreach(QGraphicsItem* item, selectedItems())
    {
        if (penChanged || brushChanged)
        {
            QAbstractGraphicsShapeItem* shapeItem = dynamic_cast<QAbstractGraphicsShapeItem*>(item);
            if (shapeItem != NULL)
            {
                if (penChanged)
                {
                    if (shapeItem->pen() != Qt::NoPen)
                    {
                        shapeItem->setPen(QPen(m_fgColor, m_lineWidth,
                                               shapeItem->pen().style(), shapeItem->pen().capStyle(), shapeItem->pen().joinStyle()));
                    }
                }

                if (brushChanged)
                {
                    if (shapeItem->brush() != Qt::NoBrush)
                    {
                        shapeItem->setBrush(bgColor());
                        //shapeItem->setPen(QPen(m_bgColor, 1));
                    }
                    break;
                }
            }

            if (item->type() == GraphicsArrowItem::Type && penChanged)
            {
                GraphicsArrowItem* arrowItem = qgraphicsitem_cast<GraphicsArrowItem*>(item);
                if (arrowItem->arrowSize() > 0)
                {
                    arrowItem->setArrowSize(m_lineWidth * 3);
                }

                arrowItem->setPen(QPen(m_fgColor, m_lineWidth,
                                       arrowItem->pen().style(), arrowItem->pen().capStyle(), arrowItem->pen().joinStyle()));
            }

            if (item->type() == GraphicsPenItem::Type && penChanged)
            {
                GraphicsPenItem* penItem = qgraphicsitem_cast<GraphicsPenItem*>(item);
                QColor fgColor = m_fgColor;
                fgColor.setAlpha(100);
                penItem->setPen(QPen(fgColor, m_penWidth,
                                     penItem->pen().style(), penItem->pen().capStyle(), penItem->pen().joinStyle()));
            }

        }

        if (fontChanged)
        {
            if (item->type() == GraphicsTextItem::Type)
            {
                GraphicsTextItem* textItem = qgraphicsitem_cast<GraphicsTextItem*>(item);
                if (textItem != NULL)
                {
                    //textItem->setPen(m_pen);
                    //textItem->setBrush(m_brush);
                    textItem->setFont(m_font);
                    textItem->setDefaultTextColor(m_textColor);
                }
            }
        }
    }
}

void ImageEditorScene::editorLostFocus(GraphicsTextItem* item)
{
    Q_UNUSED(item);

}


void ImageEditorScene::setMode(Mode mode)
{
    //Q_ASSERT(m_newItem == NULL);
    if (mode == Select)
    {
        //clearSelection();
    }
    else
    {
        cancelCrop();
    }    

    m_mode = mode;
    emit modeChanged(m_mode);
}

QGraphicsItem* ImageEditorScene::createNewItem()
{
    QGraphicsItem* result = NULL;
    Q_ASSERT(m_newItem == NULL);

    switch(m_mode)
    {
    case ItemPen:
        {
            GraphicsPenItem* item = new GraphicsPenItem(m_startPoint);
            //item->setCursor(views().at(0)->cursor());
            QColor fgColor = m_fgColor;
            fgColor.setAlpha(100);
            item->setPen(QPen(fgColor, m_penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            result = item;
        }
        break;
    case ItemRect:
    case ItemEmptyRect:
        {
            GraphicsRectItem* item = new GraphicsRectItem(QRectF(m_startPoint, m_currentPoint).normalized());
            item->setPen(QPen(m_fgColor, m_lineWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
            if (m_mode == ItemEmptyRect)
            {
                item->setEmpty(true);
            }
            else
            {
                item->setBrush(m_bgColor);
            }
            result = item;
        }
        break;
    //case ItemHighlight:
    //    {
    //       GraphicsHighlightItem* item = new GraphicsHighlightItem(QRectF(m_startPoint, m_currentPoint).normalized());
    //       item->setPen(Qt::NoPen);
    //       item->setBrush(bgColor());
    //       result = item;
    //   }
    //   break;
    case ItemEllipse:
    case ItemEmptyEllipse:
        {
            GraphicsEllipseItem* item = new GraphicsEllipseItem(QRectF(m_startPoint, m_currentPoint).normalized());
            item->setPen(QPen(m_fgColor, m_lineWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
            if (m_mode == ItemEmptyEllipse)
            {
                item->setEmpty(true);
            }
            else
            {
                item->setBrush(m_bgColor);
            }
            result = item;
        }
        break;
    case ItemStar:
    case ItemEmptyStar:
        {
            GraphicsStarItem* item = new GraphicsStarItem(QRectF(m_startPoint, m_currentPoint).normalized());
            item->setPen(QPen(m_fgColor, m_lineWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
            if (m_mode == ItemEmptyStar)
            {
                item->setEmpty(true);
            }
            else
            {
                item->setBrush(m_bgColor);
            }
            result = item;
        }
        break;
    //case ItemNumber:
    //    {
    //        GraphicsNumberItem* item = new GraphicsNumberItem(QRectF(m_startPoint, m_currentPoint).normalized());
    //        //item->setFont(m_font);
    //        item->setPen(QPen(m_fgColor, m_lineWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    //        QColor bgColor = m_bgColor;
    //        bgColor.setAlpha(80);
    //        item->setBrush(QBrush(bgColor));
    //        item->setNumber(++m_number);
    //        result = item;
    //    }
    //    break;
    case ItemLine:
        {
            GraphicsArrowItem* item = new GraphicsArrowItem(QLineF(m_startPoint, m_currentPoint));
            item->setPen(QPen(m_fgColor, m_lineWidth));
            item->setArrowSize(0.0);
            result = item;
        }
        break;
    case ItemArrow:
        {
            GraphicsArrowItem* item = new GraphicsArrowItem(QLineF(m_startPoint, m_currentPoint));
            item->setPen(QPen(m_fgColor, m_lineWidth));
            item->setArrowSize(m_lineWidth * 3);
            result = item;
        }
        break;
    case ItemImage:
        {
            if (!m_itemPixmap.isNull())
            {
                result = new GraphicsPixmapItem(m_itemPixmap, QRectF(m_startPoint, m_currentPoint).normalized());
            }
            else
            {
                //result = new GraphicsSvgItem(m_itemFileName, QRectF(m_startPoint, m_currentPoint).normalized());
            }
        }
        break;
    case ItemBlur:
        {
            GraphicsBlurItem* item = new GraphicsBlurItem(QRectF(m_startPoint, m_currentPoint).normalized());
            //item->setPen(QPen(Qt::black, 1));
            result = item;
        }
        break;
    case ItemText:
        {
            GraphicsTextItem* item = new GraphicsTextItem(tr("Double click for edit text here ..."), QRectF(m_startPoint, m_currentPoint).normalized());
            if (item)
            {
                //item->setPen(QPen(m_textColor, 1.0));
                connect(item, SIGNAL(lostFocus(GraphicsTextItem*)), this, SLOT(editorLostFocus(GraphicsTextItem*)));
                //item->setTextInteractionFlags(Qt::TextEditorInteraction);
                item->setFont(m_font);
                item->setPen(Qt::NoPen);
                item->setBrush(QColor(0, 0, 0, 203));
                item->setDefaultTextColor(m_textColor);
                //item->setFocus();
                item->setPos(m_startPoint);
            }

            result = item;
        }
        break;
    case Crop:
        {
            clearSelection();
            GraphicsCropItem* item = new GraphicsCropItem(QRectF(m_startPoint, m_currentPoint).normalized());
            item->setSelected(true);
            result = item;
            if (result)
            {
                result->setZValue(100.0);
            }
            invalidate(); // update all
        }
        break;
    default:
        break;
    }

    if (result)
    {
        if (m_mode != Crop)
        {
            clearSelection(); // if somebody already selected it can overlap new item. deselect it
        }        
        addItem(result);
        bringToFront(result);
        emit newItemCreated(result);
    }

    return result;
}

void ImageEditorScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (isCropping())
    {
        doCrop();
        event->accept();
        return;
    }

    QGraphicsScene::mouseDoubleClickEvent(event);

    if (!event->isAccepted() && event->button() == Qt::LeftButton &&
            (m_mode > Select && m_mode < Crop && m_mode != ItemPen))
    {
        QSize size(40, 40);

        if (m_mode == ItemStar || m_mode == ItemEmptyStar)
        {
            size = QSize(25, 25);
        }
        else if (m_mode == ItemLine || m_mode == ItemArrow)
        {
            size = QSize(150, 150);
        }
        else if (m_mode == ItemText)
        {
            size = QSize(150, 80);
        }
        else if (m_mode == ItemImage)
        {
            if (!m_itemPixmap.isNull())
            {
                size = m_itemPixmap.size();
            }
            else
            {
                //QSvgRenderer svg(m_itemFileName);
                //size = svg.defaultSize();
            }
        }
        //else if (m_mode == ItemNumber)
        //{
        //    size = QSize(20, 20);
        //}

        m_startPoint = event->scenePos() - QPointF(size.width() / 2, size.height() / 2);
        m_currentPoint = m_startPoint + QPointF(size.width(), size.height());

        clearSelection();
        clearFocus();

        m_newItem = createNewItem();
    }
}

void ImageEditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::RightButton)
    {
        if (isCropping())
        {
            cancelCrop();
        }
        else
        {
            clearFocus();
            clearSelection();
            setMode(Select);
        }
        event->accept();
        return;
    }

    m_startPoint = event->scenePos();
    m_multiSelection = false;
    m_pressedItem = NULL;

    QGraphicsItem* item = itemAt(event->scenePos());

    bool itemHitTest = false;
    if (item)
    {
        itemHitTest = item->hasCursor();
    }

    if (m_mode != ItemPen)
    {

        if (item && ((m_mode == Crop && item->type() != GraphicsCropItem::Type)
            || (m_mode != Select && (item->type() == GraphicsPenItem::Type))))
        {
            // for this types of items, don't select them
            // but allow creating new items on top of them
            item = NULL;
        }

        if (item == NULL && m_mode == Select && !isCropping())
        {
            m_multiSelection = true;
            event->accept();
        }
        else if ((m_mode == Select || (m_selectOnPress && item ))
                 || (item && (!item->isSelected() || (item->isSelected() && itemHitTest ))))
        {
            m_pressedItem = item;

            if (isCropping())
            {
                if (event->button() == Qt::RightButton)
                {
                    cancelCrop();
                }

                if (item && (item->isSelected() && itemHitTest) ) // hasCursor mean hover over some selection mark
                {
                }
                else
                {
                    // don't allow anithing in cropping mode
                    return;
                }
            }

            if (item)
            {
                if (focusItem())
                {
                    focusItem()->clearFocus();
                }
                if (event->modifiers() & Qt::ShiftModifier && !m_selectOnPress)
                {
                    QPointF point = m_backgroundPixmap.rect().bottomRight(); // find top left point
                    foreach(QGraphicsItem* item, selectedItems())
                    {
                        QPointF pos = QPointF((item->boundingRect().left() + item->boundingRect().right()) / 2,
                                              (item->boundingRect().top() + item->boundingRect().bottom()) / 2);
                        if (pos.x() < point.x())
                        {
                            point.setX(pos.x());
                        }
                        if (pos.y() < point.y())
                        {
                            point.setY(pos.y());
                        }
                    }
                    if (point.x() < m_backgroundPixmap.rect().right()
                        && point.y() < m_backgroundPixmap.rect().bottom())
                    {
                        QRectF selectionRect(point, m_startPoint);
                        QPainterPath path;
                        path.addRect(selectionRect);
                        setSelectionArea(path);
                    }
                    event->accept();
                }
                else if (event->modifiers() & Qt::ControlModifier && !m_selectOnPress)
                {
                    item->setSelected(!item->isSelected());
                    event->accept();
                }
                else
                {
                    if (!item->isSelected() && (m_mode == Select || m_selectOnPress))
                    {
                        item->setSelected(true);
                        if (!(event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)))
                        {
                            QList<QGraphicsItem*> selections = selectedItems();
                            foreach(QGraphicsItem* item2, selections)
                            {
                                if (item2 != item)
                                {
                                    item2->setSelected(false);
                                }
                            }
                        }
                        event->accept();
                    }

                    if (item && item->isSelected() && itemHitTest)
                    {
                        QGraphicsScene::mousePressEvent(event);
                    }
                }
            }

            m_pressedItemWasSelected = item && item->isSelected();

        }

    }

    /*
    if (!event->isAccepted())
    {
        if (item != NULL)
        {
            GraphicsTextItem* textItem = qgraphicsitem_cast<GraphicsTextItem*>(m_newItem);
            if (textItem)
            {
                textItem->setSelected(true);
                textItem->setFocus();
                //QTextCursor cursor = textItem->textCursor();
                //cursor.select(QTextCursor::Document);
                //textItem->setTextCursor(cursor);
                event->accept();
            }

        }
    }
    */

    if ((m_newItem == NULL && m_mode == ItemPen) || ((!m_selectOnPress || item == NULL) &&
        !event->isAccepted() && m_newItem == NULL && event->button() == Qt::LeftButton &&
            (m_mode == Crop ||            
             m_mode == ItemBlur ||
             m_mode == ItemRect ||
             m_mode == ItemEmptyRect ||
             m_mode == ItemEllipse ||
             m_mode == ItemEmptyEllipse ||
             m_mode == ItemStar ||
             m_mode == ItemEmptyStar ||
             //m_mode == ItemNumber ||
             //m_mode == ItemHighlight ||
             m_mode == ItemLine ||
             m_mode == ItemArrow ||
             m_mode == ItemImage ||
             (m_mode == ItemText && focusItem() == NULL))))
    {
        // create handler

        m_startPoint = event->scenePos();
        m_currentPoint = m_startPoint;              

        clearSelection();
        clearFocus();

        m_newItem = createNewItem();
        if (m_newItem)
        {
            event->accept();
        }

        // !create handler
    }

    if (!event->isAccepted() && m_mode != Select)
    {
        QGraphicsScene::mousePressEvent(event);
    }
}

void ImageEditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (!isCropping() && m_multiSelection)
    {
        m_multiSelection = false;
        QPainterPath path;
        QList<QGraphicsItem*> items;
        if (event->modifiers() & Qt::ControlModifier)
        {
            items = selectedItems();
        }
        path.addRect(QRectF(m_startPoint, m_currentPoint));
        setSelectionArea(path, Qt::IntersectsItemShape);
        if (event->modifiers() & Qt::ControlModifier)
        {
            foreach(QGraphicsItem* item, items)
            {
                item->setSelected(true);
            }
        }
        /*
        else if (selectedItems().count() > 0)
        {
            // exclude pens from multiselection
            items = selectedItems();
            foreach(QGraphicsItem* item, items)
            {
                if (item->type() == GraphicsPenItem::Type)
                {
                    item->setSelected(false);
                }
            }
        }
        */
        update();
    }

    if (m_newItem && ((m_currentPoint - m_startPoint).manhattanLength() < 10) && m_mode != ItemPen)
    {
        if (!isCropping())
        {
            //if (m_mode == ItemNumber)
            //{
            //    m_number--;
            //}
            removeItem(m_newItem);
            delete m_newItem;
            m_newItem = NULL;
        }
        event->accept();
        return;
    }

    if (m_newItem)
    {
        if(m_mode == ItemPen)
        {
            GraphicsPenItem* penItem = qgraphicsitem_cast<GraphicsPenItem*>(m_newItem);
            if (penItem)
            {
                penItem->finishAddingPoints();
            }
        }
        if (isCropping())
        {
            setMode(Select);
        }
        else
        {
            clearSelection();
            m_newItem->setSelected(true);
            m_newItem = NULL;
        }
    }

    if (!(event->modifiers() & Qt::ShiftModifier))
    {
        QGraphicsScene::mouseReleaseEvent(event);
    }
}

void ImageEditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        if (m_mode == ItemPen)
        {
            GraphicsPenItem* penItem = qgraphicsitem_cast<GraphicsPenItem*>(m_newItem);
            penItem->addPoint(event->scenePos());
            return;
        }

        if (m_pressedItem)
        {
            if (!m_pressedItemWasSelected)
            {
                if (!(event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)))
                {
                    QList<QGraphicsItem*> selections = selectedItems();
                    foreach(QGraphicsItem* item, selections)
                    {
                        if (item != m_pressedItem)
                        {
                            item->setSelected(false);
                            qDebug() << "deselected item " << item->type();
                        }
                    }
                }
                m_pressedItem->setSelected(true);
            }


            if (!m_pressedItem->data(ItemStateResizing).toBool())
            {
                QPointF delta = event->scenePos() - m_startPoint;
                if (!delta.isNull())
                {
                    foreach(QGraphicsItem* item, selectedItems())
                    {
                        GraphicsRectItem* rectItem = dynamic_cast<GraphicsRectItem*>(item);
                        if (rectItem)
                        {
                            QRectF r = rectItem->rect();
                            r.translate(delta);

                            if (rectItem->isClip())
                            {
                                if (r.right() > sceneRect().right())
                                {
                                    r.translate(QPointF(sceneRect().right() - r.right(), 0));
                                }
                                if (r.left() < sceneRect().left())
                                {
                                    r.translate(QPointF(sceneRect().left() - r.left(), 0));
                                }
                                if (r.bottom() > sceneRect().bottom())
                                {
                                    r.translate(QPointF(0, sceneRect().bottom() - r.bottom()));
                                }
                                if (r.top() < sceneRect().top())
                                {
                                    r.translate(QPointF(0, sceneRect().top() - r.top()));
                                }
                            }

                            rectItem->setRect(r);
                        }
                        else
                        {
                            GraphicsEllipseItem* ellipseItem = dynamic_cast<GraphicsEllipseItem*>(item);
                            if (ellipseItem)
                            {
                                QRectF r = ellipseItem->rect();
                                r.translate(delta);
                                ellipseItem->setRect(r);
                            }                           
                            else
                            {
                                GraphicsTextItem* textItem = dynamic_cast<GraphicsTextItem*>(item);
                                if (textItem)
                                {
                                    QRectF r = textItem->realRect();
                                    r.translate(delta);

                                    if (textItem->isClip())
                                    {
                                        if (r.right() > sceneRect().right())
                                        {
                                            r.translate(QPointF(sceneRect().right() - r.right(), 0));
                                        }
                                        if (r.left() < sceneRect().left())
                                        {
                                            r.translate(QPointF(sceneRect().left() - r.left(), 0));
                                        }
                                        if (r.bottom() > sceneRect().bottom())
                                        {
                                            r.translate(QPointF(0, sceneRect().bottom() - r.bottom()));
                                        }
                                        if (r.top() < sceneRect().top())
                                        {
                                            r.translate(QPointF(0, sceneRect().top() - r.top()));
                                        }
                                    }

                                    textItem->setRect(r);
                                    //textItem->setPos(textItem->pos() + r.topLeft());
                                }
                                else
                                {
                                    item->setPos(item->pos() + delta);
                                }
                            }
                        }
                    }
                    m_startPoint = event->scenePos();
                }
                event->accept();
            }
        }
        else
        {
            // pressed not on the item
            if (m_multiSelection)
            {
                m_currentPoint = event->scenePos();
                update();
                event->accept();
            }
        }
    }

    // create handler

    m_currentPoint = event->scenePos();
    if (m_newItem != NULL)
    {
        switch(m_mode)
        {        
        case ItemRect:
        case ItemEmptyRect:
        //case ItemHighlight:
        case ItemBlur:
        case ItemImage:
        case ItemStar:
        case ItemEmptyStar:
        case Crop:
            {
                GraphicsRectItem* item = dynamic_cast<GraphicsRectItem*>(m_newItem);
                Q_ASSERT(item);
                if (item)
                {
                    QRectF r(m_startPoint, m_currentPoint);
                    r = r.normalized();
                    if (item->isClip())
                    {
                        r = r.intersect(sceneRect());
                    }
                    item->setRect(r);
                }
                event->accept();
            }
            break;
        case ItemEllipse:
        case ItemEmptyEllipse:
        //case ItemNumber:
            {
                GraphicsEllipseItem* item = dynamic_cast<GraphicsEllipseItem*>(m_newItem);
                Q_ASSERT(item);
                if (item)
                {
                    item->setRect(QRectF(m_startPoint, m_currentPoint).normalized());
                }
                event->accept();
            }
            break;
        case ItemArrow:
        case ItemLine:
            {
                GraphicsArrowItem* item = qgraphicsitem_cast<GraphicsArrowItem*>(m_newItem);
                Q_ASSERT(item);
                if (item)
                {
                    item->setLine(QLineF(m_startPoint, m_currentPoint));
                }
                event->accept();
            }
            break;
        case ItemText:
            {
                GraphicsTextItem* item = qgraphicsitem_cast<GraphicsTextItem*>(m_newItem);
                Q_ASSERT(item);
                if (item)
                {
                    QRectF r(m_startPoint, m_currentPoint);
                    r = r.normalized();
                    if (item->isClip())
                    {
                        r = r.intersect(sceneRect());
                    }
                    item->setRect(r);
                }
                event->accept();
            }
            break;
        default:
            break;
        }
    }

    // !create handler

    if (!event->isAccepted())
    {
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void ImageEditorScene::deleteSelection()
{
    foreach(QGraphicsItem* item, selectedItems())
    {
        removeItem(item);
        delete item;
    }
}

void ImageEditorScene::selectAll()
{
    setMode(Select);

    foreach(QGraphicsItem* item, items())
    {
        item->setSelected(true);
    }
}

void ImageEditorScene::keyPressEvent(QKeyEvent* keyEvent)
{
    if (focusItem() == NULL)
    {
        if (isCropping())
        {
            if (keyEvent->key() == Qt::Key_Enter
                || keyEvent->key() == Qt::Key_Return)
            {
                doCrop();
            }
            else if (keyEvent->key() == Qt::Key_Escape)
            {
                cancelCrop();
            }
            keyEvent->accept();
            return;
        }

        if (keyEvent->key() == Qt::Key_Delete)
        {
            if (isCropping())
            {
                cancelCrop();
            }
            else
            {
                deleteSelection();
            }
            keyEvent->accept();
            return;
        }
    }

    QGraphicsScene::keyPressEvent(keyEvent);
}

void ImageEditorScene::drawForeground(QPainter* painter, const QRectF& rect)
{
    QGraphicsScene::drawForeground(painter, rect);

    /*
    QRectF bgRect = m_backgroundPixmap.rect();
    //bgRect = inflateRect(bgRect, 1.0, 1.0);

    painter->drawRect(bgRect);

    QColor color(Qt::black);
    color.setAlpha(200);
    QBrush brush(color);

    if ((rect.bottom() > bgRect.top()) && (rect.right() > bgRect.right()))
    {
        QRectF rect1(bgRect.topRight(), rect.bottomRight());
        painter->fillRect(rect1, brush);
    }
    if (rect.bottom() > bgRect.bottom())
    {
        QRectF rect2(bgRect.bottomLeft(), QPointF(bgRect.right(), rect.bottom()));
        painter->fillRect(rect2, brush);
    }
    if (rect.top() < 0)
    {
        QRectF rect3(rect.topLeft(), QPoint(rect.right(), bgRect.top()));
        painter->fillRect(rect3.normalized(), brush);
    }
    if (rect.left() < 0)
    {
        QRectF rect4(QPoint(rect.left(), bgRect.top()), QPoint(bgRect.left(), rect.bottom()));
        painter->fillRect(rect4.normalized(), brush);
    }
    */

    if (m_multiSelection && m_pressedItem == NULL)
    {
        QBrush brush(QColor(0, 127, 255, 50));
        QRectF selectrect = QRectF(m_startPoint, m_currentPoint);
        painter->fillRect(selectrect, brush);
        QPen pen(QColor(0, 127, 255, 255));
        painter->setPen(pen);
        painter->drawRect(selectrect);
    }
/*
    // stroke over highlight items.
    QList<QGraphicsItem*> list = items();

    while(list.count() > 0)
    {
        QGraphicsItem* item = list.at(0);

        GraphicsHighlightItem* highlightItem = qgraphicsitem_cast<GraphicsHighlightItem*>(item);
        if (item->type() == GraphicsHighlightItem::Type)
        {
            QRectF rect = item->mapRectToScene(highlightItem->rect());

            QPainterPath path;
            path.addRect(rect);

            QList<QGraphicsItem*> forRemove;
            foreach(QGraphicsItem* item2, items(rect))
            {
                if (item == item2)
                {
                    break;
                }

                GraphicsHighlightItem* highlightItem2 = qgraphicsitem_cast<GraphicsHighlightItem*>(item2);
                if (item2->type() == GraphicsHighlightItem::Type
                    && highlightItem2->brush().color() == highlightItem->brush().color())
                {
                    forRemove.push_front(item2);
                    QRectF r = item2->mapRectToScene(highlightItem2->rect());
                    QPainterPath path2;
                    path.addRect(r);
                    path = path.united(path2);
                }
            }

            QPainterPathStroker stroker;
            painter->setPen(QPen(Qt::red));
            painter->drawPath(path);

            foreach(QGraphicsItem* removeIt, forRemove)
            {
                list.removeOne(removeIt);
            }
        }

        list.removeOne(item);
    }
*/
}

void ImageEditorScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    QGraphicsScene::drawBackground(painter, rect);

    painter->drawPixmap(rect.topLeft(), m_backgroundPixmap, rect);

/*
    QRectF bgRect = m_backgroundPixmap.rect();
    bgRect = inflateRect(bgRect, 1.0, 1.0);
    painter->drawRect(bgRect);
*/
}

void ImageEditorScene::setPixmap(const QPixmap& pixmap)
{
    m_number = 0;
    clear();
    m_backgroundPixmap = pixmap;
    if (!m_backgroundPixmap.isNull())
    {
        setSceneRect(m_backgroundPixmap.rect());
    }
}

void ImageEditorScene::setItemImage(const QString& fileName)
{
    if (QFileInfo(fileName).suffix() == "png")
    {
        m_itemPixmap.load(fileName);
    }
    else
    {
        m_itemPixmap  = QPixmap();
    }
    m_itemFileName = fileName;
}

bool ImageEditorScene::isCropping() const
{
    return (m_mode == Select || m_mode == Crop)
            && m_newItem && m_newItem->type() == GraphicsCropItem::Type;
}

ImageEditorScene::Mode ImageEditorScene::mode() const
{
    return m_mode;
}

QPixmap& ImageEditorScene::backgroundPixmap()
{
    return m_backgroundPixmap;
}

const QPixmap& ImageEditorScene::backgroundPixmap() const
{
    return m_backgroundPixmap;
}

void ImageEditorScene::cancelCrop()
{
    if (isCropping())
    {
        if (m_newItem != NULL)
        {
            //removeItem(m_newItem);
            delete m_newItem;
            m_newItem = NULL;
        }

        invalidate();
    }    
}

void ImageEditorScene::doCrop()
{
    if (isCropping())
    {
        QGraphicsItem* item = m_newItem;
        if (item && item->type() == GraphicsCropItem::Type)
        {
            GraphicsCropItem* crop = qgraphicsitem_cast<GraphicsCropItem*>(item);
            QRectF r = crop->rect();
            removeItem(crop);
            delete crop;
            m_newItem = NULL;

            doCrop(r);
            invalidate();
        }
    }
}

void ImageEditorScene::doCrop(const QRectF& r)
{
    if (!m_backgroundPixmap.isNull())
    {
        QPixmap newPixmap = m_backgroundPixmap.copy(r.left(), r.top(), r.width(), r.height());
        m_backgroundPixmap = newPixmap;
        setSceneRect(m_backgroundPixmap.rect());
        invalidate();
        // move items
        QList<QGraphicsItem*> outsideItems;

        foreach(QGraphicsItem* item, items())
        {
            if (r.intersect(item->mapRectToScene(item->boundingRect())).isEmpty())
            {
                outsideItems.append(item);
            }
            else
            {
                item->translate(-r.left(), -r.top());
            }
        }

        foreach(QGraphicsItem* item, outsideItems)
        {
            removeItem(item);
            delete item;
        }
    }
    else
    {
        setSceneRect(QRectF(QPointF(0,0), r.size()));
    }
}

void ImageEditorScene::bringToFront(QGraphicsItem* selectedItem)
{
    qreal zValue = 0;
    foreach (QGraphicsItem *item, items())
    {
        qreal itemZValue = item->zValue();

        if (itemZValue >= zValue)
        {
                zValue = itemZValue + 0.001;
        }
    }
    selectedItem->setZValue(zValue);
}

void ImageEditorScene::bringToFront()
{
    if (selectedItems().isEmpty())
    {
        return;
    }

    foreach(QGraphicsItem *selectedItem, selectedItems())
    {
        QList<QGraphicsItem*> overlapItems = selectedItem->collidingItems();

        qreal zValue = 0;
        foreach (QGraphicsItem *item, overlapItems)
        {
            qreal itemZValue = item->zValue();

            if (itemZValue >= zValue)
            {
                    zValue = itemZValue + 0.001;
            }
        }
        selectedItem->setZValue(zValue);
    }
}

void ImageEditorScene::sendToBack()
{
    if (selectedItems().isEmpty())
    {
        return;
    }

    foreach(QGraphicsItem *selectedItem , selectedItems())
    {
        QList<QGraphicsItem*> overlapItems = selectedItem->collidingItems();

        qreal zValue = 0;
        foreach (QGraphicsItem *item, overlapItems)
        {
            qreal itemZValue = item->zValue();

            if (itemZValue <= zValue)
            {
                zValue = itemZValue - 0.001;
            }
        }
        selectedItem->setZValue(zValue);
    }
}
