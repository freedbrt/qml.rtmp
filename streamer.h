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

#ifndef RECORDER_H
#define RECORDER_H

#include <QObject>
#include <QImage>

class AbstractImageGrabber;
class CameraGrabber;
class AudioGrabber;
class Encoder;
class AudioTimer;

//! The Recorder class helps you easily record audio/video data.
/*!
  Using this class you can easily create audio and video files with data grabbed from the passed devices. Before start() you must setup encoder()
  and set an image grabber and/or an audio grabber calling setImageGrabber() and setAudioGrabber() correspondingly.

  The recorder takes care of audio/video streams syncronization(if both grabbers are set).
*/
class Streamer: public QObject
{
    Q_OBJECT
public:
    /*! This enum describes the state of the recorder. */
    enum State {
        ActiveState = 0,
        SuspendedState,
        StoppedState
    };

    explicit Streamer(QObject *parent = 0);
    virtual ~Streamer();
    
    void setImageGrabber(AbstractImageGrabber *grabber);
    AbstractImageGrabber* imageGrabber() const;

    void setAudioGrabber(AudioGrabber *grabber);
    AudioGrabber* audioGrabber() const;

    Encoder* encoder() const;

    Streamer::State state() const;

    /*! Returns a boolean value if audio is muted or no */
    bool isMuted() const;

public Q_SLOTS:
    /*! Starts audio/video grabbing. The state() is set to Recorder::ActiveState if no errors occurred. */
    void start();
     /*! Pauses audio/video grabbing. The state() is set to Recorder::SuspendedState. */
    void pause();
    /*! Resumes audio/video grabbing. The state() is set to Recorder::ActiveState. */
    void resume();
    /*! Stops audio/video grabbing. The state() is set to Recorder::StoppedState. */
    void stop();

    /*! Mutes audio volume (if audioGrabber() is set) */
    void mute();
    /*! Unmutes audio volume (if audioGrabber() is set) */
    void unmute();

Q_SIGNALS:
    /*! This signal is emitted when the recorder state has changed. */
    void stateChanged(Streamer::State state);
    /*! This signal is emitted when an error occurs. */
    void error(const QString &errorString);

private Q_SLOTS:
    void startGrabbers();

private:
    void setState(Streamer::State state);
    void setError(const QString &errorString);
    void onGrabbersError();

    bool connectAudioGrabber();
    bool disconnectAudioGrabber();

    void encodeSilence(int milliseconds);

    AbstractImageGrabber *m_imageGrabber;
    AudioGrabber *m_audioGrabber;
    Encoder *m_encoder;
    AudioTimer *m_timer;
    Streamer::State m_state;
    int m_startMuteTime;
    bool m_muted;
};

#endif // RECORDER_H
