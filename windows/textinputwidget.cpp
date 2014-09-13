#include "textinputwidget.h"
#include <QPainter>
#include <QDebug>
#include <QImage>
#include "applicationex.h"

TextInputWidget::TextInputWidget(QWidget *parent)
    : QLineEdit(parent)
    , m_shadowBackground(true)
    , m_clearButton(true)
    , m_errorMode(false)
    , m_toolTipWidget(NULL)
{
    m_reloadIcon.load(":/res/16x16_icon_close.png");
    m_errorIcon.load(":/res/16x16_icon_error.PNG");

    m_inactiveBackground    = QColor("#CCCCCC");
    m_activeBackground      = Qt::white;

    m_errorColor            = QColor("#ea6e07");

    setStyleSheet(styleSheetString(false));

    connect(this, SIGNAL(textEdited(QString)), this, SLOT(onTextEdited(QString)));
    connect(this, SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
}

TextInputWidget::~TextInputWidget()
{
    if (m_toolTipWidget)
    {
        delete m_toolTipWidget;
        m_toolTipWidget = NULL;
    }
}

void TextInputWidget::setEnabled(bool enabled)
{
    QLineEdit::setEnabled(enabled);
    //TODO: why won't work?
    setStyleSheet(styleSheetString(false, false));
    update();
}

void TextInputWidget::onTextEdited(const QString & text)
{
    if (!text.isEmpty())
    {
        setErrorMode(false);
        setStyleSheet(styleSheetString(true));
    }
}

void TextInputWidget::onEditingFinished()
{
    //setErrorMode(false);
    //setStyleSheet(styleSheetString(false));
}

QString TextInputWidget::styleSheetString(bool focused, bool enabled) const
{
    if (!focused && text().isEmpty())
    {
        focused = true;
    }

    QString result = " * { ";
    result +=  " font: bold 10pt \"Arial\"; ";
    if (m_shadowBackground)
    {
        QColor color = focused?m_activeBackground.name():m_inactiveBackground;
        if (enabled && isEnabled()) // don't change background on disabled control
        {
            result +=  " background-color: " + color.name() +"; ";
        }
    }
    if (m_errorMode)
    {
        result += " border: 1px solid " + m_errorColor.name() + "; ";
    }
    //result +=  " border-radius: 4px; ";
    result +=  " color: #666; ";
    result +=  "} ";

    return result;
}

void TextInputWidget::clear()
{
    setText("");
    setFocus();
    setStyleSheet(styleSheetString(true));
}

void TextInputWidget::showErrorTooltip()
{
    hideErrorToolTip();

    setStyleSheet(styleSheetString(hasFocus()));

    if (!(qobject_cast<ApplicationEx*>(qApp)->isActive()))
    {
        return;
    }

    if (m_errorMode && !m_errorTooltip.isEmpty())
    {
        QPoint p;
        if (!m_errorIcon.isNull())
        {
            QRect r = closeIconRect();
            p = mapToGlobal(r.topRight());
            p.setX(p.x() - m_errorIcon.width() / 2);
            p.setY(p.y() - 2);
        }
        m_toolTipWidget = new ToolTipWidget(m_errorTooltip, p, this);
        m_toolTipWidget->show();
        if (parentWidget())
        {
            qApp->setActiveWindow(parentWidget());
        }
    }

}

void TextInputWidget::setErrorMode(bool value, const QString& errorToolTip)
{
    m_errorMode = value;
    m_errorTooltip = errorToolTip;

    showErrorTooltip();
}

bool TextInputWidget::isErrorMode() const
{
    return m_errorMode;
}

bool TextInputWidget::isShadowBackground() const
{
    return m_shadowBackground;
}

bool TextInputWidget::isClearButton() const
{
    return m_clearButton;
}

void TextInputWidget::setShadowBackground(bool value)
{
    m_shadowBackground = value;
    setStyleSheet(styleSheetString(true));
}

void TextInputWidget::setClearButton(bool value)
{
    m_clearButton = value;
    update();
}

QRect TextInputWidget::closeIconRect() const
{
    QRect r = m_reloadIcon.rect();
    int deltaY = (height() - r.height()) / 2;
    int deltaX = width() - r.width() - deltaY;
    r.translate(deltaX, deltaY);
    return r;
}

void TextInputWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    QLineEdit::mouseMoveEvent(mouseEvent);

    bool isReload = isEnabled() && !m_reloadIcon.isNull() && !text().isEmpty() && m_clearButton;
    bool isError = isEnabled() && !m_errorIcon.isNull() && m_errorMode;

    if (isReload || isError)
    {
        QRect clearRect = closeIconRect();

        if (clearRect.contains(mouseEvent->pos()))
        {
            if (cursor().shape() != Qt::ArrowCursor)
            {
                setCursor(Qt::ArrowCursor);
            }
        }
        else
        {
            if (cursor().shape() != Qt::IBeamCursor)
            {
                setCursor(Qt::IBeamCursor);
            }
        }
    }
}

void TextInputWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
    QLineEdit::mousePressEvent(mouseEvent);

    QRect clearRect = closeIconRect();
    if (clearRect.contains(mouseEvent->pos()) )
    {
        bool isError = isEnabled() && !m_errorIcon.isNull() && !m_errorTooltip.isEmpty() && m_errorMode;
        if (isError)
        {
            if (m_toolTipWidget)
            {
                hideErrorToolTip();
            }
            else
            {
                showErrorTooltip();
            }
            return;
        }

        bool isReload = isEnabled() && !m_reloadIcon.isNull() && !text().isEmpty() && m_clearButton;
        if (isReload)
        {
            clear();
            return;
        }
    }
}

void TextInputWidget::paintEvent(QPaintEvent* event)
{
    QLineEdit::paintEvent(event);

    QPainter painter(this);

    QRect clearRect = closeIconRect();

    if (isEnabled() && m_errorMode && !m_errorIcon.isNull())
    {
        painter.drawPixmap(clearRect.topLeft(), m_errorIcon);
    }
    else if (isEnabled() && !m_reloadIcon.isNull() && !text().isEmpty() && m_clearButton)
    {
        painter.drawPixmap(clearRect.topLeft(), m_reloadIcon);
    }
}

void TextInputWidget::hideErrorToolTip()
{
    if (m_toolTipWidget)
    {
        m_toolTipWidget->hide();
        m_toolTipWidget->deleteLater();
    }
}
