import QtQuick 2.7
import QtQuick.Layouts 1.3

Rectangle {
    id: container
    property real shadef: shade/256.0
    color: Qt.rgba(shadef, shadef, shadef, 1)

    FocusScope {
        anchors.fill: parent
        focus: true

        StackLayout {
            id: stack
            property int viewIndex: -1

            currentIndex: 0
            anchors.fill: parent

            PhotoView {}
            EventView {}
            FullScreenView {}
        }
    }

    Connections {
        target: shotwin
        onPhotoListRequested: stack.currentIndex = 0
    }

    Connections {
        target: shotwin
        onEventListRequested: stack.currentIndex = 1
    }
}
