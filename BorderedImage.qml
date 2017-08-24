import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {
    property alias source: img.source
    property alias sourceSize: img.sourceSize
    property alias fillMode: img.fillMode
    property alias asynchronous: img.asynchronous

    property int borderWidth: 0
    property string borderColor: "white"
    property bool hovered: false

    Image {
        id: img
        width: parent.width - 2*parent.borderWidth
        height: parent.height - 2*parent.borderWidth
        anchors.centerIn: parent
        onStatusChanged: {
            if (status == Image.Ready) {
                borderrect.width = Math.min(Math.floor(img.paintedWidth), img.width) + 2*parent.borderWidth
                borderrect.height = Math.min(Math.floor(img.paintedHeight), img.height) + 2*parent.borderWidth
            }
        }
    }

    ColorOverlay {
        id: overlay
        visible: parent.hovered
        anchors.fill: img
        source: img
        color: "#18ffffff"
    }

    Rectangle {
        id: borderrect
        anchors.centerIn: img
        width: parent.width // Math.min(Math.floor(img.paintedWidth), img.width) + 2*parent.borderWidth
        height: parent.height //Math.min(Math.floor(img.paintedHeight), img.height) + 2*parent.borderWidth
        border.width: parent.borderWidth
        border.color: parent.borderColor
        color: "transparent"
    }
}
