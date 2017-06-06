import QtQuick 2.7
import QtQuick.Layouts 1.3

Rectangle {
    id: container
    property real shadef: shade/256.0
    color: Qt.rgba(shadef, shadef, shadef, 1)

    StackLayout {
        id: stack
        currentIndex: 0
        anchors.fill: parent

        PhotoView {}
    }
}
