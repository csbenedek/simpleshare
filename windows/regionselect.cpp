
/***************************************************************************
 *   Copyright (C) 2009 by Artem 'DOOMer' Galichkin                        *
 *   doomer3d@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QDebug>
#include "regionselect.h"
#include "applicationex.h"
#include "graber.h"
#include "utils.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include "qxtwindowsystem.h"

RegionSelect::RegionSelect(bool unused, bool cursor, int delta, QWidget *parent, int monitorIndex, RegionSelect* root, QPoint offset)
    : QDialog(parent)
    , m_selEndPoint(-1,-1)
    , m_delta(delta)
    , m_doubleClicked(false)
    , m_offset(offset)
#ifdef Q_OS_LINUX
    , m_root(root)
#endif
    , m_pressed(false)
{
    if (m_delta > 1)
    {
        m_minimumSize = QSize(m_delta*10, m_delta*10);
    }

    if(QApplication::desktop()->screenCount() == 1
            || QApplication::desktop()->screenGeometry() == Graber::getVirtualDesktopGeometry())
    {
        //WId desktop=QApplication::desktop()->winId();

        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
        setWindowState(Qt::WindowFullScreen);
        setCursor(Qt::CrossCursor);

        QRect geometry=Graber::getVirtualDesktopGeometry();
        resize(geometry.size());

        m_desktopPixmapBkg = Graber::grabDesktop(cursor);

        ///desktopPixmapBkg = QPixmap::grabWidget(QApplication::desktop());
        if(m_desktopPixmapBkg.isNull())
        	qDebug("desktopPixmapBkg is null");
        m_desktopPixmapClr = m_desktopPixmapBkg;

        move(0, 0);
        drawBackGround();
    }
    else
    {
        setWindowFlags(Qt::FramelessWindowHint |
                       Qt::WindowStaysOnTopHint );
        setWindowState(Qt::WindowFullScreen);
        setCursor(Qt::CrossCursor);

#ifndef Q_OS_LINUX
        QRect geometry=Graber::getVirtualDesktopGeometry();
        resize(geometry.size());

        m_desktopPixmapBkg= Graber::grabDesktop(cursor);
        ///desktopPixmapBkg = QPixmap::grabWidget(QApplication::desktop());
        if(m_desktopPixmapBkg.isNull())
        {
            qDebug("desktopPixmapBkg is null");
        }
        m_desktopPixmapClr = m_desktopPixmapBkg;

        int x=geometry.left();
        int y=geometry.y();
        move(x,y);
        m_offset = geometry.topLeft();
        drawBackGround();
#else
        QRect screenrect = Graber::getVirtualDesktopGeometry();
        if (monitorIndex == -1)
        {
            //QRect screenrect = QApplication::desktop()->screenGeometry(1);
            m_desktopPixmapBkg= Graber::grabDesktop(cursor);
        }
        else
        {
            screenrect = QApplication::desktop()->screenGeometry(monitorIndex);
            m_desktopPixmapBkg= Graber::grabRect(screenrect, cursor);
        }

        if(m_desktopPixmapBkg.isNull())
        {
            qDebug("desktopPixmapBkg is null");
        }
        m_desktopPixmapClr = m_desktopPixmapBkg;

        move(QPoint(screenrect.x(), screenrect.y()));
        resize(screenrect.width(), screenrect.height());
        setWindowState(Qt::WindowFullScreen);
        drawBackGround();

        if (monitorIndex == -1)
        {
            for (int i = 0; i <= QApplication::desktop()->screenCount(); ++i)
            {
                if (i != QApplication::desktop()->primaryScreen())
                {
                    RegionSelect* additionalRegionSelect = new RegionSelect(unused, cursor, delta, NULL, i, this, screenrect.topLeft());
                    additionalRegionSelect->setMouseTracking(true);
                    additionalRegionSelect->show();
                    m_regions.append(additionalRegionSelect);
                }
            }           
        }
        else
        {
            m_offset = screenrect.topLeft();
        }
#endif

    }

    setMouseTracking(true);
}

RegionSelect::~RegionSelect()
{
#ifdef Q_OS_LINUX
    foreach(RegionSelect* region, m_regions)
    {
        delete region;
    }
#endif
}

#ifdef Q_OS_LINUX

void RegionSelect::sync()
{
    foreach(RegionSelect* region, m_regions)
    {
        region->syncSelectionData(m_selectRect, m_currentPoint, m_selStartPoint, m_selEndPoint);
    }
}


void RegionSelect::syncSelectionData(QRect selectRect, QPoint currentPoint, QPoint  selStartPoint, QPoint selEndPoint)
{
    m_selectRect    = selectRect;
    m_currentPoint  = currentPoint;
    m_selStartPoint = selStartPoint;
    m_selEndPoint   = selEndPoint;

    update();
}

#endif


bool RegionSelect::mouseReleased()
{
    m_pressed = false;
    if (!m_doubleClicked && !m_selectRect.isEmpty())
    {
        accept();
        return true;
    }
    return false;
}

void RegionSelect::mouseMoved(QPoint pos)
{
    if (m_pressed)
    {
        m_currentPoint = pos;

        if (!m_doubleClicked)
        {
            if (m_selEndPoint == QPoint(-1,-1))
            {
                m_selStartPoint  = m_currentPoint;
                m_selEndPoint  = m_currentPoint;
            }
            else
            {
                // align rect
                if (m_delta > 1)
                {
                    int width = alignSize(pos.x() - m_selStartPoint.x(), m_delta, 80);
                    int height = alignSize(pos.y() - m_selStartPoint.y(), m_delta, 80);
                    m_selEndPoint = QPoint(m_selStartPoint.x() + width, m_selStartPoint.y() + height);
                }
                else
                {
                    m_selEndPoint = m_currentPoint;
                    // TODO: minimumSize ignored here.
                }
                m_selectRect = QRect(m_selStartPoint, QPoint(m_selEndPoint.x()-1, m_selEndPoint.y()-1)).normalized();
#ifdef Q_OS_LINUX
                sync();
#endif
            }
        }
        update();
    }
}

bool RegionSelect::mousePressed(QPoint pos, Qt::MouseButton button)
{
    if (button == Qt::RightButton)
    {
        reject();
        return true;
    }
    else if (button == Qt::LeftButton)
    {
        if (!m_doubleClicked)
        {
            if (button != Qt::LeftButton)
            {
                reject();
                return true;
            }

            m_pressed = true;
            m_selStartPoint = pos;
            m_selEndPoint = pos;
            m_selectRect = QRect(m_selStartPoint, QPoint(m_selEndPoint.x()-1, m_selEndPoint.y()-1)).normalized();
            update();
#ifdef Q_OS_LINUX
            sync();
#endif
        }
    }
    return false;
}

bool RegionSelect::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        if (mouseReleased())
        {
            event->accept();
        }
    }
    else if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        mouseMoved(mouseEvent->pos());

    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mousePressed(mouseEvent->pos(), mouseEvent->button()))
        {
            event->accept();
        }
    }
    else if (event->type() == QEvent::MouseButtonDblClick)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton)
        {
            reject();
            event->accept();
        }
        if (!m_doubleClicked)
        {
            m_currentPoint = mouseEvent->pos();
            m_doubleClicked = true;
            hide();
            //setWindowOpacity(0);

            //doSelectWindow();
            //accept();

            event->accept();
        }
    }
    else
    {
        return QDialog::event(event);
    }

    return true;
}


void RegionSelect::showEvent(QShowEvent * event)
{
    Q_UNUSED(event);

#ifdef Q_OS_LINUX
    if (m_root == NULL && m_regions.count() > 0)
    {
        ApplicationEx* app = dynamic_cast<ApplicationEx*>(qApp);
        Q_ASSERT(app);
        if (app)
        {
            app->setMouseListener(this);
        }
    }
#endif

}

void RegionSelect::hideEvent(QHideEvent * event)
{
    Q_UNUSED(event);

    if (m_doubleClicked)
    {
        doSelectWindow();
        accept();
        m_doubleClicked = false;
    }

#ifdef Q_OS_LINUX
    if (m_root == NULL && m_regions.count() > 0)
    {
        ApplicationEx* app = dynamic_cast<ApplicationEx*>(qApp);
        Q_ASSERT(app);
        if (app)
        {
            app->resetMouseListener();
        }
    }
#endif

}

void RegionSelect::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
    case Qt::Key_Escape:
        reject();
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Space:
        accept();
        break;
    }
}

void RegionSelect::paintEvent(QPaintEvent * /*event*/ )
{
    QPainter painter(this);

    if (!m_palBackground)
    {
        painter.drawPixmap(-m_offset, m_desktopPixmapBkg);
    }

    drawRectSelection(painter);
}

void RegionSelect::doSelectWindow()
{
    const QRect geometry = Graber::getVirtualDesktopGeometry();
    QPoint p(m_currentPoint.x() + geometry.left(), m_currentPoint.y() + geometry.top());

#ifdef Q_OS_WIN
    POINT point = { p.x(), p.y() };
    HWND hWnd = ChildWindowFromPointEx(GetDesktopWindow(), point, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
    if (hWnd)
    {
        //activateWindow(hWnd);
        //m_desktopPixmapClr = Graber::grabDesktop(m_cursor);
        RECT rect;
        if (!GetWindowRect(hWnd, &rect))
        {
            return;
        }
        m_selectRect = QRect(rect.left, rect.top,
                             rect.right - rect.left,
                             rect.bottom - rect.top);
    }
#else
    WId wid = QxtWindowSystem::windowAt(p);
    m_selectRect = QxtWindowSystem::windowGeometry(wid);
#endif

    m_selectRect.translate(-geometry.left(), -geometry.top());

    m_selStartPoint = m_selectRect.topLeft();

    if (m_delta > 1)
    {
        int width = alignSize(m_selectRect.width(), m_delta, m_minimumSize.width());
        int height = alignSize(m_selectRect.height(), m_delta, m_minimumSize.height());
        m_selEndPoint = QPoint(m_selStartPoint.x() + width - 1, m_selStartPoint.y() + height - 1);
        m_selectRect.setBottomRight(m_selEndPoint);
    }
    else
    {
        m_selEndPoint   = m_selectRect.bottomRight();
    }

}

void RegionSelect::drawBackGround()
{
    // create painter on  pixelmap of desktop
    QPainter painter(&m_desktopPixmapBkg);

    QRect geometry=Graber::getVirtualDesktopGeometry();
    //geometry.translate(-geometry.left(), -geometry.top());
    geometry.translate(-m_offset);

    // set painter brush on 75% transparency
    painter.setBrush(QBrush(QColor(0, 0, 50, 75), Qt::SolidPattern));

    // draw rect of desktop size in painter
    painter.drawRect(geometry);

#ifdef Q_OS_LINUX
    if (m_root == NULL)
#endif
    {
        QRect txtRect = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
        QString txtTip = QApplication::tr("Select a region with the mouse, right click to cancel.");

        txtRect.setHeight(qRound(txtRect.height() / 10)); // rounded val of text rect height

        painter.setPen(QPen(Qt::darkRed)); // ste message rect border color
        painter.setBrush(QBrush(QColor(255, 255, 255, 180), Qt::SolidPattern));
        QRect txtBgRect = painter.boundingRect(txtRect, Qt::AlignCenter, txtTip);

        // set height & width of bkg rect
        txtBgRect.setX(txtBgRect.x() - 6);
        txtBgRect.setY(txtBgRect.y() - 4);
        txtBgRect.setWidth(txtBgRect.width() + 12);
        txtBgRect.setHeight(txtBgRect.height() + 8);

        painter.drawRect(txtBgRect);

        // Draw the text
        painter.setPen(QPen(Qt::black)); // black color pen
        painter.drawText(txtBgRect, Qt::AlignCenter, txtTip);
    }

    m_palBackground = (qApp->desktop()->numScreens() > 1);

    // set bkg to pallette widget
    if (m_palBackground)
    {
        QPalette newPalette = palette();
        newPalette.setBrush(QPalette::Window, QBrush(m_desktopPixmapBkg));
        setPalette(newPalette);
    }
}

void RegionSelect::drawRectSelection(QPainter &painter)
{
    QRect rect = m_selectRect;
    rect.translate(-m_offset);

    painter.drawPixmap(rect, m_desktopPixmapClr, rect);
    painter.setPen(QPen(QBrush(QColor(0, 0, 0, 255)), 2));
    painter.drawRect(rect);

    QString txtSize = QApplication::tr("%1 x %2 pixels ").arg(rect.width()).arg(rect.height());
    painter.drawText(rect, Qt::AlignBottom | Qt::AlignRight, txtSize);
}

QPixmap RegionSelect::getSelection()
{
    QPixmap sel;
    sel = m_desktopPixmapClr.copy(m_selectRect);
    return sel;
}

QRect RegionSelect::getSelectionRect() const
{
    return m_selectRect;
}

QPixmap& RegionSelect::getBackgroundImage()
{
    return m_desktopPixmapClr;
}
