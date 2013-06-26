#include "tooltipwidget.h"
#include <QResizeEvent>
#include <QDebug>
#include <QWidget>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include "preferenceswidget.h"

// ToolTipWidget

ToolTipWidget::ToolTipWidget(QWidget *parent)
    : QWidget(parent)
    , m_trianglePosition(33)
{
    init();
}

ToolTipWidget::ToolTipWidget(const QString& title, QPoint basePoint, QWidget* parent)
    : QWidget(parent)
    , m_basePoint(basePoint)
    , m_trianglePosition(33)
{
    init(title);
}

ToolTipWidget::~ToolTipWidget()
{

}

void ToolTipWidget::init(const QString& title)
{
    setWindowFlags(Qt::FramelessWindowHint
                   | Qt::WindowStaysOnTopHint
                   | Qt::ToolTip);
    setAttribute(Qt::WA_DeleteOnClose);

    setStyleSheet("* { font: 75 10pt \"Arial\"; background-color: #808080; } ");
    setWindowOpacity(0.75);

    setTitle(title);
}

void ToolTipWidget::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    close();
}

void ToolTipWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    close();
}

void ToolTipWidget::updateRegion()
{
    updateSize();

    QRect r = geometry();

    r.translate(-r.left(), -r.top());

    QRegion maskRegion;
    r.setHeight(32);

    //maskRegion = maskRegion.unite(roundedRegion);
    maskRegion = maskRegion.unite(QRegion(0, 0, r.width(), r.height()));

    const int delta = 10;

    QVector<QPoint> vector(3);
    vector[0] = QPoint(m_trianglePosition - delta, 32);
    vector[1] = QPoint(m_trianglePosition, 32 + delta);
    vector[2] = QPoint(m_trianglePosition + delta, 32);

    QPolygon polygon(vector);
    QRegion triangle(polygon);

    maskRegion = maskRegion.unite(triangle);


    vector[0] = QPoint(0, 0);
    vector[1] = QPoint(0, 3);
    vector[2] = QPoint(3, 0);
    QPolygon polygon1(vector);
    maskRegion = maskRegion.subtract(QRegion(polygon1));

    vector[0] = QPoint(r.width(),0);
    vector[1] = QPoint(r.width(),3);
    vector[2] = QPoint(r.width()-3,0);
    QPolygon polygon2(vector);
    maskRegion = maskRegion.subtract(QRegion(polygon2));

    vector[0] = QPoint(r.width(),r.height());
    vector[1] = QPoint(r.width()-4,r.height());
    vector[2] = QPoint(r.width(),r.height()-4);
    QPolygon polygon3(vector);
    maskRegion = maskRegion.subtract(QRegion(polygon3));

    vector[0] = QPoint(0,r.height());
    vector[1] = QPoint(0,r.height()-4);
    vector[2] = QPoint(4,r.height());
    QPolygon polygon4(vector);
    maskRegion = maskRegion.subtract(QRegion(polygon4));

    setMask(maskRegion);
}

void ToolTipWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateRegion();
}


void ToolTipWidget::paintEvent(QPaintEvent *event)
{
    QRect r = rect();
    r.setHeight(32);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    painter.setBrush(Qt::black);
    painter.drawRoundedRect(r.adjusted(1,1,-1,-1), 6, 6);

    const int delta = 10;
    const QPoint points[3]  = {
        QPoint(m_trianglePosition - delta, 30),
        QPoint(m_trianglePosition, 30 + delta),
        QPoint(m_trianglePosition + delta, 30) };

    painter.setPen(Qt::black);
    painter.drawPolygon(points, sizeof(points)/sizeof(points[0]));

    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::white);
    painter.drawText(r, Qt::AlignCenter | Qt::TextSingleLine,  m_title);
}

void ToolTipWidget::updateSize()
{
    QRect r = fontMetrics().boundingRect(QRect(0,0, INT_MAX, INT_MAX), Qt::AlignCenter | Qt::TextSingleLine, m_title);

    const int delta = (32 - r.height() ) / 2;
    r.adjust( - delta, - delta, delta, delta);
    r.setHeight(48);

    if (size() != r.size())
    {
        setMinimumSize(r.size());
        setMaximumSize(r.size());
    }

    if (!m_basePoint.isNull())
    {
        updateErrorPos(m_basePoint);
    }
}

QString ToolTipWidget::title() const
{
    return m_title;
}

void ToolTipWidget::setTitle(const QString& title)
{
    m_title = title;
    updateSize();
}

void ToolTipWidget::updateErrorPos(QPoint p)
{
    m_basePoint = p;

    if (!m_basePoint.isNull())
    {
        QRect r = fontMetrics().boundingRect(QRect(0,0, INT_MAX, INT_MAX), Qt::AlignCenter | Qt::TextSingleLine, m_title);
        const int delta = (32 - r.height() ) / 2;

        QPoint p(m_basePoint.x() - 32, m_basePoint.y() - 48 + delta - 2);

        //p = mapToGlobal(p);

        int leftSide = p.x() + r.width() + 16;

        QRect desktopRect = QApplication::desktop()->geometry();

        if (leftSide > desktopRect.right())
        {
            int diff  = (desktopRect.right() - leftSide);
            p.setX(p.x() + diff);
            m_trianglePosition = -diff + 33;
        }

        move(p);
    }
}

