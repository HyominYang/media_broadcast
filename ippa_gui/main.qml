import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    signal qmls_request(string msg)
    function qmlf_message(msg)
    {
        console.log("gui: ", msg);
    }

    Column {
        x: 20
        y: 20
        spacing: 10
        Row {
            spacing: 5
            Rectangle {
                border.width: 1
                border.color: "black"
                width: 200
                height: 50
                TextEdit {
                    id: i_broadcast_target
                    anchors.fill: parent
                    font.pixelSize: 20
                    verticalAlignment: TextEdit.AlignVCenter
                    horizontalAlignment: TextEdit.AlignHCenter
                }
            }
            Button {
                id: i_mic_on
                text: qsTr("MIC ON")
                onClicked: qmls_request(i_broadcast_target.text + "|MIC_ON")
            }
            Button {
                id: i_mic_off
                text: qsTr("MIC OFF")
                onClicked: qmls_request(i_broadcast_target.text + "|MIC_OFF")
            }
        }
        Row {
            spacing: 5
            Rectangle {
                border.width: 1
                border.color: "black"
                width: 400
                height: 50
                TextEdit {
                    id: i_broadcast_text_value
                    anchors.fill: parent
                    font.pixelSize: 20
                    verticalAlignment: TextEdit.AlignVCenter
                    horizontalAlignment: TextEdit.AlignHCenter
                }
            }
            Button {
                id: i_boradcast_text
                text: qsTr("TEXT")
                onClicked: qmls_request(i_broadcast_target.text + "|TEXT|"+
                                        i_broadcast_text_value.text)
            }
        }
    }
}
