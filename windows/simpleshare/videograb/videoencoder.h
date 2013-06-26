/*
   QTFFmpegWrapper - QT FFmpeg Wrapper Class
   Copyright (C) 2009,2010:
         Daniel Roggen, droggen@gmail.com

   All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include <QIODevice>
#include <QFile>
#include <QImage>
//#ifdef Q_OS_UNIX
#include <stdint.h> // for uint8_t
//#endif

#include "ffmpeg.h"

class VideoEncoder
{
public:
      VideoEncoder();
      virtual ~VideoEncoder();

      bool createFile(const QString& filename, unsigned width, unsigned height, unsigned bitrate, unsigned gop, unsigned fps, bool audio);
      virtual bool close();

      virtual int encodeImage(const QImage &);
      virtual int encodeAudioFrame(int size, short int* samples);
      virtual bool isOk();

      int frameTime() const; // milliseconds
      int audioSamlesFrameSize() const;

protected:
      unsigned getWidth();
      unsigned getHeight();
      bool isSizeValid();

      void initVars();
      bool initCodec();

      // Alloc/free the output buffer
      bool initOutputBuf();
      void freeOutputBuf();

      // Alloc/free a frame
      bool initFrame();
      void freeFrame();

      // Frame conversion
      bool convertImage(const QImage &img);
      bool convertImage_sws(const QImage &img);

private:
      unsigned  m_width;
      unsigned  m_height;
      unsigned  m_bitrate;
      unsigned  m_gop; // maximal interval in frames between keyframes
      bool      m_ok;

      // FFmpeg stuff
      ffmpeg::AVFormatContext*  m_formatCtx;
      ffmpeg::AVOutputFormat*   m_outputFormat;
      ffmpeg::AVCodecContext*   m_codecCtx;
      ffmpeg::AVCodecContext*   m_audioCodecCtx;
      ffmpeg::AVStream*         m_videoStream;
      ffmpeg::AVStream*         m_audioStream;
      ffmpeg::AVCodec*          m_codec;
      ffmpeg::AVCodec*          m_audioCodec;
      // Frame data
      ffmpeg::AVFrame*          m_picture;
      uint8_t*                  m_pictureBuf;
      // Compressed data
      int                       m_outbufSize;
      uint8_t*                  m_outbuf;
      // Conversion
      ffmpeg::SwsContext*       m_imgConvertCtx;
      // Packet
      ffmpeg::AVPacket          m_pkt;

};

#endif // VIDEOENCODER_H
