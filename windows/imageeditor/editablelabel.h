#ifndef EDITABLELABEL_H
#define EDITABLELABEL_H

#include <QLabel>
#include <QLineEdit>

class EditableLabel : public QLineEdit
{
    Q_OBJECT
public:
    explicit EditableLabel(QWidget *parent=0);
    explicit EditableLabel(const QString &text, QWidget *parent=0);
    virtual ~EditableLabel();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent* event);
    virtual void paintEvent(QPaintEvent* event);

private:
    void beginEdit();

private slots:
    void endEdit();
    void onSelectionChanged();

signals:

public slots:

};

#endif // EDITABLELABEL_H
