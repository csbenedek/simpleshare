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

#include "videoencoder.h"
#include <QPainter>
#include <libavcodec/avcodec.h>
#include <QDebug>
#include "ffmpeg.h"


VideoEncoder::VideoEncoder()
{
   initVars();
   initCodec();
}

VideoEncoder::~VideoEncoder()
{
   close();
   /*if(Initmodefile)
   {
      writeFooter();
      Outdev->close();
      delete Outdev;
      Outdev=0;
   }
   else
   {
      // nothing to do
   }*/
}

int VideoEncoder::frameTime() const
{
    if (m_codecCtx && m_codecCtx->time_base.den > 0)
    {
        return m_codecCtx->time_base.num * 1000 / m_codecCtx->time_base.den;
    }
    return 1000 / 25; // default 25;
}

int VideoEncoder::audioSamlesFrameSize() const
{
    Q_ASSERT(m_audioCodecCtx);
    if (m_audioCodecCtx)
    {
        return m_audioCodecCtx->frame_size * 2 * m_audioCodecCtx->channels;
    }
    else
    {
        return 0;
    }
}


bool VideoEncoder::createFile(const QString& fileName, unsigned width, unsigned height, unsigned bitrate, unsigned gop, unsigned fps, bool audio)
{
   // If we had an open video, close it.
   close();

   m_width      = width;
   m_height     = height;
   m_gop        = gop;
   m_bitrate    = bitrate;

   if(!isSizeValid())
   {
      qDebug("Invalid size\n");
      return false;
   }

   m_outputFormat = ffmpeg::guess_format(NULL, fileName.toLocal8Bit(), NULL);
   if (!m_outputFormat) {
      qDebug("Could not deduce output format from file extension: using MPEG.\n");
      m_outputFormat = ffmpeg::guess_format("mpeg", NULL, NULL);
   }

   m_formatCtx=ffmpeg::avformat_alloc_context();
   if(!m_formatCtx)
   {
      qDebug("Error allocating format context\n");
      return false;
   }
   m_formatCtx->oformat = m_outputFormat;
   strncpy(m_formatCtx->filename, fileName.toLocal8Bit(), sizeof(m_formatCtx->filename));


   // Add the video stream

   m_videoStream = av_new_stream(m_formatCtx,0);
   if(!m_videoStream )
   {
      qDebug("Could not allocate video stream\n");
      return false;
   }


   m_codecCtx=m_videoStream->codec;
   m_codecCtx->codec_id     = m_outputFormat->video_codec;
   m_codecCtx->codec_type   = ffmpeg::CODEC_TYPE_VIDEO;

   m_codecCtx->bit_rate     = m_bitrate;
   m_codecCtx->width        = getWidth();
   m_codecCtx->height       = getHeight();
   m_codecCtx->time_base.den = fps;
   m_codecCtx->time_base.num = 1;
   m_codecCtx->gop_size     = m_gop;
   m_codecCtx->pix_fmt      = ffmpeg::PIX_FMT_YUV420P;


   //avcodec_thread_init deprecated
   //avcodec_thread_init(m_codecCtx, 10);

   //if (c->codec_id == CODEC_ID_MPEG2VIDEO)
   //{
      //c->max_b_frames = 2;  // just for testing, we also add B frames
   //}

   // some formats want stream headers to be separate
   if(m_formatCtx->oformat->flags & AVFMT_GLOBALHEADER)
      m_codecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;


   if (av_set_parameters(m_formatCtx, NULL) < 0)
   {
      qDebug("Invalid output format parameters\n");
      return false;
   }

   ffmpeg::dump_format(m_formatCtx, 0, m_formatCtx->filename, 1);

   // open_video
   // find the video encoder
   m_codec = avcodec_find_encoder(m_codecCtx->codec_id);
   if (!m_codec)
   {
      qDebug("codec not found\n");
      return false;
   }
   // open the codec
   if (avcodec_open(m_codecCtx, m_codec) < 0)
   {
      qDebug("could not open codec\n");
      return false;
   }

   // Allocate memory for output
   if(!initOutputBuf())
   {
      qDebug("Can't allocate memory for output bitstream\n");
      return false;
   }

   // Allocate the YUV frame
   if(!initFrame())
   {
      qDebug("Can't init frame\n");
      return false;
   }

   // Add the audio stream
   m_audioStream = NULL;
   m_audioCodecCtx = NULL;
   m_audioCodec = NULL;

   if (audio)
   {
        m_audioStream = av_new_stream(m_formatCtx,0);
        if(!m_audioStream)
        {
            qDebug("Could not allocate audio stream\n");
            return false;
        }

       m_audioCodecCtx = m_audioStream->codec;
       m_audioCodecCtx->codec_id = m_outputFormat->audio_codec;
       m_audioCodecCtx->codec_type = ffmpeg::CODEC_TYPE_AUDIO;

       // some formats want stream headers to be separate
       if(m_formatCtx->oformat->flags & AVFMT_GLOBALHEADER)
          m_audioCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

       // open_video
       // find the video encoder
       m_audioCodec = avcodec_find_encoder(m_audioCodecCtx->codec_id);

       if (!m_audioCodec)
       {
           qDebug("audio codec not found. trying AC3\n");

           /* find the ac3 encoder */
           m_audioCodec = avcodec_find_encoder(ffmpeg::CODEC_ID_AC3);
       }

       if (!m_audioCodec)
       {
          qDebug("audio codec not found\n");
          //return false;
       }

       m_audioCodecCtx->sample_rate = 11025;
       m_audioCodecCtx->bit_rate = 8000;
       m_audioCodecCtx->channels = 1;

       // open the codec
       if (avcodec_open(m_audioCodecCtx, m_audioCodec) < 0)
       {
          qDebug("could not open audio codec\n");
          m_audioCodec = NULL;
          m_audioStream = NULL;
          m_audioCodecCtx = NULL;
          //return false;
       }
   }

   if (url_fopen(&m_formatCtx->pb, m_formatCtx->filename, URL_WRONLY) < 0)
   {
      qDebug() << "Could not open " << fileName;
      return false;
   }

   av_write_header(m_formatCtx);

   m_ok = true;

   return true;
}

/**
   \brief Completes writing the stream, closes it, release resources.
**/
bool VideoEncoder::close()
{
   if(!isOk())
      return false;

   av_write_trailer(m_formatCtx);

   // close_video

   avcodec_close(m_videoStream->codec);
   if (m_audioCodec && m_audioStream)
   {
        avcodec_close(m_audioStream->codec);
   }

   freeFrame();
   freeOutputBuf();


   /* free the streams */

   for(unsigned int i = 0; i < m_formatCtx->nb_streams; i++)
   {
      av_freep(&m_formatCtx->streams[i]->codec);
      av_freep(&m_formatCtx->streams[i]);
   }

   // Close file
   url_fclose(m_formatCtx->pb);

   // Free the stream
   av_free(m_formatCtx);

   initVars();
   return true;
}

/**
   \brief Encode one frame

   The frame must be of the same size as specifie
**/
int VideoEncoder::encodeImage(const QImage &img)
{
   if(!isOk())
      return -1;

   //convertImage(img);       // Custom conversion routine
   convertImage_sws(img);     // SWS conversion

   int outSize = ffmpeg::avcodec_encode_video(m_codecCtx, m_outbuf, m_outbufSize, m_picture);
   //qDebug("Frame size: %d\n",out_size);
   if (outSize > 0)
   {

      av_init_packet(&m_pkt);

      //if (m_codecCtx->coded_frame->pts != AV_NOPTS_VALUE)
      if (m_codecCtx->coded_frame->pts != static_cast<signed long long>(0x8000000000000000LL))
      {
         m_pkt.pts  = av_rescale_q(m_codecCtx->coded_frame->pts, m_codecCtx->time_base, m_videoStream->time_base);
      }
      if(m_codecCtx->coded_frame->key_frame)
      {
         m_pkt.flags |= PKT_FLAG_KEY;
      }

      m_pkt.stream_index= m_videoStream->index;
      m_pkt.data= m_outbuf;
      m_pkt.size= outSize;
      int ret = av_interleaved_write_frame(m_formatCtx, &m_pkt);
      //qDebug("Wrote %d\n",ret);
      if(ret<0)
      {
         return -1;
      }
   }
   return outSize;
}


/******************************************************************************
* INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL   INTERNAL
******************************************************************************/

void VideoEncoder::initVars()
{
   m_ok=false;
   m_formatCtx=0;
   m_outputFormat=0;
   m_codecCtx=0;
   m_videoStream=0;
   m_codec=0;
   m_picture=0;
   m_outbuf=0;
   m_pictureBuf=0;
   m_imgConvertCtx=0;
}


/**
   \brief Register the codecs
**/
bool VideoEncoder::initCodec()
{
   ffmpeg::avcodec_init();
   ffmpeg::av_register_all();

   //qDebug("License: %s\n",ffmpeg::avformat_license());
   //qDebug("AVCodec version %d\n",ffmpeg::avformat_version());
   //qDebug("AVFormat configuration: %s\n",ffmpeg::avformat_configuration());

   return true;
}


/**
  Ensures sizes are some reasonable multiples
**/
bool VideoEncoder::isSizeValid()
{
   if(getWidth()%8)
      return false;
   if(getHeight()%8)
      return false;
   return true;
}

unsigned VideoEncoder::getWidth()
{
   return m_width;
}
unsigned VideoEncoder::getHeight()
{
   return m_height;
}
bool VideoEncoder::isOk()
{
   return m_ok;
}

/**
  Allocate memory for the compressed bitstream
**/
bool VideoEncoder::initOutputBuf()
{
   m_outbufSize = qMax(getWidth()*getHeight()*3, 10U*1024U);        // Some extremely generous memory allocation for the encoded frame.
   m_outbuf = new uint8_t[m_outbufSize];
   if (m_outbuf==0)
   {
      return false;
   }
   return true;
}
/**
  Free memory for the compressed bitstream
**/
void VideoEncoder::freeOutputBuf()
{
   if(m_outbuf)
   {
      delete[] m_outbuf;
      m_outbuf=0;
   }
}

bool VideoEncoder::initFrame()
{
   m_picture = ffmpeg::avcodec_alloc_frame();
   if(m_picture==0)
   {
      return false;
   }

   int size = avpicture_get_size(m_codecCtx->pix_fmt, m_codecCtx->width, m_codecCtx->height);
   m_pictureBuf = new uint8_t[size];
   if (m_pictureBuf==0)
   {
      av_free(m_picture);
      m_picture=0;
      return false;
   }

   // Setup the planes
   avpicture_fill((ffmpeg::AVPicture *)m_picture, m_pictureBuf, m_codecCtx->pix_fmt, m_codecCtx->width, m_codecCtx->height);

   return true;
}
void VideoEncoder::freeFrame()
{
   if(m_pictureBuf)
   {
      delete[] m_pictureBuf;
      m_pictureBuf=0;
   }
   if(m_picture)
   {
      av_free(m_picture);
      m_picture=0;
   }
}

/**
  \brief Convert the QImage to the internal YUV format

  Custom conversion - not very optimized.

**/

bool VideoEncoder::convertImage(const QImage &img)
{
   // Check if the image matches the size
   Q_ASSERT(img.width() >= 0);
   Q_ASSERT(img.height() >= 0);
   unsigned w = img.width();
   unsigned h = img.height();
   if(w != getWidth() || h != getHeight())
   {
      qDebug() << "Wrong image size!";
      return false;
   }
   if(img.format() != QImage::Format_RGB32 && img.format() != QImage::Format_ARGB32)
   {
      qDebug() << "Wrong image format";
      return false;
   }

   // RGB32 to YUV420

   int size = getWidth()*getHeight();
   // Y
   for(unsigned y=0;y<getHeight();y++)
   {

      unsigned char *s = (unsigned char*)img.scanLine(y);
      unsigned char *d = (unsigned char*)&m_pictureBuf[y*getWidth()];
      //qDebug("Line %d. d: %p. m_pictureBuf: %p\n",y,d,m_pictureBuf);

      for(unsigned x=0;x<getWidth();x++)
      {
         unsigned int r=s[2];
         unsigned int g=s[1];
         unsigned int b=s[0];

         unsigned Y = (r*2104 + g*4130 + b*802 + 4096 + 131072) >> 13;
         if(Y>235) Y=235;

         *d = Y;

         d+=1;
         s+=4;
      }
   }

   // U,V
   for(unsigned y=0;y<getHeight();y+=2)
   {
      unsigned char *s = (unsigned char*)img.scanLine(y);
      unsigned int ss = img.bytesPerLine();
      unsigned char *d = (unsigned char*)&m_pictureBuf[size+y/2*getWidth()/2];

      //qDebug("Line %d. d: %p. m_pictureBuf: %p\n",y,d,m_pictureBuf);

      for(unsigned x=0;x<getWidth();x+=2)
      {
         // Cr = 128 + 1/256 * ( 112.439 * R'd -  94.154 * G'd -  18.285 * B'd)
         // Cb = 128 + 1/256 * (- 37.945 * R'd -  74.494 * G'd + 112.439 * B'd)

         // Get the average RGB in a 2x2 block
         int r=(s[2] + s[6] + s[ss+2] + s[ss+6] + 2) >> 2;
         int g=(s[1] + s[5] + s[ss+1] + s[ss+5] + 2) >> 2;
         int b=(s[0] + s[4] + s[ss+0] + s[ss+4] + 2) >> 2;

         int Cb = (-1214*r - 2384*g + 3598*b + 4096 + 1048576)>>13;
         if(Cb<16)
            Cb=16;
         if(Cb>240)
            Cb=240;

         int Cr = (3598*r - 3013*g - 585*b + 4096 + 1048576)>>13;
         if(Cr<16)
            Cr=16;
         if(Cr>240)
            Cr=240;

         *d = Cb;
         *(d+size/4) = Cr;

         d+=1;
         s+=8;
      }
   }
   return true;
}

/**
  \brief Convert the QImage to the internal YUV format

  SWS conversion

   Caution: the QImage is allocated by QT without guarantee about the alignment and bytes per lines.
   It *should* be okay as we make sure the image is a multiple of many bytes (8 or 16)...
   ... however it is not guaranteed that sws_scale won't at some point require more bytes per line.
   We keep the custom conversion for that case.

**/

bool VideoEncoder::convertImage_sws(const QImage &img)
{
   // Check if the image matches the size
   Q_ASSERT(img.width() >= 0);
   Q_ASSERT(img.height() >= 0);
   unsigned w = img.width();
   unsigned h = img.height();
   if(w != getWidth() || h != getHeight())
   {
      qDebug() << "Wrong image size!";
      return false;
   }
   if(img.format() != QImage::Format_RGB32	&& img.format() != QImage::Format_ARGB32)
   {
      qDebug() << "Wrong image format";
      return false;
   }

   m_imgConvertCtx = ffmpeg::sws_getCachedContext(m_imgConvertCtx,getWidth(),getHeight(),ffmpeg::PIX_FMT_BGRA,getWidth(),getHeight(),ffmpeg::PIX_FMT_YUV420P,SWS_BICUBIC, NULL, NULL, NULL);
   //m_imgConvertCtx = ffmpeg::sws_getCachedContext(m_imgConvertCtx,getWidth(),getHeight(),ffmpeg::PIX_FMT_BGRA,getWidth(),getHeight(),ffmpeg::PIX_FMT_YUV420P,SWS_FAST_BILINEAR, NULL, NULL, NULL);
   if (m_imgConvertCtx == NULL)
   {
      qDebug() << "Cannot initialize the conversion context";
      return false;
   }

   uint8_t *srcplanes[3];
   srcplanes[0]=(uint8_t*)img.bits();
   srcplanes[1]=0;
   srcplanes[2]=0;

   int srcstride[3];
   srcstride[0]=img.bytesPerLine();
   srcstride[1]=0;
   srcstride[2]=0;


   ffmpeg::sws_scale(m_imgConvertCtx, srcplanes, srcstride,0, getHeight(), m_picture->data, m_picture->linesize);

   return true;
}

int VideoEncoder::encodeAudioFrame(int size, short int* samples)
{
    if(!isOk())
       return 0;

    if (m_audioCodecCtx == NULL)
    {
        return 0;
    }

    int outSize = ffmpeg::avcodec_encode_audio(m_audioCodecCtx, m_outbuf, m_outbufSize, samples);
    //qDebug("Audio Frame size: %d samples = %d\n", out_size, size);
    if (outSize > 0)
    {
        av_init_packet(&m_pkt);

        if (m_audioCodecCtx->coded_frame->pts != static_cast<signed long long>(0x8000000000000000LL))
        {
            m_pkt.pts = av_rescale_q(m_audioCodecCtx->coded_frame->pts, m_audioCodecCtx->time_base, m_audioStream->time_base);
        }
        if(m_audioCodecCtx->coded_frame->key_frame)
        {
            m_pkt.flags |= PKT_FLAG_KEY;
        }

        m_pkt.stream_index= m_audioStream->index;
        m_pkt.data= m_outbuf;
        m_pkt.size= outSize;
        int ret = av_interleaved_write_frame(m_formatCtx, &m_pkt);
        if (ret < 0)
        {
            qDebug() << "av_interleaved_write_frame(m_formatCtx, &pkt); failed";
            return 0;
        }
    }
    return outSize;
}
