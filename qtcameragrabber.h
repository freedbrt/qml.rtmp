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

#ifndef QTCAMERAGRABBER_H
#define QTCAMERAGRABBER_H

#include "abstractimagegrabber.h"

#include <QStringList>
#include <QCamera>
#include <QCameraImageCapture>
#include <QVideoFrame>

//! The CameraGrabber class allows the application to capture frames from a camera device.
/*!
  Using this class you can capture frames with variable frame rate from a camera device. In order to get available devices
  call the availableDeviceNames() static function. To let CameraGrabber know which camera it must grab pass the device index to the setDeviceIndex() function.
  The CameraGrabber class has the setInitializationTime() function to set time the app will wait before grabbing start. It is need to be used because you
  will get black(or other colors) frames while a device is turning on. To get maximum frame size supported by a device use the the maximumFrameSize() static function.
  If you want to get a frame with the size that smaller or larger than maximumFrameSize() then you can set the needed size calling the setSize() function.

  The signal frameAvailable() is emmited whenever a new frame is available.
*/

#include <QAbstractVideoSurface>
#include <QVideoFrame>

//#include <opencv/cv.h>
//#include <opencv/highgui.h>

class MySurface: public QAbstractVideoSurface
{
    Q_OBJECT
public:
    MySurface(QObject* parent = 0) : QAbstractVideoSurface(parent) {

    }

    bool isFormatSupported ( const QVideoSurfaceFormat & format ) const {
        return true;
    }

    bool present(const QVideoFrame& frame){
        if (frame.isValid()) {
            QVideoFrame cloneFrame(frame);
            cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
            const QImage img(cloneFrame.bits(),
                             cloneFrame.width(),
                             cloneFrame.height(),
                             QVideoFrame::imageFormatFromPixelFormat(cloneFrame .pixelFormat()));

            // do something with the image ...

            cloneFrame.unmap();
            return true;
        }
        return false;
    }

    QList<QVideoFrame::PixelFormat>	supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const {
        QList<QVideoFrame::PixelFormat> list;

        list.append(QVideoFrame::Format_Invalid);
        list.append(QVideoFrame::Format_ARGB32);
        list.append(QVideoFrame::Format_ARGB32_Premultiplied);
        list.append(QVideoFrame::Format_RGB32);
        list.append(QVideoFrame::Format_RGB24);
        list.append(QVideoFrame::Format_RGB565);
        list.append(QVideoFrame::Format_RGB555);
        list.append(QVideoFrame::Format_ARGB8565_Premultiplied);
        list.append(QVideoFrame::Format_BGRA32);
        list.append(QVideoFrame::Format_BGRA32_Premultiplied);
        list.append(QVideoFrame::Format_BGR32);
        list.append(QVideoFrame::Format_BGR24);
        list.append(QVideoFrame::Format_BGR565);
        list.append(QVideoFrame::Format_BGR555);
        list.append(QVideoFrame::Format_BGRA5658_Premultiplied);

        list.append(QVideoFrame::Format_AYUV444);
        list.append(QVideoFrame::Format_AYUV444_Premultiplied);
        list.append(QVideoFrame::Format_YUV444);
        list.append(QVideoFrame::Format_YUV420P);
        list.append(QVideoFrame::Format_YV12);
        list.append(QVideoFrame::Format_UYVY);
        list.append(QVideoFrame::Format_YUYV);
        list.append(QVideoFrame::Format_NV12);
        list.append(QVideoFrame::Format_NV21);
        list.append(QVideoFrame::Format_IMC1);
        list.append(QVideoFrame::Format_IMC2);
        list.append(QVideoFrame::Format_IMC3);
        list.append(QVideoFrame::Format_IMC4);
        list.append(QVideoFrame::Format_Y8);
        list.append(QVideoFrame::Format_Y16);

        list.append(QVideoFrame::Format_Jpeg);

        list.append(QVideoFrame::Format_CameraRaw);
        list.append(QVideoFrame::Format_AdobeDng);

        return list;
    }

};

class QtCameraGrabber : public AbstractImageGrabber
{
    Q_OBJECT
public:
    QtCameraGrabber(QObject *parent = 0);
    virtual ~QtCameraGrabber();

    /*!
      Lets CameraGrabber know which device it must grab.
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
      Sets the frame size. A frame will be scaled as large as possible inside a given size, preserving the aspect ratio.
      \sa size()
    */
    void setSize(const QSize &size);
    /*!
      Returns the frame size.
      \sa setSize()
    */
    QSize size() const;

    /*!
      Returns names of available cameras.
    */
    static QStringList availableDeviceNames();
    /*!
      Returns the maximum frame size supported by a device.
      \sa availableDeviceNames()
    */
    static QSize maximumFrameSize(int deviceIndex);

public Q_SLOTS:
    bool start();

private Q_SLOTS:
    void onStateChanged(AbstractGrabber::State state);
    void imageAvailable(int id, const QVideoFrame & buffer);
    void imageCaptured(int id, const QImage & preview);
private:
    bool createCamera();
    void releaseCamera();
    QImage captureFrame();

    QCamera* _camera;
    QVideoWidget* _videoWidget;
    QCameraImageCapture* _imageCapture;
//    CvCapture *m_camera;
    int m_deviceIndex;
    QSize m_size;
};

#endif // CAMERAGRABBER_H
