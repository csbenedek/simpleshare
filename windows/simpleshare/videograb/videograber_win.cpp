#include "videograber_win.h"
#include "utils.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QElapsedTimer>
#include "graber.h"
#include <QDebug>
#include <QPainter>

// VideoGraber

VideoGraber::VideoGraber(QObject* parent)
    : QObject(parent)
    , m_videoEncoder(NULL)
    , m_audioGraber(NULL)
    , m_mute(false)
    , m_maxSize(0)
{
    m_audioGraber = new AudioGraber();
    connect(m_audioGraber, SIGNAL(processBuffer(short int*,int)), this, SLOT(processAudioBuffer(short int*,int)));

    connect(&m_timer, SIGNAL(timeout()), SLOT(captureFrame()));
}

VideoGraber::~VideoGraber()
{    
    m_timer.stop();
    delete m_videoEncoder;
    delete m_audioGraber;
}

QString VideoGraber::fileName() const
{
    return m_fileName;
}

int VideoGraber::frameTime() const
{
    Q_ASSERT(m_videoEncoder);
    if (m_videoEncoder)
    {
        return m_videoEncoder->frameTime();
    }
    else
    {
        return 0;
    }
}

QRect VideoGraber::rect() const
{
    return m_captureRect;
}

bool VideoGraber::isActive() const
{
    return m_videoEncoder != NULL;
}

bool VideoGraber::isMute() const
{
    return m_mute;
}

void VideoGraber::setMute(bool muted)
{
    m_mute = muted;
}

size_t VideoGraber::maxSize() const
{
    return m_maxSize;
}

void VideoGraber::setMaxSize(size_t size)
{
    m_maxSize = size;
}

bool VideoGraber::start(QRect rect, const QString& fileName, bool audio)
{
    m_totalSize = 0;
    m_frame = 0;
    stop();

    int fps = 25;

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

    QElapsedTimer timer;
    timer.start();
    QPixmap pix = Graber::grabRect(m_captureRect, true);
    qint64 elapsed = timer.elapsed() + 75;
    if (elapsed > 0)
    {
        fps = 1000 / elapsed;
        if (fps == 0)
        {
            fps = 1;
        }
    }
    else
    {
        fps = 25;
    }
    if (fps > 25)
    {
        fps = 25; // doesn't need more
    }

    Q_ASSERT(m_videoEncoder == NULL);
    if (m_videoEncoder)
    {
        delete m_videoEncoder;
    }
    m_videoEncoder = new VideoEncoder();
    bool created = m_videoEncoder->createFile(
                m_fileName,
                m_captureRect.width(),
                m_captureRect.height(), bitrate, gop, fps, audio);

    if (!created)
    {
        delete m_videoEncoder;
        m_videoEncoder = NULL;
        return false;
    }

    if (audio && m_audioGraber)
    {
        qDebug() << "AUDIO: samples buff size = " << m_videoEncoder->audioSamlesFrameSize();
        m_audioGraber->start(m_videoEncoder->audioSamlesFrameSize());
    }

    emit startScreencast(m_fileName);
    m_timer.start(m_videoEncoder->frameTime());

    return true;
}

void VideoGraber::stop()
{
    m_timer.stop();

    if (m_audioGraber && m_audioGraber->isActive())
    {
        m_audioGraber->stop();
    }

    if (m_videoEncoder)
    {
        qDebug() << "stop capturing " << m_fileName;
        m_videoEncoder->close();        
        delete m_videoEncoder;
        m_videoEncoder = NULL;            
    }

    emit stopScreencast(m_fileName);
}

void VideoGraber::captureFrame()
{
    // TODO: check disk full and other situations

    Q_ASSERT(m_videoEncoder);
    if (m_videoEncoder == NULL)
    {
        return;
    }

    m_frame++;

    QPixmap pix;
    //QElapsedTimer timer;
    if (m_captureRect.isNull()) // if rect not set, capture all desktop
    {
        pix = Graber::grabRect(QRect(), true);
    }
    else
    {
        pix = Graber::grabRect(m_captureRect, true);
    }
    Q_ASSERT(!pix.isNull());
    if (pix.isNull())
    {
        qDebug() << "can't capture frame";
        return;
    }
    qDebug() << "  capture: " << m_captureRect; //timer.elapsed();

    // The image on which we draw the frames
    QImage frame(m_captureRect.width(), m_captureRect.height(), QImage::Format_ARGB32);     // Only RGB32 and ARG32 are supported

    // helper painter
    QPainter painter(&frame);
    //painter.setBrush(Qt::red);
    //painter.setPen(Qt::white);

    //painter.fillRect(frame.rect(), Qt::red);

    painter.drawPixmap(0, 0, pix);

    //qDebug() << "  drawing: " << timer.elapsed();

    // Frame number
    //painter.drawText(frame.rect() ,Qt::AlignCenter, QString("Frame %1\nLast frame was %2 bytes").arg(m_frame).arg(m_size));

    int size = m_videoEncoder->encodeImage(frame);
    //qDebug() << "  encoded:" << timer.elapsed();

    m_totalSize += size;
    if ((m_maxSize > 0) && (m_totalSize > m_maxSize))
    {
        emit maximumSizeReached();
    }

    qDebug() << "frame " << m_frame << " " << size << " bytes";

}

void VideoGraber::processAudioBuffer(short int* buff, int size)
{
    if (m_audioGraber && m_videoEncoder && buff && size > 0)
    {
        if (m_mute)
        {
            // TODO: is this an ugly hack ?

            memset(buff, 0, size);
        }
        int sz = m_videoEncoder->encodeAudioFrame(size, buff);

        m_totalSize += sz;
        if ((m_maxSize > 0) && (m_totalSize > m_maxSize))
        {
            emit maximumSizeReached();
        }
    }
}
