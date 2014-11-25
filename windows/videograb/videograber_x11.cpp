/* videograbber_x11.cpp
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#include "videograber_x11.h"
#include <QApplication>
#include <QDesktopWidget>
#include "utils.h"

// X11VideoGraber

X11VideoGraber::X11VideoGraber(QObject* parent)
    : QObject(parent)
    , m_mute(false)
    , m_maxSize(0)
{

}

X11VideoGraber::~X11VideoGraber()
{

}

QString X11VideoGraber::fileName() const
{
    return m_fileName;
}

QRect X11VideoGraber::rect() const
{
    return m_captureRect;
}

bool X11VideoGraber::isActive() const
{
    return m_ffmpeg.isActive();
}

bool X11VideoGraber::isMute() const
{
    return m_mute;
}

void X11VideoGraber::setMute(bool muted)
{
    m_mute = muted;
}

size_t X11VideoGraber::maxSize() const
{
    return m_maxSize;
}

void X11VideoGraber::setMaxSize(size_t size)
{
    m_maxSize = size;
}

bool X11VideoGraber::start(QRect rect, const QString& fileName, bool audio)
{
    audio = audio && !m_mute;

    if (rect.isNull())
    {
        rect = QApplication::desktop()->rect();
    }
    m_captureRect = rect;
    m_captureRect.setWidth(alignSize(rect.width(), 8, 80));
    m_captureRect.setHeight(alignSize(rect.height(), 8, 80));

    Q_ASSERT(m_captureRect.width() % 8 == 0);
    Q_ASSERT(m_captureRect.height() % 8 == 0);

    //TODO: what to do if rect outside screen boundiares

    m_fileName = fileName; // decorateFileName("Screencast", "flv", true)

    int bitrate = 1000000; // TODO: what bitrate should I use ?
    int gop = 20;

    emit startScreencast(m_fileName);

    return m_ffmpeg.screencast(m_fileName, m_captureRect, audio, m_maxSize);
}

void X11VideoGraber::stop()
{
    m_ffmpeg.stop();

    emit stopScreencast(m_fileName);
}

