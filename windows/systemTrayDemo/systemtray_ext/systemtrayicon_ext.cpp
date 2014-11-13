#include "systemtrayicon_ext.h"

#include "qsystemtrayicon_p.h"
#ifndef QT_NO_SYSTEMTRAYICON

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x600
#endif

#include <qt_windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <private/qsystemlibrary_p.h>
#include <QApplication>
#include <QSettings>

QT_BEGIN_NAMESPACE

static const UINT q_uNOTIFYICONID = 0;

static uint MYWM_TASKBARCREATED = 0;
#define MYWM_NOTIFYICON (WM_APP+101)

struct Q_NOTIFYICONIDENTIFIER {
    DWORD cbSize;
    HWND hWnd;
    UINT uID;
    GUID guidItem;
};

#ifndef NOTIFYICON_VERSION_4
#define NOTIFYICON_VERSION_4 4
#endif

#ifndef NIN_SELECT
#define NIN_SELECT (WM_USER + 0)
#endif

#ifndef NIN_KEYSELECT
#define NIN_KEYSELECT (WM_USER + 1)
#endif

#ifndef NIN_BALLOONTIMEOUT
#define NIN_BALLOONTIMEOUT (WM_USER + 4)
#endif

#ifndef NIN_BALLOONUSERCLICK
#define NIN_BALLOONUSERCLICK (WM_USER + 5)
#endif

#ifndef NIF_SHOWTIP
#define NIF_SHOWTIP 0x00000080
#endif

#define Q_MSGFLT_ALLOW 1

typedef HRESULT (WINAPI *PtrShell_NotifyIconGetRect)(const Q_NOTIFYICONIDENTIFIER* identifier, RECT* iconLocation);
typedef BOOL (WINAPI *PtrChangeWindowMessageFilter)(UINT message, DWORD dwFlag);
typedef BOOL (WINAPI *PtrChangeWindowMessageFilterEx)(HWND hWnd, UINT message, DWORD action, void* pChangeFilterStruct);

class QSystemTrayIconSys : QWidget
{
public:
    QSystemTrayIconSys(QSystemTrayIcon *object);
    ~QSystemTrayIconSys();
    bool winEvent( MSG *m, long *result );
    bool trayMessage(DWORD msg);
    void setIconContents(NOTIFYICONDATA &data);
    bool showMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, uint uSecs);
    QRect findIconGeometry(const int a_iButtonID);
    void createIcon();
    HICON hIcon;
    QPoint globalPos;
    QSystemTrayIcon *q;
private:
    uint notifyIconSize;
    int maxTipLength;
    int version;
    bool ignoreNextMouseRelease;
};

static bool allowsMessages()
{
#ifndef QT_NO_SETTINGS
    QSettings settings(QLatin1String("HKEY_CURRENT_USER\\Software\\Microsoft"
                                      "\\Windows\\CurrentVersion\\Explorer\\Advanced"), QSettings::NativeFormat);
    return settings.value(QLatin1String("EnableBalloonTips"), true).toBool();
#else
    return false;
#endif
}

QSystemTrayIconSys::QSystemTrayIconSys(QSystemTrayIcon *object)
    : hIcon(0), q(object), ignoreNextMouseRelease(false)

{
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA) {
        notifyIconSize = sizeof(NOTIFYICONDATA);
        version = NOTIFYICON_VERSION_4;
    } else {
        notifyIconSize = NOTIFYICONDATA_V2_SIZE;
        version = NOTIFYICON_VERSION;
    }

    maxTipLength = 128;

    // For restoring the tray icon after explorer crashes
    if (!MYWM_TASKBARCREATED) {
        MYWM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated");
    }

    // Allow the WM_TASKBARCREATED message through the UIPI filter on Windows Vista and higher
    static PtrChangeWindowMessageFilterEx pChangeWindowMessageFilterEx =
        (PtrChangeWindowMessageFilterEx)QSystemLibrary::resolve(QLatin1String("user32"), "ChangeWindowMessageFilterEx");

    if (pChangeWindowMessageFilterEx) {
        // Call the safer ChangeWindowMessageFilterEx API if available
        pChangeWindowMessageFilterEx(winId(), MYWM_TASKBARCREATED, Q_MSGFLT_ALLOW, 0);
    } else {
        static PtrChangeWindowMessageFilter pChangeWindowMessageFilter =
            (PtrChangeWindowMessageFilter)QSystemLibrary::resolve(QLatin1String("user32"), "ChangeWindowMessageFilter");

        if (pChangeWindowMessageFilter) {
            // Call the deprecated ChangeWindowMessageFilter API otherwise
            pChangeWindowMessageFilter(MYWM_TASKBARCREATED, Q_MSGFLT_ALLOW);
        }
    }
}

QSystemTrayIconSys::~QSystemTrayIconSys()
{
    if (hIcon)
        DestroyIcon(hIcon);
}

void QSystemTrayIconSys::setIconContents(NOTIFYICONDATA &tnd)
{
    tnd.uFlags |= NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tnd.uCallbackMessage = MYWM_NOTIFYICON;
    tnd.hIcon = hIcon;
    QString tip = q->toolTip();

    if (!tip.isNull()) {
        tip = tip.left(maxTipLength - 1) + QChar();
        memcpy(tnd.szTip, tip.utf16(), qMin(tip.length() + 1, maxTipLength) * sizeof(wchar_t));
    }
}

static int iconFlag( QSystemTrayIcon::MessageIcon icon )
{
    switch (icon) {
        case QSystemTrayIcon::Information:
            return NIIF_INFO;
        case QSystemTrayIcon::Warning:
            return NIIF_WARNING;
        case QSystemTrayIcon::Critical:
            return NIIF_ERROR;
        case QSystemTrayIcon::NoIcon:
            return NIIF_NONE;
        default:
            Q_ASSERT_X(false, "QSystemTrayIconSys::showMessage", "Invalid QSystemTrayIcon::MessageIcon value");
            return NIIF_NONE;
    }
}

bool QSystemTrayIconSys::showMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, uint uSecs)
{
    NOTIFYICONDATA tnd;
    memset(&tnd, 0, notifyIconSize);

    memcpy(tnd.szInfo, message.utf16(), qMin(message.length() + 1, 256) * sizeof(wchar_t));
    memcpy(tnd.szInfoTitle, title.utf16(), qMin(title.length() + 1, 64) * sizeof(wchar_t));

    tnd.uID = q_uNOTIFYICONID;
    tnd.dwInfoFlags = iconFlag(type);
    tnd.cbSize = notifyIconSize;
    tnd.hWnd = winId();
    tnd.uTimeout = uSecs;
    tnd.uFlags = NIF_INFO | NIF_SHOWTIP;

    Q_ASSERT(testAttribute(Qt::WA_WState_Created));

    return Shell_NotifyIcon(NIM_MODIFY, &tnd);
}

bool QSystemTrayIconSys::trayMessage(DWORD msg)
{
    NOTIFYICONDATA tnd;
    memset(&tnd, 0, notifyIconSize);

    tnd.uID = q_uNOTIFYICONID;
    tnd.cbSize = notifyIconSize;
    tnd.hWnd = winId();
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

void QSystemTrayIconSys::createIcon()
{
    hIcon = 0;
    QIcon icon = q->icon();
    if (icon.isNull())
        return;

    const int iconSizeX = GetSystemMetrics(SM_CXSMICON);
    const int iconSizeY = GetSystemMetrics(SM_CYSMICON);
    QSize size = icon.actualSize(QSize(iconSizeX, iconSizeY));
    QPixmap pm = icon.pixmap(size);
    if (pm.isNull())
        return;

    hIcon = pm.toWinHICON();
}

bool QSystemTrayIconSys::winEvent( MSG *m, long *result )
{
    switch(m->message) {
    case MYWM_NOTIFYICON:
        {
            int message = 0;
            QPoint gpos;

            if (version == NOTIFYICON_VERSION_4) {
                Q_ASSERT(q_uNOTIFYICONID == HIWORD(m->lParam));
                message = LOWORD(m->lParam);
                gpos = QPoint(GET_X_LPARAM(m->wParam), GET_Y_LPARAM(m->wParam));
            } else {
                Q_ASSERT(q_uNOTIFYICONID == m->wParam);
                message = m->lParam;
                gpos = QCursor::pos();
            }

            switch (message) {
            case NIN_SELECT:
            case NIN_KEYSELECT:
                if (ignoreNextMouseRelease)
                    ignoreNextMouseRelease = false;
                else 
                    emit q->activated(QSystemTrayIcon::Trigger);
                break;

            case WM_LBUTTONDBLCLK:
                ignoreNextMouseRelease = true; // Since DBLCLICK Generates a second mouse 
                                               // release we must ignore it
                emit q->activated(QSystemTrayIcon::DoubleClick);
                break;

            case WM_CONTEXTMENU:
                if (q->contextMenu()) {
                    q->contextMenu()->popup(gpos);
                    q->contextMenu()->activateWindow();
                }
                emit q->activated(QSystemTrayIcon::Context);
                break;

            case NIN_BALLOONUSERCLICK:
                emit q->messageClicked();
                break;

            case WM_MBUTTONUP:
                emit q->activated(QSystemTrayIcon::MiddleClick);
                break;

            default:
                break;
            }
            break;
        }
    default:
        if (m->message == MYWM_TASKBARCREATED)
            trayMessage(NIM_ADD);
        else
            return QWidget::winEvent(m, result);
        break;
    }
    return 0;
}

void QSystemTrayIconPrivate::install_sys()
{
    Q_Q(QSystemTrayIcon);
    if (!sys) {
        sys = new QSystemTrayIconSys(q);
        sys->createIcon();
        sys->trayMessage(NIM_ADD);
    }
}

/*
* This function tries to determine the icon geometry from the tray
*
* If it fails an invalid rect is returned.
*/
QRect QSystemTrayIconSys::findIconGeometry(const int iconId)
{
    static PtrShell_NotifyIconGetRect Shell_NotifyIconGetRect =
        (PtrShell_NotifyIconGetRect)QSystemLibrary::resolve(QLatin1String("shell32"), "Shell_NotifyIconGetRect");

    if (Shell_NotifyIconGetRect) {
        Q_NOTIFYICONIDENTIFIER nid;
        memset(&nid, 0, sizeof(nid));
        nid.cbSize = sizeof(nid);
        nid.hWnd = winId();
        nid.uID = iconId;

        RECT rect;
        HRESULT hr = Shell_NotifyIconGetRect(&nid, &rect);
        if (SUCCEEDED(hr)) {
            return QRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
        }
    }

    QRect ret;

    TBBUTTON buttonData;
    DWORD processID = 0;
    HWND trayHandle = FindWindow(L"Shell_TrayWnd", NULL);

    //find the toolbar used in the notification area
    if (trayHandle) {
        trayHandle = FindWindowEx(trayHandle, NULL, L"TrayNotifyWnd", NULL);
        if (trayHandle) {
            HWND hwnd = FindWindowEx(trayHandle, NULL, L"SysPager", NULL);
            if (hwnd) {
                hwnd = FindWindowEx(hwnd, NULL, L"ToolbarWindow32", NULL);
                if (hwnd)
                    trayHandle = hwnd;
            }
        }
    }

    if (!trayHandle)
        return ret;

    GetWindowThreadProcessId(trayHandle, &processID);
    if (processID <= 0)
        return ret;

    HANDLE trayProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ, 0, processID);
    if (!trayProcess)
        return ret;

    int buttonCount = SendMessage(trayHandle, TB_BUTTONCOUNT, 0, 0);
    LPVOID data = VirtualAllocEx(trayProcess, NULL, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE);

    if ( buttonCount < 1 || !data ) {
        CloseHandle(trayProcess);
        return ret;
    }

    //search for our icon among all toolbar buttons
    for (int toolbarButton = 0; toolbarButton  < buttonCount; ++toolbarButton ) {
        SIZE_T numBytes = 0;
        DWORD appData[2] = { 0, 0 };
        SendMessage(trayHandle, TB_GETBUTTON, toolbarButton , (LPARAM)data);

        if (!ReadProcessMemory(trayProcess, data, &buttonData, sizeof(TBBUTTON), &numBytes))
            continue;

        if (!ReadProcessMemory(trayProcess, (LPVOID) buttonData.dwData, appData, sizeof(appData), &numBytes))
            continue;

        int currentIconId = appData[1];
        HWND currentIconHandle = (HWND) appData[0];
        bool isHidden = buttonData.fsState & TBSTATE_HIDDEN;

        if (currentIconHandle == winId() &&
            currentIconId == iconId && !isHidden) {
            SendMessage(trayHandle, TB_GETITEMRECT, toolbarButton , (LPARAM)data);
            RECT iconRect = {0, 0, 0, 0};
            if(ReadProcessMemory(trayProcess, data, &iconRect, sizeof(RECT), &numBytes)) {
                MapWindowPoints(trayHandle, NULL, (LPPOINT)&iconRect, 2);
                QRect geometry(iconRect.left + 1, iconRect.top + 1,
                                iconRect.right - iconRect.left - 2,
                                iconRect.bottom - iconRect.top - 2);
                if (geometry.isValid())
                    ret = geometry;
                break;
            }
        }
    }
    VirtualFreeEx(trayProcess, data, 0, MEM_RELEASE);
    CloseHandle(trayProcess);
    return ret;
}

void QSystemTrayIconPrivate::showMessage_sys(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon type, int timeOut)
{
    if (!sys || !allowsMessages())
        return;

    uint uSecs = 0;
    if ( timeOut < 0)
        uSecs = 10000; //10 sec default
    else uSecs = (int)timeOut;

    //message is limited to 255 chars + NULL
    QString messageString;
    if (message.isEmpty() && !title.isEmpty())
        messageString = QLatin1Char(' '); //ensures that the message shows when only title is set
    else
        messageString = message.left(255) + QChar();

    //title is limited to 63 chars + NULL
    QString titleString = title.left(63) + QChar();

    sys->showMessage(titleString, messageString, type, uSecs);
}

QRect QSystemTrayIconPrivate::geometry_sys() const
{
    if (!sys)
        return QRect();

    return sys->findIconGeometry(q_uNOTIFYICONID);
}

void QSystemTrayIconPrivate::remove_sys()
{
    if (!sys)
        return;

    sys->trayMessage(NIM_DELETE);
    delete sys;
    sys = 0;
}

void QSystemTrayIconPrivate::updateIcon_sys()
{
    if (!sys)
        return;

    HICON hIconToDestroy = sys->hIcon;

    sys->createIcon();
    sys->trayMessage(NIM_MODIFY);

    if (hIconToDestroy)
        DestroyIcon(hIconToDestroy);
}

void QSystemTrayIconPrivate::updateMenu_sys()
{

}

void QSystemTrayIconPrivate::updateToolTip_sys()
{
    if (!sys)
        return;

    sys->trayMessage(NIM_MODIFY);
}

bool QSystemTrayIconPrivate::isSystemTrayAvailable_sys()
{
    return true;
}

bool QSystemTrayIconPrivate::supportsMessages_sys()
{
    return allowsMessages();
}

QT_END_NAMESPACE

#endif
















































bool SystemTrayIconExt::trayMessage(uint16_t msg)
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

SystemTrayIconExt::SystemTrayIconExt(QObject *parent)	: QObject(parent)
{
}

SystemTrayIconExt::SystemTrayIconExt(const QIcon &icon, QObject *parent) : QObject(parent)
{
    setIcon(icon);
}

SystemTrayIconExt::~SystemTrayIconExt()
{
    trayMessage(NIM_DELETE);
    if (m_hIcon)
    {
        DestroyIcon(m_hIcon);
    }
}

void SystemTrayIconExt::setContextMenu(QMenu *menu)
{
    m_menu = menu;
}

QMenu* SystemTrayIconExt::contextMenu() const
{
    return m_menu;
}

void SystemTrayIcon::setIcon(const QIcon &icon)
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

QIcon SystemTrayIcon::icon() const
{
    return m_icon;
}


void SystemTrayIcon::setToolTip(const QString &tooltip)
{
    Q_D(QSystemTrayIcon);
    d->toolTip = tooltip;
    d->updateToolTip_sys();
}

QString SystemTrayIcon::toolTip() const
{
    Q_D(const QSystemTrayIcon);
    return d->toolTip;
}

QRect SystemTrayIcon::geometry() const
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



SystemTrayIconExt::SystemTrayIconExt(QObject *parent)
    : QSystemTrayIcon(parent)
    , m_hBalloonIcon(0)
{

}

SystemTrayIconExt::SystemTrayIconExt(const QIcon &icon, QObject *parent)
    : QSystemTrayIcon(icon, parent)
    , m_hBalloonIcon(0)
{

}

SystemTrayIconExt::~SystemTrayIconExt()
{
    if (m_hBalloonIcon)
    {
        DestroyIcon(m_hBalloonIcon);
    }
}

void SystemTrayIconExt::showMessage(const QString &title, const QString &msg,
                 QSystemTrayIcon::MessageIcon icon = Information, int msecs = 10000, QPixmap pixmap = QPixmap())
{
    if ( pixmap.isNull() || m_hIcon == NULL )
    {
        QSystemTrayIcon::showMessage(title, message, icon, msecs);
        return;
    }

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

