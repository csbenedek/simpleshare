/* graber.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef GRABER_H
#define GRABER_H

#include <QPixmap>
#include <QPoint>
#include <QRect>

class Graber
{
public:
    Graber();

    static void delay(unsigned long time);
    static QRect getVirtualDesktopGeometry();

    static QRect doSelectRect(bool video = false, bool inverted = false, bool cursor = false);
    static QPixmap grabVirtualDesktop();
    static QPixmap grabDesktop(bool cursor = true);
#ifdef Q_OS_WIN
    static QPixmap grabWindow(WId id, bool cursor = false);
#endif
    static QPixmap grabActiveWindow(bool inverted = false, bool cursor = false);
    static QPixmap grabRegion(bool inverted = false, bool cursor = false);
    static QPixmap grabRect(QRect rect, bool cursor = false);
private:
#ifdef Q_OS_WIN
    static QPixmap grabWindow_Win(bool cursor = false);
    static QPixmap grabWindow_Win(HWND hWnd, bool cursor);
    static QPixmap grabRect_Win(QRect rect, bool cursor = false);
    static void grabCursor_Win(HDC hdc, QPoint offset);
#endif
#ifdef Q_OS_MAC
    static QPixmap grabDesktop_Mac();
    static QPixmap grabWindow_Mac();
#endif
};

#endif // GRABER_H
