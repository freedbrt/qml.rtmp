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

#include "audioplayer.h"
#include "3rdparty/RtAudio/RtAudio.h"

#include <QDebug>
#include <QMutexLocker>

int pos = 0;

int dataCallback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                 double streamTime, RtAudioStreamStatus status, void *data)
{
    Q_UNUSED(outputBuffer)
    Q_UNUSED(streamTime)
    Q_UNUSED(status)

    if (status) {
        qDebug() << "Stream audio overflow/underlow";
        return 0;
    }

    AudioPlayer *player = static_cast<AudioPlayer *>(data);

    QByteArray array = player->readData(nBufferFrames * 2);
    if (array.isEmpty()) {
        memset(outputBuffer, '\0', nBufferFrames * 2);
        return nBufferFrames * 2;
    }

    memcpy(outputBuffer, array.data(), array.size());
    return array.size();
}

AudioPlayer::AudioPlayer(QObject *parent)
    : AbstractGrabber(parent)
    , m_deviceIndex(-1)
{
    init();
}

AudioPlayer::~AudioPlayer()
{
    cleanup();
}


void AudioPlayer::setDeviceIndex(int index)
{
    if (m_deviceIndex != index) {
        m_deviceIndex = index;
    }
}

int AudioPlayer::deviceIndex() const
{
    return m_deviceIndex;
}

void AudioPlayer::setFormat(const AudioFormat &format)
{
    m_format = format;
}

AudioFormat AudioPlayer::format() const
{
    return m_format;
}

int AudioPlayer::grabbedAudioDataSize() const
{
    return m_grabbedAudioDataSize;
}

int AudioPlayer::elapsedMilliseconds() const
{ 
    if (!m_rtAudio || state() == AudioPlayer::StoppedState)
        return 0;

    return m_rtAudio->getStreamTime() * 1000;
}

QHash<int, QString> AudioPlayer::availableDevices()
{
    RtAudio rtAudio;
    int deviceCount = rtAudio.getDeviceCount();
    RtAudio::DeviceInfo info;
    QHash<int, QString> devices;

    for (int i = 0; i < deviceCount; ++i) {
        info = rtAudio.getDeviceInfo(i);

        if (info.outputChannels > 0) {
            devices.insert(i, QString::fromStdString(info.name));
        }
    }

    return devices;
}

void AudioPlayer::writeData(const QByteArray &newArray)
{
    if (state() != AbstractGrabber::ActiveState)
        return;

    QMutexLocker locker(&this->_mutex);
    qDebug() << "write" << newArray.size() << "size" << this->_audioData.size();

    this->_audioData += newArray;
}

QByteArray AudioPlayer::readData(int num)
{
//    qDebug() << this->_audioData.size();

    QMutexLocker locker(&this->_mutex);
    if (this->_audioData.isEmpty() || this->_audioData.size() < num || state() != AbstractGrabber::ActiveState) {
        return QByteArray();
    }

    QByteArray ret;
    ret.resize(num);
    memcpy(ret.data(), this->_audioData.data(), num);
    this->_audioData.remove(0, num);
    return ret;
}

QByteArray AudioPlayer::readAll()
{
    QMutexLocker locker(&this->_mutex);
    if (this->_audioData.isEmpty())
        return QByteArray();

    QByteArray ret;
    ret.resize(this->_audioData.size());
    memcpy(ret.data(), this->_audioData.data(), this->_audioData.size());
    this->_audioData.clear();
    return ret;
}

bool AudioPlayer::start()
{
    if (state() == AbstractGrabber::StoppedState) {
        if (deviceIndex() < 0 || deviceIndex() > availableDevices().count()) {
            setError(AbstractGrabber::DeviceNotFoundError, tr("Device to be players was not found."));
            return false;
        }

    #ifdef Q_OS_WIN32
        m_rtAudio = new RtAudio(RtAudio::WINDOWS_DS);
    #endif

    #ifdef Q_OS_LINUX
        m_rtAudio = new RtAudio(RtAudio::LINUX_ALSA);
    #endif

        RtAudio::StreamParameters params;
        RtAudio::StreamOptions options;
        options.flags = RTAUDIO_SCHEDULE_REALTIME | RTAUDIO_MINIMIZE_LATENCY;
        options.numberOfBuffers = 1;
        options.priority = 8;

        unsigned int bufferFrames = 1024;

        params.deviceId = deviceIndex();
        params.nChannels = format().channelCount();
        params.firstChannel = 0;

        try {
            m_rtAudio->openStream(&params, NULL, format().format(), format().sampleRate(), &bufferFrames, &dataCallback, this, &options);
            m_rtAudio->startStream();
        } catch (RtAudioError& e) {
            setError(AbstractGrabber::DeviceOpenError, "Unable to open device. Error " +
                     QString::fromStdString(e.getMessage()));
            return false;
        }

        setState(AbstractGrabber::ActiveState);

        return true;
    }

    return false;
}

void AudioPlayer::stop()
{
    if (state() != AbstractGrabber::StoppedState) {
        m_rtAudio->closeStream();
        this->_audioData.clear();
        cleanup();

        setState(AbstractGrabber::StoppedState);
    }
}

void AudioPlayer::suspend()
{
    if (state() == AbstractGrabber::ActiveState) {
        m_rtAudio->stopStream();

        setState(AbstractGrabber::SuspendedState);
    }
}

void AudioPlayer::resume()
{
    if (state() == AbstractGrabber::SuspendedState) {
        m_rtAudio->startStream();

        setState(AbstractGrabber::ActiveState);
    }
}

void AudioPlayer::init()
{
    m_grabbedAudioDataSize = 0;
    m_rtAudio = 0;
}

void AudioPlayer::cleanup()
{
    if (m_rtAudio)
        delete m_rtAudio;

    init();
}

int AudioPlayer::defaultAudioDeviceIndex() const
{
    RtAudio rtAudio;
    return rtAudio.getDefaultOutputDevice();
}

void AudioPlayer::setStreamTime(double time)
{
    m_rtAudio->setStreamTime(time);
}

double AudioPlayer::getStreamTime()
{
    return m_rtAudio->getStreamTime();
}
