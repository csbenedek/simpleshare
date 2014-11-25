/* textinputwidget.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef TEXTINPUTWIDGET_H
#define TEXTINPUTWIDGET_H

#include <QLineEdit>
#include <QKeySequence>
#include <QString>
#include <QEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QPointer>
#include "tooltipwidget.h"

class TextInputWidget : public QLineEdit
{
    Q_OBJECT

public:
    explicit TextInputWidget(QWidget *parent = 0);
    virtual ~TextInputWidget();

    QRect closeIconRect() const;
    QString styleSheetString(bool focused, bool enabled = true) const;

    bool isShadowBackground() const;
    bool isClearButton() const;
    void setShadowBackground(bool value);
    void setClearButton(bool value);
    void setErrorMode(bool value, const QString& errorToolTip = "");
    bool isErrorMode() const;
    void hideErrorToolTip();
    void showErrorTooltip();
    void setEnabled(bool);

protected:
    virtual void mousePressEvent(QMouseEvent* mouseEvent);
    virtual void mouseMoveEvent(QMouseEvent* mouseEvent);
    virtual void paintEvent(QPaintEvent* event);

public slots:
    void clear();
    void onTextEdited(const QString & text);
    void onEditingFinished();

    // members
private:
    bool            m_shadowBackground;
    bool            m_clearButton;

    QPixmap         m_reloadIcon;
    QColor          m_inactiveBackground;
    QColor          m_activeBackground;

    QColor          m_errorColor;
    QPixmap         m_errorIcon;
    bool            m_errorMode;
    QString         m_errorTooltip;

    QPointer<ToolTipWidget>  m_toolTipWidget;
};

#endif // KEYSEQUENCEWIDGET_H
