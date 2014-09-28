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

#include "qtcameragrabber.h"

#include <QEventLoop>
#include <QTimer>
#include <QElapsedTimer>
#include <QDateTime>
#include <QPainter>

#include <QGraphicsVideoItem>
#include <QCamera>
#include <QCameraViewfinder>

QtCameraGrabber::QtCameraGrabber(QObject *parent)
    : AbstractImageGrabber(parent)
    , m_deviceIndex(-1)
{
    setInitializationTime(1000);

    connect(this, SIGNAL(stateChanged(AbstractGrabber::State)), this, SLOT(onStateChanged(AbstractGrabber::State)));
}

QtCameraGrabber::~QtCameraGrabber()
{
    if (this->state() == AbstractGrabber::ActiveState)
        this->releaseCamera();
}

void QtCameraGrabber::setDeviceIndex(int index)
{
    if (m_deviceIndex != index) {
        m_deviceIndex = index;
    }
}

int QtCameraGrabber::deviceIndex() const
{
    return m_deviceIndex;
}

void QtCameraGrabber::setSize(const QSize &size)
{
    if (m_size != size) {
        m_size = size;
    }
}

QSize QtCameraGrabber::size() const
{
    return m_size;
}

QStringList QtCameraGrabber::availableDeviceNames()
{
    QByteArray device;
    QList<QByteArray> devices = QCamera::availableDevices();
    QStringList names;

    Q_FOREACH (device, devices) {
        names.append(QCamera::deviceDescription(device));
    }

    return names;
}

QSize QtCameraGrabber::maximumFrameSize(int deviceIndex)
{
    if (deviceIndex < 0 || deviceIndex > QtCameraGrabber::availableDeviceNames().count())
        return QSize();

//    CvCapture *capture = cvCreateCameraCapture(deviceIndex);
    QSize size(640, 480);
//    size.setWidth(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH));
//    size.setHeight(cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT));
//    cvReleaseCapture(&capture);

    return size;
}

bool QtCameraGrabber::start()
{
    if (!createCamera())
        return false;

    return AbstractImageGrabber::start();
}

void QtCameraGrabber::onStateChanged(AbstractGrabber::State state)
{
    if (state == AbstractGrabber::StoppedState) {
//        releaseCamera();
    }
}

void QtCameraGrabber::imageAvailable(int id, const QVideoFrame &buffer)
{
    qDebug() << "imageAvailable" << buffer.size();
}

void QtCameraGrabber::imageCaptured(int id, const QImage &preview)
{
    qDebug() << "imageCaptured" << preview.size();
}

bool QtCameraGrabber::createCamera()
{
    if (m_deviceIndex < 0 || m_deviceIndex > QtCameraGrabber::availableDeviceNames().count()) {
        setError(AbstractGrabber::DeviceNotFoundError, tr("Device to be grabbed was not found."));
        return false;
    }

      _camera = new QCamera(QCamera::availableDevices().at(0), this);
      _camera->setCaptureMode(QCamera::CaptureStillImage);

////      connect(&captureTimer,  SIGNAL(timeout()),
////                       this,  SLOT (frameUpdated()));

      QGraphicsVideoItem* tmp = new QGraphicsVideoItem();
      MySurface * surface = new MySurface(this);
      _imageCapture = new QCameraImageCapture(_camera);
      _imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

      QObject::connect(_imageCapture, &QCameraImageCapture::imageAvailable,
                       this, &QtCameraGrabber::imageAvailable);

      QObject::connect(_imageCapture, &QCameraImageCapture::imageCaptured,
                       this, &QtCameraGrabber::imageCaptured);

      QCameraViewfinder* viewfinder = new QCameraViewfinder();
      viewfinder->show();

      _camera->setViewfinder(viewfinder);
      _camera->start();

      _imageCapture->capture();
      //imageAvailable

//          QImageEncoderSettings encoder_setting = m_imageCapture->encodingSettings();
//          encoder_setting.setCodec("image/jpeg");
//          encoder_setting.setQuality(QtMultimediaKit::NormalQuality);
//          encoder_setting.setResolution(800,600);
//          m_imageCapture->setEncodingSettings(encoder_setting);
//          connect(m_imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(processSavedImage(int,QString)));


      //_camera->setViewfinder(_videoWidget);

//    m_camera = cvCreateCameraCapture(m_deviceIndex);

//    //init size
//    if (!size().isValid()) {
//        QSize size;
//        size.setWidth(cvGetCaptureProperty(m_camera, CV_CAP_PROP_FRAME_WIDTH));
//        size.setHeight(cvGetCaptureProperty(m_camera, CV_CAP_PROP_FRAME_HEIGHT));
//        setSize(size);
//    }

    return true;
}

void QtCameraGrabber::releaseCamera()
{
//    cvReleaseCapture(&m_camera);
//    m_camera = 0;
}

QImage QtCameraGrabber::captureFrame()
{
//    if (_videoWidget)
//        return this->_videoWidget->grab().toImage();

    return QImage();
//    IplImage *iplImage = cvQueryFrame(m_camera);
//    if (!iplImage)
//        return QImage();

//    int height = iplImage->height;
//    int width = iplImage->width;

//    QImage frame;
//    if (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 3) {
//        cvCvtColor( iplImage, iplImage, CV_BGR2RGB);

//        uchar *data = (uchar*)iplImage->imageData;
//        frame = QImage(data, width, height, QImage::Format_RGB888); // .rgbSwapped(); // memory leak

//        //resize the frame to a given size
//        if (frame.size() != size()) {
//            QImage scaledFrame = frame.scaled(size(), Qt::KeepAspectRatio);

//            if (scaledFrame.size() != size()) {
//                QImage newFrame(size(), QImage::Format_RGB888);
//                newFrame.fill(Qt::black);
//                QPainter painter(&newFrame);

//                painter.drawImage(scaledFrame.width() < size().width() ? (newFrame.width() - scaledFrame.width()) / 2 : 0,
//                                  scaledFrame.height() < size().height() ? (newFrame.height() - scaledFrame.height()) / 2 : 0, scaledFrame);

//                scaledFrame = newFrame;
//            }

//            frame = scaledFrame;
//        }
//    }

    //return frame;
}
