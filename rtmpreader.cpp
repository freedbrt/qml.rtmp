/****************************************************************************
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
****************************************************************************/

#include "rtmpreader.h"

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

//ffmpeg include files
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavdevice/avdevice.h>
}

#include <QAudioOutput>
#include <QBuffer>
#include <QMutexLocker>
#include <QAudioFormat>
#include <QElapsedTimer>

RTMPReaderPrivate::RTMPReaderPrivate(QObject* parent)
    : QObject(parent)
    , _isStopped(false)
    , _isRunning(false)
{
    AudioFormat rtFormat;
    rtFormat.setChannelCount(1);
    rtFormat.setSampleRate(44100);
    rtFormat.setFormat(AudioFormat::SignedInt16);

    player.setDeviceIndex(1);
    player.setFormat(rtFormat);

//    QAudioFormat format;
//    format.setChannelCount(1);
//    format.setSampleRate(44100);
//    format.setSampleSize(16);
//    format.setCodec("audio/pcm");
//    format.setByteOrder(QAudioFormat::LittleEndian);
//    format.setSampleType(QAudioFormat::SignedInt);

//    audioOutput = new QAudioOutput(format, this);
    //    audioOutput->setBufferSize(65536 * 8);
}

RTMPReaderPrivate::~RTMPReaderPrivate()
{
    this->setStopRequest(true);
}

void RTMPReaderPrivate::start(QString url) {
    if (this->_isRunning) {
        return;
    }

    this->_isRunning = true;
    this->setStopRequest(false);

    AVFormatContext* context = avformat_alloc_context();
    AVCodecContext  *pCodecCtx;
    int video_stream_index = -1;
    int audio_stream_index = -1;

    QString params = url + " live=1";
    if(avformat_open_input(&context, params.toUtf8().data(), NULL, NULL) != 0){
        this->_isRunning = false;
        return;
    }
    context->probesize2 = 200000;
    context->max_analyze_duration2 = 200000;

    if(avformat_find_stream_info(context, NULL) < 0){
        this->_isRunning = false;
        return;
    }

    av_dump_format(context, 0, params.toUtf8().data(), 0);

    for (int i = 0; i< context->nb_streams; i++){
        if (context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        }

        if (context->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
        }
    }

    AVCodec* pCodec = 0;
    AVCodec* audioCodec = 0;
    AVCodecContext* audioCodecContext = 0;

    AVFrame* pFrame    = av_frame_alloc();
    AVFrame* audioFrame = av_frame_alloc();
    AVFrame* pFrameRGB = av_frame_alloc();

    uint8_t *buffer;
    int numBytes;
    int res;
    int audioFrameFinished, frameFinished;
    AVPacket packet;

    AVPixelFormat pFormat = AV_PIX_FMT_RGB24;

    if (video_stream_index != -1) {
        pCodecCtx = context->streams[video_stream_index]->codec;
        pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

        if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
            this->_isRunning = false;
            return;
        }

        numBytes = avpicture_get_size(pFormat, pCodecCtx->width, pCodecCtx->height);
        buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    }

    if (audio_stream_index != -1) {
        audioCodecContext = context->streams[audio_stream_index]->codec;
        audioCodec = avcodec_find_decoder(audioCodecContext->codec_id);

        if(avcodec_open2(audioCodecContext, audioCodec, NULL) < 0) {
            this->_isRunning = false;
            return;
        }
    }

    this->unmute();

    int currentPts = 0;

    player.start();
    QElapsedTimer timer;
    timer.start();

    while (res = av_read_frame(context, &packet) >= 0)
    {
        if (this->isStopRequest())
            break;

        currentPts = packet.pts;
        player.setStreamTime((static_cast<double>(packet.pts) / 1000.0f));

        if (audio_stream_index != -1 && packet.stream_index == audio_stream_index){
            avcodec_decode_audio4(audioCodecContext, audioFrame, &audioFrameFinished, &packet);

            if(audioFrameFinished){
                if (packet.pts >= timer.elapsed() - 1000) {
                    QByteArray sample;
                    sample.resize(audioFrame->linesize[0]);
                    memcpy(sample.data(), (char*)audioFrame->extended_data[0], audioFrame->linesize[0]);
                    player.writeData(static_cast<double>(packet.pts) / 1000.0f, sample);
                }

                av_free_packet(&packet);
            }
        }

        else if (video_stream_index != -1 && packet.stream_index == video_stream_index){
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            if(frameFinished){
                avpicture_fill((AVPicture *)pFrameRGB, buffer, pFormat, pCodecCtx->width, pCodecCtx->height);

                struct SwsContext * img_convert_ctx;

                img_convert_ctx = sws_getCachedContext(NULL,
                                                       pCodecCtx->width,
                                                       pCodecCtx->height,
                                                       pCodecCtx->pix_fmt,
                                                       pCodecCtx->width,
                                                       pCodecCtx->height,
                                                       AV_PIX_FMT_RGB24,
                                                       SWS_BICUBIC, NULL, NULL,NULL);

                sws_scale(img_convert_ctx,
                          ((AVPicture*)pFrame)->data,
                          ((AVPicture*)pFrame)->linesize,
                          0,
                          pCodecCtx->height,
                          ((AVPicture *)pFrameRGB)->data,
                          ((AVPicture *)pFrameRGB)->linesize);

                QImage image = QImage(pFrameRGB->data[0], pFrame->width, pFrame->height, QImage::Format_RGB888);
                emit this->frameAvailable(image);

                av_free_packet(&packet);
                sws_freeContext(img_convert_ctx);
            }
        }
    }

    emit this->frameAvailable(QImage());

    player.stop();

   // av_free(buffer);
    av_free_packet(&packet);
    av_free(pFrame);
    av_free(pFrameRGB);
    av_free(audioFrame);
    avformat_close_input(&context);

    this->_isRunning = false;
    this->setStopRequest(false);
}

void RTMPReaderPrivate::setStopRequest(bool stop)
{
    QMutexLocker locker(&_stopPauseMutex);
    this->_isStopped = stop;
}

bool RTMPReaderPrivate::isStopRequest() const
{
    return this->_isStopped;
}

bool RTMPReaderPrivate::isRunning() const
{
    return this->_isRunning;
}

void RTMPReaderPrivate::mute()
{
    player.suspend();
}

void RTMPReaderPrivate::unmute()
{
    player.resume();
}

RTMPReader::RTMPReader(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , _thread(new QThread(this))
    , d_ptr(new RTMPReaderPrivate())
{
    av_register_all();
    avdevice_register_all();
    avcodec_register_all();
    avformat_network_init();

    this->d_ptr->moveToThread(this->_thread);
    connect(d_ptr, &RTMPReaderPrivate::frameAvailable,
            this, &RTMPReader::frameAvailable);

    this->_thread->start();
}

RTMPReader::~RTMPReader()
{
    this->_thread->terminate();
//    delete this->d_ptr;
//    this->d_ptr = 0;
}

void RTMPReader::paint(QPainter *painter)
{
    QRect paintRect = this->_frame.rect();
    QImage frame = this->_frame;

    if (!this->_frame.isNull()) {
        if (paintRect.width() != painter->window().width() ||
            paintRect.height() != painter->window().height()) {
            frame = frame.scaled(painter->window().width(), painter->window().height(),
                                 Qt::KeepAspectRatio);
        }
    }

    painter->drawImage(QPoint(0,0), frame);
    this->update();
}

QString RTMPReader::url()
{
    return this->_url;
}

void RTMPReader::setUrl(const QString &newUrl)
{
    if (this->_url != newUrl) {
        this->_url = newUrl;
        emit this->urlChanged();
    }
}

void RTMPReader::start(QString url)
{
    if (!url.isEmpty()) {
        this->setUrl(url);
    }

    if (this->url().isEmpty()) {
        qDebug() << "Warning, no url set.";
        return;
    }

    if (!d_ptr->isRunning())
        QMetaObject::invokeMethod(d_ptr, "start", Qt::QueuedConnection, Q_ARG(QString, this->url()));
}

void RTMPReader::stop()
{
    d_ptr->setStopRequest(true);
}

void RTMPReader::mute()
{
    this->d_ptr->mute();
}

void RTMPReader::unmute()
{
    this->d_ptr->unmute();
}

void RTMPReader::frameAvailable(QImage image)
{
    this->_frame = image;
}
