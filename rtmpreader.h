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

#ifndef RTMPREADER_H
#define RTMPREADER_H

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QThread>
#include <QMutex>
#include <QAudioOutput>
#include <QIODevice>
#include <QAudioOutput>

#include <audioplayer.h>


class AudioBuffer;

class RTMPReaderPrivate: public QObject
{
    Q_OBJECT

public:
    explicit RTMPReaderPrivate(QObject* parent = 0);

public Q_SLOTS:
    Q_INVOKABLE void start(QString url);

    /*!
      Sets the stop flag.
      \sa stopRequest()
    */
    void setStopRequest(bool stop);

    /*!
      Returns the stop flag. If the flag is true then grabing device is going to be stopped (if it is active)
      \sa setStopRequest()
    */
    bool isStopRequest() const;

    void mute();
    void unmute();

signals:
    void frameAvailable(QImage image);

private:
    bool _isStopped;
    QMutex _stopPauseMutex;
    AudioPlayer player;
};

class RTMPReader : public QQuickPaintedItem
{
    Q_OBJECT
    Q_DISABLE_COPY(RTMPReader)
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)

public:
    explicit RTMPReader(QQuickItem *parent = 0);
    virtual ~RTMPReader();

    void paint(QPainter *painter);

    QString url();
    void setUrl(const QString& newUrl);

public slots:
    Q_INVOKABLE void start(QString url = QString());
    Q_INVOKABLE void stop();

    void mute();
    void unmute();

signals:
    void urlChanged();

private slots:
    void frameAvailable(QImage image);

private:
    QImage _frame;
    QString _url;
    QThread* _thread;
    RTMPReaderPrivate* d_ptr;
};

#endif // RTMPREADER_H
