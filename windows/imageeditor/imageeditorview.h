#ifndef IMAGEEDITORVIEW_H
#define IMAGEEDITORVIEW_H

#include <QGraphicsView>

class ImageEditorView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ImageEditorView(QWidget *parent = 0);
    explicit ImageEditorView(QGraphicsScene *scene, QWidget *parent = 0);

protected:
    virtual void scrollContentsBy(int dx, int dy);
    virtual void resizeEvent(QResizeEvent* event);

signals:

public slots:

};

#endif // IMAGEEDITORVIEW_H
