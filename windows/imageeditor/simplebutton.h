/* simplebutton.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef SIMPLEBUTTON_H
#define SIMPLEBUTTON_H

#include <QToolButton>

class SimpleButton : public QToolButton
{
    Q_OBJECT
public:
    explicit SimpleButton(QWidget *parent = 0);
    virtual ~SimpleButton();

    bool isCurrentTool() const;
    void setCurrentTool(bool current);

signals:

public slots:

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    bool m_currentTool;
};

#endif // SIMPLEBUTTON_H
