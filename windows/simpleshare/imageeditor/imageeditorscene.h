#ifndef IMAGEEDITORSCENE_H
#define IMAGEEDITORSCENE_H

#include <QGraphicsScene>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
QT_END_NAMESPACE

class GraphicsTextItem;

class ImageEditorScene: public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode
    {
        Select = 0,
        ItemPen,
        ItemEmptyRect,
        ItemRect,
        ItemEmptyEllipse,
        ItemEllipse,
        ItemStar,
        ItemEmptyStar,
        //ItemHighlight,
        //ItemMarker = ItemHighlight,
        ItemLine,
        //ItemWaveLine = ItemLine,
        ItemArrow,
        ItemText,
        //ItemNumber,
        ItemBlur,
        ItemImage,
        Crop,
        ItemUser = 1000
    };

    ImageEditorScene(QObject *parent = 0);

    QFont   font() const;
    QColor  textColor() const;
    QColor  bgColor() const;
    QColor  fgColor() const;
    int     lineWidth() const;
    void setTextColor(const QColor &color);
    void setBgColor(const QColor &color);
    void setFgColor(const QColor &color);
    void setLineWidth(const int lineWidth);
    void setFont(const QFont &font);
    bool isSelectionModificationsBlocked() const;
    void blockSelectionModifications(bool block = true);

    void setPixmap(const QPixmap& pixmap);
    void setItemImage(const QString& fileName);
    void doCrop(const QRectF& r);
    void doCrop();
    void cancelCrop();
    bool isCropping() const;
    Mode mode() const;

    QPixmap& backgroundPixmap();
    const QPixmap& backgroundPixmap() const;

public slots:
    void editorLostFocus(GraphicsTextItem* item);
    void setMode(Mode mode);
    void sendToBack();
    void bringToFront(QGraphicsItem* selectedItem);
    void bringToFront();
    void deleteSelection();
    void selectAll();

signals:
    void newItemCreated(QGraphicsItem* item);
    void modeChanged(int mode);

protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent* keyEvent);
    virtual void drawForeground(QPainter* painter, const QRectF& rect);
    virtual void drawBackground(QPainter* painter, const QRectF& rect);

private:
    QGraphicsItem* createNewItem();
    void selectedItemsModified(bool penChanged, bool brushChanged, bool fontChanged);

private:
    Mode    m_mode;
    bool    m_leftButtonDown;
    QFont   m_font;
    QColor  m_textColor;
    QColor  m_fgColor;
    QColor  m_bgColor;
    int     m_lineWidth;
    int     m_penWidth; // for marker only

    QPointF m_currentPoint;
    QPointF m_startPoint;

    QPixmap         m_backgroundPixmap;
    QPixmap         m_itemPixmap;
    QString         m_itemFileName;
    QGraphicsItem*  m_newItem;
    QGraphicsItem*  m_pressedItem;
    bool            m_pressedItemWasSelected;
    int             m_number;

    bool            m_multiSelection;
    bool            m_blockSelectionModifications;

    bool            m_selectOnPress;
};

#endif //IMAGEEDITORSCENE_H
