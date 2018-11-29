TEMPLATE = lib
TARGET = qml.rtmp
QT += qml core gui quick multimedia multimediawidgets
CONFIG += qt plugin

DESTDIR = lib

TARGET = $$qtLibraryTarget($$TARGET)
uri = qml.rtmp


INCLUDEPATH += $$(FFMPEG_INCLUDE_PATH) $$(OPENCV_PATH)\include
LIBS += -L$$(FFMPEG_LIBRARY_PATH) -L$$(OPENCV_PATH)\lib

CONFIG(debug, debug|release) {
    LIBS += -llibopencv_highgui246d -llibopencv_core246d -llibopencv_imgproc246d
}

CONFIG(release, debug|release) {
    LIBS += -llibopencv_highgui246 -llibopencv_core246 -llibopencv_imgproc246
}

win32 {
    DEFINES += __WINDOWS_DS__
    LIBS += -ldsound -lole32 -lwinmm

    LIBS += -lavcodec \
    -lavdevice \
    -lavformat \
    -lswscale \
    -lavutil \
    -ldsound \
    -lole32 \
    -lwinmm
}

# Input
SOURCES += \
    streamer_plugin.cpp \
    rtmpsender.cpp \
    abstractgrabber.cpp \
    abstractimagegrabber.cpp \
    audiograbber.cpp \
    cameragrabber.cpp \
    streamer.cpp \
    3rdparty/RtAudio/include/asio.cpp \
    3rdparty/RtAudio/include/asiodrivers.cpp \
    3rdparty/RtAudio/include/asiolist.cpp \
    3rdparty/RtAudio/include/iasiothiscallresolver.cpp \
    3rdparty/RtAudio/RtAudio.cpp \
    encoder/audiocodecsettings.cpp \
    encoder/encoder.cpp \
    encoder/videocodecsettings.cpp \
    helpers/audiotimer.cpp \
    rtmpreader.cpp \
    audioplayer.cpp \
    audioformat.cpp \
    qtcameragrabber.cpp

HEADERS += \
    streamer_plugin.h \
    rtmpsender.h \
    abstractgrabber.h \
    abstractimagegrabber.h \
    audiograbber.h \
    cameragrabber.h \
    streamer.h \
    3rdparty/RtAudio/include/asio.h \
    3rdparty/RtAudio/include/asiodrivers.h \
    3rdparty/RtAudio/include/asiodrvr.h \
    3rdparty/RtAudio/include/asiolist.h \
    3rdparty/RtAudio/include/asiosys.h \
    3rdparty/RtAudio/include/dsound.h \
    3rdparty/RtAudio/include/ginclude.h \
    3rdparty/RtAudio/include/iasiodrv.h \
    3rdparty/RtAudio/include/iasiothiscallresolver.h \
    3rdparty/RtAudio/include/soundcard.h \
    3rdparty/RtAudio/RtAudio.h \
    encoder/audiocodecsettings.h \
    encoder/encoder.h \
    encoder/encoderglobal.h \
    encoder/videocodecsettings.h \
    helpers/audiotimer.h \
    rtmpreader.h \
    audioplayer.h \
    audioformat.h \
    qtcameragrabber.h


OTHER_FILES = qmldir

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

