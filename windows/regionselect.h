#ifndef REGIONSELECT_H
#define REGIONSELECT_H

#include <QDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QPainter>

//#include <QDebug>


QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class RegionSelect : public QDialog
{
public:
    explicit RegionSelect(bool unused, bool cursor = false, int delta = 1, QWidget *parent = 0, int monitorIndex = -1, RegionSelect* root = NULL, QPoint offset = QPoint(0,0));
    ~RegionSelect();

    QPixmap     getSelection();
    QPixmap&    getBackgroundImage();
    QRect   getSelectionRect() const;

protected:
    virtual bool event(QEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void showEvent(QShowEvent * event);
    virtual void hideEvent(QHideEvent * event);

    bool mousePressed(QPoint pos, Qt::MouseButton button);
    void mouseMoved(QPoint pos);
    bool mouseReleased();
#ifdef Q_OS_LINUX
    void sync();
    void syncSelectionData(QRect selectRect, QPoint currentPoint, QPoint  selStartPoint, QPoint selEndPoint);
#endif

private slots:
    void doSelectWindow();

private:
    void drawBackGround();
    void drawRectSelection(QPainter &painter);

private:
    QRect   m_selectRect;
    QSize   m_minimumSize;
    QSize   m_sizeDesktop;

    QPoint  m_currentPoint;
    QPoint  m_selStartPoint;
    QPoint  m_selEndPoint;

    bool    m_palBackground;

    int     m_delta;

    QPixmap m_desktopPixmapBkg;
    QPixmap m_desktopPixmapClr;

    bool    m_doubleClicked;

    QPoint  m_offset;

#ifdef Q_OS_LINUX
    // for linux version
    QList<RegionSelect*>    m_regions;
    RegionSelect*           m_root;
#endif
    bool    m_pressed;
};

#endif // REGIONSELECT_H
