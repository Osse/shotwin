import QtQuick 2.7

Image {
    id: img

    property int borderWidth: 0
    property string borderColor: "white"

    anchors.topMargin: borderWidth

    Rectangle {
        id: borderrect

        anchors.centerIn: parent
        width: Math.min(parent.paintedWidth, parent.width) + 2*parent.borderWidth
        height: Math.min(parent.height, parent.paintedHeight) + 2*parent.borderWidth
        border.width: parent.borderWidth
        border.color: parent.borderColor
        color: "transparent"
    }
}
