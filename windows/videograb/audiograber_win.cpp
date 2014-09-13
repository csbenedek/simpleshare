#include "audiograber_win.h"

#include <aviriff.h>
#include "riff.h"
#include <QDebug>


AudioGraber::AudioGraber(QObject *parent)
    : QObject(parent)
    , m_sampleRate(11025)
    , m_channels(1)
    , m_bits(16)
    , m_hWavIn(NULL)
{

}

AudioGraber::AudioGraber(int sampleRate, int channels, int bits, QObject *parent)
    : QObject(parent)
    , m_sampleRate(sampleRate)
    , m_channels(channels)
    , m_bits(bits)
    , m_hWavIn(NULL)
{

}

AudioGraber::~AudioGraber()
{
    if (m_hWavIn)
    {
        stop();
    }
}

void AudioGraber::processHeader(WAVEHDR* wavHdr)
{
    MMRESULT ret = 0;

    if (wavHdr && m_hWavIn)
    {
        qDebug() << "AUDIO: wavHdr->dwUser = " << wavHdr->dwUser;
        if (wavHdr->dwFlags & WHDR_DONE)
        {
            if (wavHdr->lpData && wavHdr->dwBytesRecorded > 0)
            {
                emit processBuffer(reinterpret_cast<short int*>(wavHdr->lpData), wavHdr->dwBytesRecorded);

                ret = waveInAddBuffer(m_hWavIn, wavHdr, sizeof(WAVEHDR));
                if (ret != MMSYSERR_NOERROR)
                {
                    qDebug() << "AUDIO: waveInAddBuffer failed";
                }
            }
        }
    }
}

void CALLBACK AudioGraber::waveInProc(
    HWAVEIN hWavIn,
    UINT uMsg,
    DWORD_PTR dwInstance,
    DWORD_PTR dwParam1,
    DWORD_PTR dwParam2
)
{
    AudioGraber* This = reinterpret_cast<AudioGraber*>(dwInstance);
    if (This == NULL || This->m_hWavIn == NULL)
    {
        return;
    }

    switch(uMsg)
    {
    case WIM_CLOSE:
        break;
    case WIM_DATA:
        This->processHeader(reinterpret_cast<WAVEHDR*>(dwParam1));
        qDebug() << "AUDIO: waveInProc WIM_DATA -> processHeader";
        break;
    case WIM_OPEN:
        break;
    default:
        break;
    }
}

bool AudioGraber::isActive() const
{
    return (m_hWavIn != NULL);
}

bool AudioGraber::start(int buffSize)
{
    MMRESULT ret;

    InitWavFormat();    

    if (waveInGetNumDevs() == 0)
    {
        qDebug() << "AUDIO: Audio device not found";
        return false;
    }

    ret = waveInOpen(&m_hWavIn, WAVE_MAPPER, &m_format, 0, 0, WAVE_FORMAT_QUERY);
    if (ret != MMSYSERR_NOERROR)
    {
        qDebug() << "AUDIO: Unsupported WAV format";
        return false;
    }

    ret = waveInOpen(&m_hWavIn, WAVE_MAPPER, &m_format, reinterpret_cast<DWORD>(&waveInProc), reinterpret_cast<DWORD>(this), CALLBACK_FUNCTION);
    if (ret != MMSYSERR_NOERROR)
    {
        char fault[256];
        waveInGetErrorTextA(ret, fault, 256);
        qDebug() << fault << "AUDIO: Failed to open waveform input device.";
        return false;
    }

    prepareBuffers(buffSize);


    // Recording now

    ret = waveInStart(m_hWavIn);
    if (ret != MMSYSERR_NOERROR)
    {
        qDebug() << "AUDIO: Failed to start recording";
        return false;
    }

    return true;
}

void AudioGraber::stop()
{
    MMRESULT ret = 0;

    if(m_hWavIn)
    {
        ret = waveInClose(m_hWavIn);
        unPrepareBuffers();
    }
    m_hWavIn = NULL;
}

void AudioGraber::InitWavFormat()
{
    m_format.wFormatTag         = WAVE_FORMAT_PCM;
    m_format.nChannels          = m_channels;
    m_format.nSamplesPerSec     = m_sampleRate;
    m_format.wBitsPerSample     = m_bits;
    m_format.nBlockAlign        = m_format.nChannels * m_format.wBitsPerSample/8;
    m_format.nAvgBytesPerSec    = m_format.nSamplesPerSec * m_format.nBlockAlign;
    m_format.cbSize             = 0;
}

void AudioGraber::prepareBuffers(int buffSize)
{
    MMRESULT ret = 0;

    if (buffSize == 0)
    {
        buffSize = m_format.nAvgBytesPerSec;
    }

    for(int i = 0; i<MAX_BUFFERS; ++i)
    {
        m_stWHDR[i].lpData              = (LPSTR)HeapAlloc(GetProcessHeap(), 8, buffSize);
        m_stWHDR[i].dwBufferLength      = buffSize;
        m_stWHDR[i].dwUser              = i;
        m_stWHDR[i].dwBytesRecorded     = 0;
        m_stWHDR[i].dwUser              = 0;
        m_stWHDR[i].dwFlags             = 0;
        m_stWHDR[i].dwLoops             = 1;
        m_stWHDR[i].lpNext              = 0;
        m_stWHDR[i].reserved            = 0;

        ret = waveInPrepareHeader(m_hWavIn, &m_stWHDR[i], sizeof(WAVEHDR));
        if (ret != MMSYSERR_NOERROR)
        {
            qDebug() << "AUDIO: can not prepeare buffer " << i;
        }
        ret = waveInAddBuffer(m_hWavIn, &m_stWHDR[i], sizeof(WAVEHDR));
        if (ret != MMSYSERR_NOERROR)
        {
            qDebug() << "AUDIO: can not add buffer " << i;
        }
    }
}

void AudioGraber::unPrepareBuffers()
{
    MMRESULT ret = 0;
    if (m_hWavIn)
    {
        ret = waveInStop(m_hWavIn);
        for(int i = 0; i < MAX_BUFFERS; ++i)
        {
            if (m_stWHDR[i].lpData)
            {
                ret = waveInUnprepareHeader(m_hWavIn, &m_stWHDR[i], sizeof(WAVEHDR));
                HeapFree(GetProcessHeap(), 0, m_stWHDR[i].lpData);
                ZeroMemory(&m_stWHDR[i], sizeof(WAVEHDR));
            }
        }
    }
}

