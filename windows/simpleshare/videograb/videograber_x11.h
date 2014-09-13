#ifndef X11VIDEOGRABER_H
#define X11VIDEOGRABER_H

#include <QString>
#include <QRect>
#include <QTimer>
#include "ffmpegrunner.h"

class X11VideoGraber: public QObject
{
    Q_OBJECT

public:
    explicit X11VideoGraber(QObject* parent = 0);
    virtual ~X11VideoGraber();

    bool start(QRect rect, const QString& fileName, bool audio = true);
    void stop();

    QString fileName() const;
    QRect   rect() const;
    bool    isActive() const;
    bool    isMute() const;
    void    setMute(bool muted);
    size_t  maxSize() const;
    void    setMaxSize(size_t size);

signals:
    void startScreencast(QString fileName);
    void stopScreencast(QString fileName);

private:
    QRect        m_captureRect;
    QString      m_fileName;
    bool         m_mute;
    FFmpegRunner m_ffmpeg;
    size_t       m_maxSize;
};

#endif // X11VIDEOGRABER_H
