#ifndef AUDIOGRABER_H
#define AUDIOGRABER_H

#include <QObject>
#include <windows.h>
#include <mmsystem.h>

const int MAX_BUFFERS = 16;

class AudioGraber : public QObject
{
    Q_OBJECT
public:
    explicit AudioGraber(QObject *parent = 0);
    AudioGraber(int sampleRate, int channels, int bits, QObject *parent = 0);
    virtual ~AudioGraber();

    bool    isActive() const;

    bool start(int buffSize);
    void stop();

signals:
    void processBuffer(short int* buff, int size);

public slots:

protected:
    void    InitWavFormat();
    static void CALLBACK waveInProc(
        HWAVEIN hwi,
        UINT uMsg,
        DWORD_PTR dwInstance,
        DWORD_PTR dwParam1,
        DWORD_PTR dwParam2
    );
    void processHeader(WAVEHDR* wavHdr);
    void prepareBuffers(int buffSize);
    void unPrepareBuffers();

private:
    int m_sampleRate;
    int m_channels;
    int m_bits;
    WAVEHDR         m_stWHDR[MAX_BUFFERS];
    HWAVEIN         m_hWavIn;
    WAVEFORMATEX    m_format;
    bool            m_working;
};

#endif // AUDIOGRABER_H
