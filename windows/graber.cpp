#include "graber.h"
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include "regionselect.h"
#include "qxtwindowsystem.h"
#include <QMessageBox>
#ifdef Q_OS_MAC
#include <Carbon/Carbon.h>
#endif
#ifdef Q_OS_WIN
#define _WIN32_WINNT  0x0500
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#endif

// Graber

Graber::Graber()
{

}

QRect Graber::getVirtualDesktopGeometry()
{
    QRect rect = QApplication::desktop()->geometry();
    return rect;
}

QPixmap Graber::grabDesktop(bool cursor)
{
#ifdef Q_WS_MAC
    return grabDesktop_Mac();//TODO dual screen suport
#endif
    QPixmap pixmap;

    pixmap = grabRect(getVirtualDesktopGeometry(), cursor);
    if (!pixmap.isNull())
    {
        return pixmap;
    }

    WId desktop = QApplication::desktop()->winId();
    pixmap = QPixmap::grabWindow(desktop);

    if (pixmap.isNull())
    {
        qDebug("QPixmap::grabWidget(QApplication::desktop()) returned a null image");
    }

    return pixmap;
}

QRect Graber::doSelectRect(bool video, bool inverted, bool cursor)
{
    Q_UNUSED(inverted);

    QRect result;    
    RegionSelect* selector = video
        ? new RegionSelect(true, false, 8)
        : new RegionSelect(true, cursor);
    Q_ASSERT(selector);
    if (selector)
    {
        //selector->setInverted(inverted);
        const int ret = selector->exec();
        if (ret == QDialog::Accepted)
        {
            result = selector->getSelectionRect();
        }
        delete selector;
    }
    return result;
}

QPixmap Graber::grabRegion(bool inverted, bool cursor)
{
    Q_UNUSED(inverted);

    QPixmap result;
    RegionSelect* selector = new RegionSelect(true, cursor);
    Q_ASSERT(selector);
    if (selector)
    {
        //selector->setInverted(inverted);
        const int ret = selector->exec();
        if (ret == QDialog::Accepted)
        {
            const QRect rect = selector->getSelectionRect();
            if (rect.isEmpty())
            {
                result = QPixmap();
            }
            else if (selector->getBackgroundImage().isNull())
            {
                selector->hide();
                result = grabRect(rect, cursor);
            }
            else
            {
                result = selector->getBackgroundImage().copy(rect);
            }
        }
        delete selector;
    }
    return result;
}

QPixmap Graber::grabRect(QRect rect, bool cursor)
{
    QPixmap pixmap;

    if (rect.isNull())
    {
        rect = getVirtualDesktopGeometry();
    }

#ifdef Q_WS_WIN
    pixmap = grabRect_Win(rect, cursor);
    if (!pixmap.isNull())
    {
        return pixmap;
    }
#endif

    pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(),
                                 rect.left(), rect.top(),
                                 rect.width(), rect.height());

    if (cursor && !pixmap.isNull())
    {
        QPoint cursorPos = QCursor::pos() - rect.topLeft();// - QCursor::hotSpot();
        QPixmap s_cursorPixmap;// = QCursor::pixmap();
        if (s_cursorPixmap.isNull())
        {
            s_cursorPixmap.load(":/res/cursor.png");
        }

        if (!s_cursorPixmap.isNull())
        {
            QPainter painter(&pixmap);
            painter.drawPixmap(cursorPos, s_cursorPixmap);
        }
    }

    if (pixmap.isNull())
    {
        qDebug("QPixmap::grabWidget(QApplication::desktop()) returned a null image");
    }

    return pixmap;
}

#ifdef Q_WS_WIN
QPixmap Graber::grabWindow(WId id, bool cursor)
{
    return grabWindow_Win(id, cursor);
}
#endif


QPixmap Graber::grabActiveWindow(bool inverted, bool cursor)
{
    QPixmap screenshot;
#ifdef Q_WS_MAC
    screenshot = grabWindow_Mac();
#endif
#ifdef Q_WS_WIN
    screenshot = grabWindow_Win(cursor);
#endif
    if (screenshot.isNull())
    {
        WId wid = QxtWindowSystem::activeWindow();
        QRect rect = QxtWindowSystem::windowGeometry(wid);
        QPixmap screenshot = grabRect(rect, cursor);
    }

    if (!screenshot.isNull())
    {
        return screenshot;
    }
    else
    {
        //if it fails try with Region
        return grabRegion(inverted);
    }
}

void Graber::delay(unsigned long time)
{
    QMutex mutex;
    mutex.lock();
    QWaitCondition pause;
    pause.wait(&mutex, time);
}

#ifdef Q_OS_WIN

QPixmap Graber::grabWindow_Win(bool cursor)
{
    return grabWindow_Win(GetForegroundWindow(), cursor);
}

QPixmap Graber::grabWindow_Win(HWND hWnd, bool cursor)
{
    if (hWnd)
    {
        RECT rect;
        if (GetWindowRect(hWnd, &rect))
        {
            return grabRect(QRect(rect.left, rect.top,
                                rect.right - rect.left,
                                rect.bottom - rect.top), cursor);
        }
    }

    return QPixmap();
}


QPixmap Graber::grabRect_Win(QRect rect, bool cursor)
{
    QPixmap pixmap;

    Q_ASSERT(!rect.isNull());

    HDC hdcScreen = GetDC(NULL);

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmCapture = CreateCompatibleBitmap(hdcScreen, rect.width(), rect.height());
    SelectObject(hdcMem, hbmCapture);

    BitBlt(hdcMem, 0, 0, rect.width(), rect.height(), hdcScreen, rect.left(), rect.top(), SRCCOPY);

    ReleaseDC(NULL, hdcScreen);

    if (cursor)
    {
        grabCursor_Win(hdcMem, QPoint(rect.left(), rect.top()));
    }

    ReleaseDC(NULL, hdcMem);
    DeleteDC(hdcMem);

    pixmap = QPixmap::fromWinHBITMAP(hbmCapture);

    DeleteObject(hbmCapture);

    return pixmap;
}

void Graber::grabCursor_Win(HDC hdc, QPoint offset)
{
    CURSORINFO cursorInfo;
    cursorInfo.cbSize = sizeof(cursorInfo);

    if (!GetCursorInfo(&cursorInfo))
    {
        qDebug() << "can't get cursor info";
        return;
    }

    if (cursorInfo.flags == CURSOR_SHOWING)
    {
        ICONINFO iconInfo;
        if (GetIconInfo(cursorInfo.hCursor, &iconInfo))
        {
            // Allign cursor location to Bitmap coordinates (instead of Screen coordinates)
            int x = cursorInfo.ptScreenPos.x - iconInfo.xHotspot - offset.x();
            int y = cursorInfo.ptScreenPos.y - iconInfo.yHotspot - offset.y();

            DrawIcon(hdc, x, y, cursorInfo.hCursor);

            // GetIconInfo creates bitmaps for the hbmMask and hbmColor members of ICONINFO. The calling application must manage these bitmaps and delete them when they are no longer necessary.
            if (iconInfo.hbmMask)
            {
                DeleteObject(iconInfo.hbmMask);
            }
            if (iconInfo.hbmColor)
            {
                DeleteObject(iconInfo.hbmColor);
            }
        }
    }
}
#endif

#ifdef Q_WS_MAC
QPixmap Graber::grabWindow_Mac()
{
    if(!AXAPIEnabled())
    {
        QMessageBox::warning(NULL, QObject::tr("Error"), QObject::tr("Please enable access for assistive devices on Universal Access preferences to enable active window capture."));
        return NULL;
    }

    pid_t pid;
    ProcessSerialNumber psn;

    AXValueRef temp;
    CGSize windowSize;
    CGPoint windowPosition;
    AXUIElementRef frontMostApp;
    AXUIElementRef frontMostWindow;

    GetFrontProcess(&psn);
    GetProcessPID(&psn, &pid);
    frontMostApp = AXUIElementCreateApplication(pid);

    AXUIElementCopyAttributeValue(frontMostApp, kAXFocusedWindowAttribute, (CFTypeRef *)&frontMostWindow);
    if (!frontMostWindow)
    {
        QString message = QString("Please select an active window before taking a screenshot.");
        QMessageBox::warning(NULL, QObject::tr("Error"), message);
        return NULL;
    }

    /* Get the window size and position */
    AXUIElementCopyAttributeValue(frontMostWindow, kAXSizeAttribute, (CFTypeRef *)&temp);
    AXValueGetValue(temp, kAXValueCGSizeType, &windowSize);
    CFRelease(temp);

    AXUIElementCopyAttributeValue(frontMostWindow, kAXPositionAttribute, (CFTypeRef *)&temp);
    AXValueGetValue(temp, kAXValueCGPointType, &windowPosition);
    CFRelease(temp);

    CFRelease(frontMostWindow);
    CFRelease(frontMostApp);

    CGImageRef windowImage = CGWindowListCreateImage(CGRectMake(windowPosition.x, windowPosition.y, windowSize.width, windowSize.height),
                                                                                                         kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageShouldBeOpaque);
    return QPixmap::fromMacCGImageRef(windowImage);
}

QPixmap Graber::grabDesktop_Mac()
{
        CGImageRef windowImage = CGWindowListCreateImage(CGRectInfinite, kCGWindowListOptionOnScreenOnly,
                                                         kCGNullWindowID, kCGWindowImageShouldBeOpaque);

        return QPixmap::fromMacCGImageRef(windowImage);
}
#endif



