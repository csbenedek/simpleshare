#ifndef TOOLTIPWIDGET_H
#define TOOLTIPWIDGET_H

#include <QWidget>

class ToolTipWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ToolTipWidget(QWidget* parent = 0);
    explicit ToolTipWidget(const QString& title, QPoint basePoint, QWidget* parent = 0);
    ~ToolTipWidget();

    QString title() const;
    void setTitle(const QString& title);
    void updateErrorPos(QPoint p);

protected:
    virtual void resizeEvent(QResizeEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void paintEvent(QPaintEvent *event);


private:
    void init(const QString& title = "");
    void updateSize();
    void updateRegion();

private:
    QString m_title;
    QPoint  m_basePoint;
    int     m_trianglePosition;
};

#endif // TOOLTIPWIDGET_H
