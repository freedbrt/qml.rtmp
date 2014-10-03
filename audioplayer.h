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

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "abstractgrabber.h"
#include <audioformat.h>

#include <QHash>
#include <QQueue>
#include <QByteArray>
#include <QList>
#include <QVector>
#include <QMutex>

class QAudioInput;
class RtAudio;

typedef unsigned int RtAudioStreamStatus;

//! The AudioGrabber class allows the application to capture data from audio input devices.
/*!
  Using this class you can capture audio data from devices available on your system. In order to get available devices
  call the availableDeviceNames() static function. To let AudioGrabber know which input device it must grab pass the device index to the setDeviceIndex() function.
  Also you can adjust the quality of audio by passing AudioFormat object to the setFormat() function. During the process of capture you can call the grabbedAudioDataSize()
  function that returns the size of grabbed audio data.

  The signal dataAvailable() is emmited whenever a new data chunk is available.

  Here is an example of AudioGrabber usage:
  @code
    AudioFormat format;
    format.setChannelCount(2);
    format.setSampleRate(44100);
    format.setFormat(AudioFormat::SignedInt16);

    AudioGrabber *audioGrabber = new AudioGrabber(this);
    audioGrabber->setDeviceIndex(0);
    audioGrabber->setFormat(format);
    audioGrabber->start();
  @endcode
*/
class AudioPlayer : public AbstractGrabber
{
    Q_OBJECT

    friend int handleData(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                          double streamTime, RtAudioStreamStatus status, void *data);

public:
    explicit AudioPlayer(QObject *parent = 0);
    virtual ~AudioPlayer();

    /*!
      Lets AudioPlayer know which device it must playback.
      \sa deviceIndex()
      \sa availableDeviceNames()
    */
    void setDeviceIndex(int index);
    /*!
      Returns current device index.
      \sa setDeviceIndex()
    */
    int deviceIndex() const;

    /*!
      Sets the given format.
      \sa format()
    */
    void setFormat(const AudioFormat &format);
    /*!
      Returns current audio format.
      \sa setFormat()
    */
    AudioFormat format() const;

    /*!
      Returns the size of grabbed audio data.
      When the grabber is not active returns 0.
      \sa elapsedMilliseconds()
    */
    int grabbedAudioDataSize() const;
    /*!
      Returns time in milliseconds from start of grabbing process till this moment.
      When the grabber is not active returns 0.
      \sa grabbedAudioDataSize()
    */
    int elapsedMilliseconds() const;

    /*!
      Returns available audio input devices.
      Return value consists of <b>deviceId</b> and <b>deviceName</b> parameters.
    */
    static QHash<int /*id*/, QString /*name*/> availableDevices();

    void writeData(double pts, const QByteArray& newArray);
    QByteArray readData(double pts, int num);
    QByteArray readAll();

    int defaultAudioDeviceIndex() const;

    void setStreamTime(double time);
    double getStreamTime();

public Q_SLOTS:
    bool start();
    void stop();
    void suspend();
    void resume();

private:
    void init();
    void cleanup();

    AudioFormat m_format;
    int m_deviceIndex;
    int m_grabbedAudioDataSize;
    RtAudio *m_rtAudio;
    QMutex _mutex;
    QByteArray _audioData;

    QList<QPair<double, QByteArray> > _audioDataList;
};

#endif // AUDIOPLAYER_H
