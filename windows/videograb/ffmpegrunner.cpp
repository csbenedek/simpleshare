#include "ffmpegrunner.h"
#include <QDebug>

FFmpegRunner::FFmpegRunner(QObject *parent) :
    QObject(parent)
{
}

FFmpegRunner::~FFmpegRunner()
{
    //stop();
}

bool FFmpegRunner::screencast(const QString& outputName, QRect rect, bool audio, size_t maxFileSize)
{

    QStringList arguments;

    if (audio)
    {
        arguments << "-f" << "alsa" << "-ac" << "2" << "-i" << "hw:0,0";
    }

    if (maxFileSize > 0)
    {
        // arguments << "-fs" << QString::number(maxFileSize);
    }

    // ffmpeg -sameq -b 64K -f x11grab -r 25 -s widthxheight -i :0.0+left,top output.flv
    arguments << "-sameq" << "-b" << "64K" << "-f" << "alsa" << "-f" << "x11grab" << "-r" << "25"
              << "-s" << QString::number(rect.width()) + "x" + QString::number(rect.height())
              << "-i" << ":0.0+" + QString::number(rect.left()) + "," + QString::number(rect.top())
              << outputName;

    m_process.start("ffmpeg", arguments);
    return m_process.waitForStarted();
}


void FFmpegRunner::stop()
{
    if (isActive())
    {
        m_process.write("q\n", 2);
        m_process.waitForFinished();
        m_process.close();
    }
}

bool FFmpegRunner::isActive() const
{
    return m_process.state() == QProcess::Running;
}
