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

#include "rtmpsender.h"

#include <QSGGeometry>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>


#include <opencv2/opencv.hpp>

#include "encoder/encoder.h"
#include "cameragrabber.h"
#include "cameragrabber.h"
#include "audiograbber.h"

#include <QImage>
#include <QDebug>

using namespace cv;

AudioCodecSettings audioCodecSettings(AudioGrabber* audioGrabber)
{
    AudioCodecSettings settings;
    settings.setSampleRate(44100);
    settings.setChannelCount(1);
    settings.setSampleFormat(EncoderGlobal::SAMPLE_FMT_S16P);
    settings.setBitrate(128000);

    return settings;
}

VideoCodecSettings videoCodecSettingsLosslessStreaming()
{
    //x264 loseless fast preset
    VideoCodecSettings settings;
    settings.setBitrate(32000);
    settings.setCoderType(EncoderGlobal::Vlc);
    settings.setFlags(EncoderGlobal::GlobalHeader);
    settings.setMotionEstimationComparison(4);
//    settings.setPartitions(EncoderGlobal::I4x4);
//    settings.setSubpixelMotionEstimationQuality(5);
//    settings.setMotionEstimationRange(16);
//    settings.setGopSize(80);
//    settings.setMinimumKeyframeInterval(25);
//    settings.setSceneChangeThreshold(0);
//    settings.setIQuantFactor(0.71f);
//    settings.setBFrameStrategy(0);
//    settings.setQuantizerCurveCompressionFactor(1.6f);
//    settings.setMinimumQuantizer(1);
//    settings.setMaximumQuantizer(69);
//    settings.setMaximumQuantizerDifference(4);
    settings.setDirectMvPredictionMode(EncoderGlobal::SpatialMode);
    settings.setFlags2(EncoderGlobal::FastPSkip | EncoderGlobal::WPred | EncoderGlobal::BPyramid);
    settings.setConstantQuantizerMode(0);
    settings.setPFramePredictionAnalysisMethod(EncoderGlobal::NoWpm);

    return settings;
}

RTMPSender::RTMPSender(QQuickItem *parent):
    QQuickPaintedItem(parent)
{
     setFlag(ItemHasContents, true);

     camera = new CameraGrabber(this);
     camera->setDeviceIndex(0);
     camera->setLatency(50);

     AudioFormat format;
     format.setChannelCount(1);
     format.setSampleRate(44100);
     format.setFormat(AudioFormat::SignedInt16);

     audioGrabber = new AudioGrabber(this);
     audioGrabber->setDeviceIndex(audioGrabber->defaultAudioDeviceIndex());
     audioGrabber->setFormat(format);

     this->_streamer = new Streamer(this);

     Encoder *encoder = this->_streamer->encoder();

     encoder->setOutputPixelFormat(EncoderGlobal::YUV420P);
     encoder->setAudioCodec(EncoderGlobal::MP3);
     encoder->setVideoCodec(EncoderGlobal::FLV1);
     encoder->setVideoCodecSettings(videoCodecSettingsLosslessStreaming());
     encoder->setAudioCodecSettings(audioCodecSettings(audioGrabber));
     encoder->setEncodingMode(Encoder::VideoAudioMode);

     this->_streamer->setAudioGrabber(audioGrabber);
     this->_streamer->setImageGrabber(camera);

     QObject::connect(camera, &CameraGrabber::frameAvailable,
                      this, &RTMPSender::frameAvailable);
}

RTMPSender::~RTMPSender()
{
}

QMutex locker;

void RTMPSender::paint(QPainter *painter)
{
    QMutexLocker mutex(&locker);

    QRect paintRect = this->_frame.rect();
    QImage frame = this->_frame;

    if (this->_streamer->encoder()->state() == Encoder::StoppedState) {
        this->_frame = QImage();
        this->update();
        return;
    }

    if (!this->_frame.isNull()) {
        if (paintRect.width() != painter->window().width() ||
            paintRect.height() != painter->window().height()) {
            frame = frame.scaled(painter->window().width(), painter->window().height(),
                                 Qt::KeepAspectRatio);
        }
    }

    painter->beginNativePainting();
    painter->drawImage(QPoint(0,0), frame);
    painter->endNativePainting();
    this->update();
}

void RTMPSender::start(QString url)
{
    if (!url.isEmpty()) {
        this->setUrl(url);
    }

    if (this->url().isEmpty()) {
        qDebug() << "Warning no stream url is set";
        return;
    }
    this->_streamer->encoder()->setVideoSize(camera->maximumFrameSize(camera->deviceIndex()));
    this->_streamer->encoder()->setFilePath(this->url());
    this->_streamer->encoder()->setFixedFrameRate(this->frameRate());

    this->_streamer->start();
}

void RTMPSender::stop()
{
    this->_streamer->stop();
}

void RTMPSender::mute()
{
    this->_streamer->mute();
}

void RTMPSender::unmute()
{
    this->_streamer->unmute();
}

QString RTMPSender::url() const
{
    return this->_url;
}

void RTMPSender::setUrl(const QString &url)
{
    if (this->url() != url) {
        this->_url = url;
        emit this->urlChanged();
    }
}

int RTMPSender::frameRate() const
{
    return this->_frameRate;
}

void RTMPSender::setFrameRate(int fps)
{
    if (this->_frameRate != fps) {
        this->_frameRate = fps;
        emit this->frameRateChanged();
    }
}

void RTMPSender::setCameraIndex(int index)
{
    this->camera->setDeviceIndex(index);
}

int RTMPSender::cameraIndex() const
{
    return this->camera->deviceIndex();
}

QVariant RTMPSender::cameraDevices() const
{
    return QVariant::fromValue(this->camera->availableDeviceNames());
}

QString RTMPSender::cameraDeviceByIndex(int index)
{
    return this->camera->availableDeviceNames().at(index);
}

void RTMPSender::setAudioDeviceIndex(int index)
{
    if (audioGrabber->deviceIndex() != index) {
        audioGrabber->setDeviceIndex(index);
        emit this->audioDeviceIndexChanged();
    }
}

int RTMPSender::audioDeviceIndex() const
{
    return this->audioGrabber->deviceIndex();
}

QVariant RTMPSender::audioDevices() const
{
    return QVariant::fromValue(this->audioGrabber->deviceListIndex());
}

QString RTMPSender::audioDeviceByIndex(int index)
{
    return this->_streamer->audioGrabber()->deviceNameByIndex(index);
}

void RTMPSender::frameAvailable(const QImage &frame, int pts)
{
    this->_frame = frame;
}

