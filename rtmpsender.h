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

#ifndef RTMPSENDER_H
#define RTMPSENDER_H

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QString>

#include "audiograbber.h"
#include "cameragrabber.h"
#include "streamer.h"

class QtCameraGrabber;

class RTMPSender : public QQuickPaintedItem
{
    Q_OBJECT
    Q_DISABLE_COPY(RTMPSender)
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(int frameRate READ frameRate WRITE setFrameRate NOTIFY frameRateChanged)

    Q_PROPERTY(int audioDeviceIndex READ audioDeviceIndex WRITE setAudioDeviceIndex NOTIFY audioDeviceIndexChanged)
    Q_PROPERTY(QVariant audioDevices READ audioDevices)

    Q_PROPERTY(int cameraIndex READ cameraIndex WRITE setCameraIndex NOTIFY cameraIndexChanged)
    Q_PROPERTY(QVariant cameraDevices READ cameraDevices)

public:
    explicit RTMPSender(QQuickItem *parent = 0);
    virtual ~RTMPSender();

    void paint(QPainter *painter);

public slots:
    Q_INVOKABLE void start(QString url = QString());
    Q_INVOKABLE void stop();
    void mute();
    void unmute();

    QString url() const;
    void setUrl(const QString& url);

    int frameRate() const;
    void setFrameRate(int fps);

    void setCameraIndex(int index);
    int cameraIndex() const;

    QVariant cameraDevices() const;
    QString cameraDeviceByIndex(int index);

    void setAudioDeviceIndex(int index);
    int audioDeviceIndex() const;

    QVariant audioDevices() const;
    QString audioDeviceByIndex(int index);

signals:
    void urlChanged();
    void frameRateChanged();
    void audioDeviceIndexChanged();
    void cameraIndexChanged();

private slots:
    void frameAvailable(const QImage &frame, int pts);

private:
    CameraGrabber* camera;
    AudioGrabber* audioGrabber;
    QImage _frame;
    Streamer* _streamer;

    QString _url;
    int _frameRate;
};

#endif // RTMPSENDER_H

