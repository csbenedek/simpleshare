#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600

#undef _WIN32_IE
#define _WIN32_IE 0x600

#include "systemtrayicon_p.h"
#ifndef QT_NO_SYSTEMTRAYICON

#include <qglobal.h>
#include <qt_windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "qsystemlibrary_p.h"
#include <QApplication>
#include <QSettings>

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

#ifndef NIIF_USER
#define NIIF_USER 0x00000004
#endif

#ifndef NIIF_LARGE_ICON
#define NIIF_LARGE_ICON 0x00000020
#endif

#define Q_MSGFLT_ALLOW 1

typedef HRESULT (WINAPI *PtrShell_NotifyIconGetRect)(const Q_NOTIFYICONIDENTIFIER* identifier, RECT* iconLocation);
typedef BOOL (WINAPI *PtrChangeWindowMessageFilter)(UINT message, DWORD dwFlag);
typedef BOOL (WINAPI *PtrChangeWindowMessageFilterEx)(HWND hWnd, UINT message, DWORD action, void* pChangeFilterStruct);

class SystemTrayIconSys : QWidget
{
public:
    SystemTrayIconSys(SystemTrayIcon *object);
    ~SystemTrayIconSys();
    bool winEvent( MSG *m, long *result );
    bool trayMessage(DWORD msg);
    void setIconContents(NOTIFYICONDATA &data);
    bool showMessage(const QString &title, const QString &message, SystemTrayIcon::MessageIcon type, uint uSecs, QIcon pixmap);
    QRect findIconGeometry(const int a_iButtonID);
    void createIcon();
    HICON hIcon;
    QPoint globalPos;
    SystemTrayIcon *q;
    HICON hBalloonIcon;
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

SystemTrayIconSys::SystemTrayIconSys(SystemTrayIcon *object)
    : hIcon(0), q(object), hBalloonIcon(0), ignoreNextMouseRelease(false)

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

SystemTrayIconSys::~SystemTrayIconSys()
{
    if (hIcon)
        DestroyIcon(hIcon);
    if (hBalloonIcon)
        DestroyIcon(hBalloonIcon);
}

void SystemTrayIconSys::setIconContents(NOTIFYICONDATA &tnd)
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

static int iconFlag( SystemTrayIcon::MessageIcon icon )
{
    switch (icon) {
        case SystemTrayIcon::Information:
            return NIIF_INFO;
        case SystemTrayIcon::Warning:
            return NIIF_WARNING;
        case SystemTrayIcon::Critical:
            return NIIF_ERROR;
        case SystemTrayIcon::NoIcon:
            return NIIF_NONE;
        case SystemTrayIcon::UserIcon:
            return NIIF_USER;
        default:
            Q_ASSERT_X(false, "SystemTrayIconSys::showMessage", "Invalid SystemTrayIcon::MessageIcon value");
            return NIIF_NONE;
    }
}

bool SystemTrayIconSys::showMessage(const QString &title, const QString &message, SystemTrayIcon::MessageIcon type, uint uSecs, QIcon pixmap)
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

    if (!pixmap.isNull())
    {
        tnd.dwInfoFlags = NIIF_USER;
        if (QSysInfo::windowsVersion() >= QSysInfo::WV_VISTA)
        {
            tnd.dwInfoFlags |= NIIF_LARGE_ICON;
        }
        int x  = GetSystemMetrics(SM_CXICON);
        int y  = GetSystemMetrics(SM_CYICON);
        tnd.hBalloonIcon = pixmap.pixmap(x, y).toWinHICON();
    }

    Q_ASSERT(testAttribute(Qt::WA_WState_Created));

    bool result = Shell_NotifyIcon(NIM_MODIFY, &tnd);

    if (hBalloonIcon)
        DestroyIcon(hBalloonIcon);

    hBalloonIcon = tnd.hBalloonIcon;

    return result;
}

bool SystemTrayIconSys::trayMessage(DWORD msg)
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

void SystemTrayIconSys::createIcon()
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

bool SystemTrayIconSys::winEvent( MSG *m, long *result )
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

void SystemTrayIconPrivate::install_sys()
{
    Q_Q(SystemTrayIcon);
    if (!sys) {
        sys = new SystemTrayIconSys(q);
        sys->createIcon();
        sys->trayMessage(NIM_ADD);
    }
}

/*
* This function tries to determine the icon geometry from the tray
*
* If it fails an invalid rect is returned.
*/
QRect SystemTrayIconSys::findIconGeometry(const int iconId)
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
            RECT iconRect = {0, 0};
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

void SystemTrayIconPrivate::showMessage_sys(const QString &title, const QString &message, SystemTrayIcon::MessageIcon type, int timeOut, QIcon pixmap)
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

    sys->showMessage(titleString, messageString, type, uSecs, pixmap);
}

QRect SystemTrayIconPrivate::geometry_sys() const
{
    if (!sys)
        return QRect();

    return sys->findIconGeometry(q_uNOTIFYICONID);
}

void SystemTrayIconPrivate::remove_sys()
{
    if (!sys)
        return;

    sys->trayMessage(NIM_DELETE);
    delete sys;
    sys = 0;
}

void SystemTrayIconPrivate::updateIcon_sys()
{
    if (!sys)
        return;

    HICON hIconToDestroy = sys->hIcon;

    sys->createIcon();
    sys->trayMessage(NIM_MODIFY);

    if (hIconToDestroy)
        DestroyIcon(hIconToDestroy);
}

void SystemTrayIconPrivate::updateMenu_sys()
{

}

void SystemTrayIconPrivate::updateToolTip_sys()
{
    if (!sys)
        return;

    sys->trayMessage(NIM_MODIFY);
}

bool SystemTrayIconPrivate::isSystemTrayAvailable_sys()
{
    return true;
}

bool SystemTrayIconPrivate::supportsMessages_sys()
{
    return allowsMessages();
}

#endif
