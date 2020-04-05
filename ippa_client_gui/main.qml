import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Client")
    signal qmls_request(string msg)
    function qmlf_message(msg)
    {
        console.log("gui: ", msg);
        i_text.text = msg;
    }
    Column {
        anchors.centerIn: parent
        Label {
            id: i_text
            text: ""
            font.pixelSize: 30;
        }
    }

}
