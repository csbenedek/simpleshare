/* videograbber_win.h
 * Author: Evgeniy Sergeev, <evgeniy.sereev@gmail.com>
 */

#ifndef VIDEOGRABER_H
#define VIDEOGRABER_H

#include <QObject>
#include <QString>
#include <QRect>
#include <QTimer>
#include "videoencoder.h"
#include "audiograber_win.h"

class VideoGraber : public QObject
{
    Q_OBJECT

public:
    explicit VideoGraber(QObject* parent = 0);
    virtual ~VideoGraber();

    bool start(QRect rect, const QString& fileName, bool audio = true);
    void stop();

    int     frameTime() const;
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
    void maximumSizeReached();

private slots:
    void captureFrame();
    void processAudioBuffer(short int* buff, int size);

private:
    int                 m_frame;
    QTimer              m_timer;
    QRect               m_captureRect;
    VideoEncoder*       m_videoEncoder;
    QString             m_fileName;
    AudioGraber*        m_audioGraber;
    bool                m_mute;
    size_t              m_totalSize;
    size_t              m_maxSize;
};

#endif // VIDEOGRABER_H
