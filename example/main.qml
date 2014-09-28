import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.1
import qml.rtmp 1.0

Window {
    visible: true
    width: 1000
    height: 600

    RTMPSender {
        id: sender

        x: 50
        y: 150

        url: stremingUrl.text
        frameRate: 40

        width: 400
        height: 500
    }

    RTMPReader {
        id: reader

        url: readingUrl.text

        x: 500
        y: 150

        width: 400
        height: 500
    }

    Column {
        anchors {
            left: parent.left
            top: parent.top
            margins: 15
        }
        width: 600
        height: 300
        spacing: 5

        Row {
            width: 600
            height: 20
            spacing: 5

            Button {
                width: 100
                height: 20
                text: 'Start'
                onClicked: sender.start();
            }

            Button {
                width: 100
                height: 20
                text: 'Stop'
                onClicked: sender.stop();
            }

            Button {
                width: 100
                height: 20
                text: 'Mute'
                onClicked: sender.mute();
            }

            Button {
                width: 100
                height: 20
                text: 'Unmute'
                onClicked: sender.unmute();
            }
        }

        Row {
            width: 600
            height: 20
            spacing: 5

            ComboBox {
                id: audioInComboBox

                currentIndex: sender.audioDeviceIndex

                model: ListModel {
                    id: audioInModel
                }

                width: 200
                height: 20
                onCurrentIndexChanged: {
                    if (audioInModel.count > 0)
                        sender.audioDeviceIndex = audioInModel.get(currentIndex).index;
                }
            }


            ComboBox {
                id: cameraInComboBox

                currentIndex: sender.cameraDeviceIndex

                model: ListModel {
                    id: cameraInModel
                }

                width: 200
                height: 20
                onCurrentIndexChanged: {
                    if (cameraInModel.count > 0)
                        sender.cameraIndex = cameraInModel.get(currentIndex).index;
                }
            }
        }

        TextField {
            id: stremingUrl

            width: 400
            height: 20
            text: 'rtmp://192.168.11.101/live/live'
        }
    }

    Component.onCompleted: {
        Object.keys(sender.audioDevices).forEach(function(e){
            audioInModel.append({index: e, text: sender.audioDeviceByIndex(e)});
        });

        Object.keys(sender.cameraDevices).forEach(function(e){
            cameraInModel.append({index: e, text: sender.cameraDeviceByIndex(e)});
        });
    }

    Column {
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: 500
            topMargin: 15
        }
        width: 600
        height: 300
        spacing: 5

        Row {
            width: 600
            height: 20
            spacing: 5

            Button {
                width: 100
                height: 20
                text: 'Start'
                onClicked: reader.start();
            }

            Button {
                width: 100
                height: 20
                text: 'Stop'
                onClicked: reader.stop();
            }

            Button {
                width: 100
                height: 20
                text: 'Mute'
                onClicked: reader.mute();
            }

            Button {
                width: 100
                height: 20
                text: 'Unmute'
                onClicked: reader.unmute();
            }
        }

        TextField {
            id: readingUrl

            width: 400
            height: 20
            text: 'rtmp://192.168.11.101/live/live'
        }
    }
}
