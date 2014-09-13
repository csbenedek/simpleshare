#include "systemtrayicon.h"





bool SystemTrayIcon::trayMessage(DWORD msg)
{
    NOTIFYICONDATA tnd;
    memset(&tnd, 0, notifyIconSize);

    tnd.uID = q_uNOTIFYICONID;
    tnd.cbSize = sizeof(NOTIFYICONDATA);
    tnd.hWnd = m_hWnd;
    tnd.uFlags = NIF_SHOWTIP;
    tnd.uVersion = version;

    Q_ASSERT(testAttribute(Qt::WA_WState_Created));

    if (msg == NIM_ADD || msg == NIM_MODIFY) {
        setIconContents(tnd);
    }

    bool success = Shell_NotifyIcon(msg, &tnd);

    if (msg == NIM_ADD)
        return success && Shell_NotifyIcon(NIM_SETVERSION, &tnd);
    else
        return success;
}












SystemTrayIcon::SystemTrayIcon(QObject *parent)	: QObject(parent)
{
}

SystemTrayIcon::SystemTrayIcon(const QIcon &icon, QObject *parent) : QObject(parent)
{
    setIcon(icon);
}

SystemTrayIcon::~SystemTrayIcon()
{
    trayMessage(NIM_DELETE);
}

void SystemTrayIcon::setContextMenu(QMenu *menu)
{
    m_menu = menu;
}

QMenu* SystemTrayIcon::contextMenu() const
{
    return m_menu;
}

void QSystemTrayIcon::setIcon(const QIcon &icon)
{
    m_icon = icon;

    HICON hIconToDestroy = m_hIcon;

    m_hIcon = 0;
    if (icon.isNull())
        return;

    const int iconSizeX = GetSystemMetrics(SM_CXSMICON);
    const int iconSizeY = GetSystemMetrics(SM_CYSMICON);
    QSize size = icon.actualSize(QSize(iconSizeX, iconSizeY));
    QPixmap pm = icon.pixmap(size);
    if (!pm.isNull())
	{
		hIcon = pm.toWinHICON();
	}

    trayMessage(NIM_MODIFY);

    if (hIconToDestroy)
        DestroyIcon(hIconToDestroy);
}

QIcon QSystemTrayIcon::icon() const
{
    return m_icon;
}

/*!
    \property QSystemTrayIcon::toolTip
    \brief the tooltip for the system tray entry

    On some systems, the tooltip's length is limited. The tooltip will be truncated
    if necessary.
*/
void QSystemTrayIcon::setToolTip(const QString &tooltip)
{
    Q_D(QSystemTrayIcon);
    d->toolTip = tooltip;
    d->updateToolTip_sys();
}

QString QSystemTrayIcon::toolTip() const
{
    Q_D(const QSystemTrayIcon);
    return d->toolTip;
}

/*!
    \fn void QSystemTrayIcon::show()

    Shows the icon in the system tray.

    \sa hide(), visible
*/

/*!
    \fn void QSystemTrayIcon::hide()

    Hides the system tray entry.

    \sa show(), visible
*/

/*!
    \since 4.3
    Returns the geometry of the system tray icon in screen coordinates.

    \sa visible
*/
QRect QSystemTrayIcon::geometry() const
{
    Q_D(const QSystemTrayIcon);
    if (!d->visible)
        return QRect();
    return d->geometry_sys();
}

/*!
    \property QSystemTrayIcon::visible
    \brief whether the system tray entry is visible

    Setting this property to true or calling show() makes the system tray icon
    visible; setting this property to false or calling hide() hides it.
*/
void QSystemTrayIcon::setVisible(bool visible)
{
    Q_D(QSystemTrayIcon);
    if (visible == d->visible)
        return;
    if (d->icon.isNull() && visible)
        qWarning("QSystemTrayIcon::setVisible: No Icon set");
    d->visible = visible;
    if (d->visible)
        d->install_sys();
    else
        d->remove_sys();
}

bool QSystemTrayIcon::isVisible() const
{
    Q_D(const QSystemTrayIcon);
    return d->visible;
}

/*!
  \reimp
*/
bool QSystemTrayIcon::event(QEvent *e)
{
#if defined(Q_WS_X11)
    if (e->type() == QEvent::ToolTip) {
        Q_D(QSystemTrayIcon);
        return d->sys->deliverToolTipEvent(e);
    }
#endif
    return QObject::event(e);
}

/*!
    \enum QSystemTrayIcon::ActivationReason

     This enum describes the reason the system tray was activated.

     \value Unknown     Unknown reason
     \value Context     The context menu for the system tray entry was requested
     \value DoubleClick The system tray entry was double clicked
     \value Trigger     The system tray entry was clicked
     \value MiddleClick The system tray entry was clicked with the middle mouse button

     \sa activated()
*/

/*!
    \fn void QSystemTrayIcon::activated(QSystemTrayIcon::ActivationReason reason)

    This signal is emitted when the user activates the system tray icon. \a reason
    specifies the reason for activation. QSystemTrayIcon::ActivationReason enumerates
    the various reasons.

    \sa QSystemTrayIcon::ActivationReason
*/

/*!
    \fn void QSystemTrayIcon::messageClicked()

    This signal is emitted when the message displayed using showMessage()
    was clicked by the user.

    Currently this signal is not sent on Mac OS X.

    \note We follow Microsoft Windows XP/Vista behavior, so the
    signal is also emitted when the user clicks on a tray icon with
    a balloon message displayed.

    \sa activated()
*/


/*!
    Returns true if the system tray is available; otherwise returns false.

    If the system tray is currently unavailable but becomes available later,
    QSystemTrayIcon will automatically add an entry in the system tray if it
    is \l visible.
*/

bool QSystemTrayIcon::isSystemTrayAvailable()
{
    return QSystemTrayIconPrivate::isSystemTrayAvailable_sys();
}

/*!
    Returns true if the system tray supports balloon messages; otherwise returns false.

    \sa showMessage()
*/
bool QSystemTrayIcon::supportsMessages()
{
    return QSystemTrayIconPrivate::supportsMessages_sys();
}

/*!
    \fn void QSystemTrayIcon::showMessage(const QString &title, const QString &message, MessageIcon icon, int millisecondsTimeoutHint)
    \since 4.3

    Shows a balloon message for the entry with the given \a title, \a message and
    \a icon for the time specified in \a millisecondsTimeoutHint. \a title and \a message
    must be plain text strings.

    Message can be clicked by the user; the messageClicked() signal will emitted when
    this occurs.

    Note that display of messages are dependent on the system configuration and user
    preferences, and that messages may not appear at all. Hence, it should not be
    relied upon as the sole means for providing critical information.

    On Windows, the \a millisecondsTimeoutHint is usually ignored by the system
    when the application has focus.

    On Mac OS X, the Growl notification system must be installed for this function to
    display messages.
 
    \sa show() supportsMessages()
  */
void QSystemTrayIcon::showMessage(const QString& title, const QString& msg,
                            QSystemTrayIcon::MessageIcon icon, int msecs)
{
    Q_D(QSystemTrayIcon);
    if (d->visible)
        d->showMessage_sys(title, msg, icon, msecs);
}

//////////////////////////////////////////////////////////////////////
static QBalloonTip *theSolitaryBalloonTip = 0;

void QBalloonTip::showBalloon(QSystemTrayIcon::MessageIcon icon, const QString& title,
                              const QString& message, QSystemTrayIcon *trayIcon,
                              const QPoint& pos, int timeout, bool showArrow)
{
    hideBalloon();
    if (message.isEmpty() && title.isEmpty())
        return;

    theSolitaryBalloonTip = new QBalloonTip(icon, title, message, trayIcon);
    if (timeout < 0)
        timeout = 10000; //10 s default
    theSolitaryBalloonTip->balloon(pos, timeout, showArrow);
}

void QBalloonTip::hideBalloon()
{
    if (!theSolitaryBalloonTip)
        return;
    theSolitaryBalloonTip->hide();
    delete theSolitaryBalloonTip;
    theSolitaryBalloonTip = 0;
}

bool QBalloonTip::isBalloonVisible()
{
    return theSolitaryBalloonTip;
}

QBalloonTip::QBalloonTip(QSystemTrayIcon::MessageIcon icon, const QString& title,
                         const QString& message, QSystemTrayIcon *ti)
    : QWidget(0, Qt::ToolTip), trayIcon(ti), timerId(-1)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(ti, SIGNAL(destroyed()), this, SLOT(close()));

    QLabel *titleLabel = new QLabel;
    titleLabel->installEventFilter(this);
    titleLabel->setText(title);
    QFont f = titleLabel->font();
    f.setBold(true);
#ifdef Q_WS_WINCE
    f.setPointSize(f.pointSize() - 2);
#endif
    titleLabel->setFont(f);
    titleLabel->setTextFormat(Qt::PlainText); // to maintain compat with windows

#ifdef Q_WS_WINCE
    const int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
    const int closeButtonSize = style()->pixelMetric(QStyle::PM_SmallIconSize) - 2;
#else
    const int iconSize = 18;
    const int closeButtonSize = 15;
#endif

    QPushButton *closeButton = new QPushButton;
    closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    closeButton->setIconSize(QSize(closeButtonSize, closeButtonSize));
    closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    closeButton->setFixedSize(closeButtonSize, closeButtonSize);
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QLabel *msgLabel = new QLabel;
#ifdef Q_WS_WINCE
    f.setBold(false);
    msgLabel->setFont(f);
#endif
    msgLabel->installEventFilter(this);
    msgLabel->setText(message);
    msgLabel->setTextFormat(Qt::PlainText);
    msgLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // smart size for the message label
#ifdef Q_WS_WINCE
    int limit = QApplication::desktop()->availableGeometry(msgLabel).size().width() / 2;
#else
    int limit = QApplication::desktop()->availableGeometry(msgLabel).size().width() / 3;
#endif
    if (msgLabel->sizeHint().width() > limit) {
        msgLabel->setWordWrap(true);
        if (msgLabel->sizeHint().width() > limit) {
            msgLabel->d_func()->ensureTextControl();
            if (QTextControl *control = msgLabel->d_func()->control) {
                QTextOption opt = control->document()->defaultTextOption();
                opt.setWrapMode(QTextOption::WrapAnywhere);
                control->document()->setDefaultTextOption(opt);
            }
        }
#ifdef Q_WS_WINCE
        // Make sure that the text isn't wrapped "somewhere" in the balloon widget
        // in the case that we have a long title label.
        setMaximumWidth(limit);
#else
        // Here we allow the text being much smaller than the balloon widget
        // to emulate the weird standard windows behavior.
        msgLabel->setFixedSize(limit, msgLabel->heightForWidth(limit));
#endif
    }

    QIcon si;
    switch (icon) {
    case QSystemTrayIcon::Warning:
        si = style()->standardIcon(QStyle::SP_MessageBoxWarning);
        break;
    case QSystemTrayIcon::Critical:
	si = style()->standardIcon(QStyle::SP_MessageBoxCritical);
        break;
    case QSystemTrayIcon::Information:
	si = style()->standardIcon(QStyle::SP_MessageBoxInformation);
        break;
    case QSystemTrayIcon::NoIcon:
    default:
        break;
    }

    QGridLayout *layout = new QGridLayout;
    if (!si.isNull()) {
        QLabel *iconLabel = new QLabel;
        iconLabel->setPixmap(si.pixmap(iconSize, iconSize));
        iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        iconLabel->setMargin(2);
        layout->addWidget(iconLabel, 0, 0);
        layout->addWidget(titleLabel, 0, 1);
    } else {
        layout->addWidget(titleLabel, 0, 0, 1, 2);
    }

    layout->addWidget(closeButton, 0, 2);
    layout->addWidget(msgLabel, 1, 0, 1, 3);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->setMargin(3);
    setLayout(layout);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0xff, 0xff, 0xe1));
    pal.setColor(QPalette::WindowText, Qt::black);
    setPalette(pal);
}

QBalloonTip::~QBalloonTip()
{
    theSolitaryBalloonTip = 0;
}

void QBalloonTip::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), pixmap);
}

void QBalloonTip::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);
}

void QBalloonTip::balloon(const QPoint& pos, int msecs, bool showArrow)
{
    QRect scr = QApplication::desktop()->screenGeometry(pos);
    QSize sh = sizeHint();
    const int border = 1;
    const int ah = 18, ao = 18, aw = 18, rc = 7;
    bool arrowAtTop = (pos.y() + sh.height() + ah < scr.height());
    bool arrowAtLeft = (pos.x() + sh.width() - ao < scr.width());
    setContentsMargins(border + 3,  border + (arrowAtTop ? ah : 0) + 2, border + 3, border + (arrowAtTop ? 0 : ah) + 2);
    updateGeometry();
    sh  = sizeHint();

    int ml, mr, mt, mb;
    QSize sz = sizeHint();
    if (!arrowAtTop) {
        ml = mt = 0;
        mr = sz.width() - 1;
        mb = sz.height() - ah - 1;
    } else {
        ml = 0;
        mt = ah;
        mr = sz.width() - 1;
        mb = sz.height() - 1;
    }

    QPainterPath path;
#if defined(QT_NO_XSHAPE) && defined(Q_WS_X11)
    // XShape is required for setting the mask, so we just
    // draw an ugly square when its not available
    path.moveTo(0, 0);
    path.lineTo(sz.width() - 1, 0);
    path.lineTo(sz.width() - 1, sz.height() - 1);
    path.lineTo(0, sz.height() - 1);
    path.lineTo(0, 0);
    move(qMax(pos.x() - sz.width(), scr.left()), pos.y());
#else
    path.moveTo(ml + rc, mt);
    if (arrowAtTop && arrowAtLeft) {
        if (showArrow) {
            path.lineTo(ml + ao, mt);
            path.lineTo(ml + ao, mt - ah);
            path.lineTo(ml + ao + aw, mt);
        }
        move(qMax(pos.x() - ao, scr.left() + 2), pos.y());
    } else if (arrowAtTop && !arrowAtLeft) {
        if (showArrow) {
            path.lineTo(mr - ao - aw, mt);
            path.lineTo(mr - ao, mt - ah);
            path.lineTo(mr - ao, mt);
        }
        move(qMin(pos.x() - sh.width() + ao, scr.right() - sh.width() - 2), pos.y());
    }
    path.lineTo(mr - rc, mt);
    path.arcTo(QRect(mr - rc*2, mt, rc*2, rc*2), 90, -90);
    path.lineTo(mr, mb - rc);
    path.arcTo(QRect(mr - rc*2, mb - rc*2, rc*2, rc*2), 0, -90);
    if (!arrowAtTop && !arrowAtLeft) {
        if (showArrow) {
            path.lineTo(mr - ao, mb);
            path.lineTo(mr - ao, mb + ah);
            path.lineTo(mr - ao - aw, mb);
        }
        move(qMin(pos.x() - sh.width() + ao, scr.right() - sh.width() - 2),
             pos.y() - sh.height());
    } else if (!arrowAtTop && arrowAtLeft) {
        if (showArrow) {
            path.lineTo(ao + aw, mb);
            path.lineTo(ao, mb + ah);
            path.lineTo(ao, mb);
        }
        move(qMax(pos.x() - ao, scr.x() + 2), pos.y() - sh.height());
    }
    path.lineTo(ml + rc, mb);
    path.arcTo(QRect(ml, mb - rc*2, rc*2, rc*2), -90, -90);
    path.lineTo(ml, mt + rc);
    path.arcTo(QRect(ml, mt, rc*2, rc*2), 180, -90);

    // Set the mask
    QBitmap bitmap = QBitmap(sizeHint());
    bitmap.fill(Qt::color0);
    QPainter painter1(&bitmap);
    painter1.setPen(QPen(Qt::color1, border));
    painter1.setBrush(QBrush(Qt::color1));
    painter1.drawPath(path);
    setMask(bitmap);
#endif

    // Draw the border
    pixmap = QPixmap(sz);
    QPainter painter2(&pixmap);
    painter2.setPen(QPen(palette().color(QPalette::Window).darker(160), border));
    painter2.setBrush(palette().color(QPalette::Window));
    painter2.drawPath(path);

    if (msecs > 0)
        timerId = startTimer(msecs);
    show();
}

void QBalloonTip::mousePressEvent(QMouseEvent *e)
{
    close();
    if(e->button() == Qt::LeftButton)
        emit trayIcon->messageClicked();
}

void QBalloonTip::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == timerId) {
        killTimer(timerId);
        if (!underMouse())
            close();
        return;
    }
    QWidget::timerEvent(e);
}

void qtsystray_sendActivated(QSystemTrayIcon *i, int r)
{
    emit i->activated((QSystemTrayIcon::ActivationReason)r);
}


























SystemTrayIcon::SystemTrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
    , m_hBalloonIcon(0)
{

}

SystemTrayIcon::SystemTrayIcon(const QIcon &icon, QObject *parent)
    : QSystemTrayIcon(icon, parent)
    , m_hBalloonIcon(0)
{

}

SystemTrayIcon::~SystemTrayIcon()
{
    if (m_hBalloonIcon)
    {
        DestroyIcon(m_hBalloonIcon);
    }
}

void SystemTrayIcon::showMessage(const QString& title, const QString& message, QPixmap pixmap, int msecs)
{
    uint uSecs = 0;
    if (msecs < 0)
        uSecs = 10000; //10 sec default
    else uSecs = static_cast<int>(msecs);

    //message is limited to 255 chars + NULL
    QString messageString;
    if (message.isEmpty() && !title.isEmpty())
        messageString = QLatin1Char(' '); //ensures that the message shows when only title is set
    else
        messageString = message.left(255) + QChar();

    //title is limited to 63 chars + NULL
    QString titleString = title.left(63) + QChar();

    NOTIFYICONDATA tnd;
    memset(&tnd, 0, sizeof(NOTIFYICONDATA));

    memcpy(tnd.szInfo, message.utf16(), qMin(message.length() + 1, 256) * sizeof(wchar_t));
    memcpy(tnd.szInfoTitle, title.utf16(), qMin(title.length() + 1, 64) * sizeof(wchar_t));

    tnd.uID = q_uNOTIFYICONID;
    tnd.cbSize = sizeof(NOTIFYICONDATA);
    tnd.hWnd = 0; // TODO:
    tnd.uTimeout = uSecs;
    tnd.uFlags = NIF_INFO | NIF_SHOWTIP;
    tnd.dwInfoFlags = NIIF_USER;
    tnd.hBalloonIcon = pixmap.toWinHICON(); // TODO: need to delete that

    if (!Shell_NotifyIcon(NIM_MODIFY, &tnd))
    {
        QSystemTrayIcon::showMessage(title, message, Information, msecs);
    }

    if (m_hBalloonIcon)
    {
        DestroyIcon(m_hBalloonIcon);
    }
    m_hBalloonIcon = tnd.hBalloonIcon;
}

