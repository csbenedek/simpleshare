#ifndef FFMPEGRUNNER_H
#define FFMPEGRUNNER_H

#include <QObject>
#include <QRect>
#include <QProcess>

class FFmpegRunner : public QObject
{
    Q_OBJECT
public:
    explicit FFmpegRunner(QObject *parent = 0);
    virtual ~FFmpegRunner();

    bool screencast(const QString& outputName, QRect rect, bool audio, size_t maxFileSize);
    void stop();
    bool isActive() const;

signals:

public slots:

private:
    QProcess m_process;

};

#endif // FFMPEGRUNNER_H
