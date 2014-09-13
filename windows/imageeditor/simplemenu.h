#ifndef SIMPLEMENU_H
#define SIMPLEMENU_H

#include <QMenu>
#include <QToolButton>

class SimpleMenu : public QMenu
{
    Q_OBJECT
public:
    explicit SimpleMenu(QToolButton* button, QWidget *parent = 0);

protected:
    virtual void mouseReleaseEvent(QMouseEvent* e);

protected:
    QToolButton* m_button;
};

class SimpleMenuEx : public SimpleMenu
{
    Q_OBJECT
public:
    explicit SimpleMenuEx(QToolButton* button, QWidget *parent = 0);

    void setColumnCount(int count);
    int columnCount() const;
    void setItemSize(QSize size);
    QSize itemSize() const;

    QAction *addAction(const QString &text);
    QAction *addAction(const QIcon &icon, const QString &text);
    QAction *addAction(const QString &text, const QObject *receiver, const char* member, const QKeySequence &shortcut = 0);
    QAction *addAction(const QIcon &icon, const QString &text, const QObject *receiver, const char* member, const QKeySequence &shortcut = 0);

signals:

public slots:


protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void showEvent(QShowEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);

    void updateGeometry();

private:
    QAction* actionAt(const QPoint &) const;

private:
    int             m_columnCount;
    QSize           m_itemSize;
    QList<QAction*> m_items;
    QAction*        m_currentItem;
};

#endif // SIMPLEMENU_H
