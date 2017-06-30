import QtQuick 2.7
import QtQuick.Controls 1.4

Item {
    anchors.fill: parent
    CenteredGridView {
        id: photoView
        clip: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: slider.top
        anchors.right: parent.right

        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 20

        idealCellHeight: slider.value
        idealCellWidth: slider.value

        focus: visible

        model: photoListModel

        Binding {
            target: photoView
            property: "currentIndex"
            value: stack.viewIndex
        }
        onCurrentIndexChanged: stack.viewIndex = currentIndex

        delegate: Item {
            // The delegate size is equal to the cell size
            height: GridView.view.cellHeight
            width: GridView.view.cellWidth
            Item {
                // The visible part is not. The height is as mentioned above,
                // But the width is here set to the ideal width.
                anchors.centerIn: parent
                width: parent.GridView.view.idealCellWidth - 20
                height: parent.height - 20

                BorderedImage {
                    id: img
                    fillMode: Image.PreserveAspectFit
                    width: parent.width
                    height: parent.height

                    // This makes the image provider get a proper requestedSize
                    sourceSize.width: width
                    sourceSize.height: height

                    anchors.centerIn: parent
                    source: "image://thumbnails/" + thumbnail

                    borderWidth: 1
                    borderColor: "white"
                }
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                onContainsMouseChanged: img.hovered = containsMouse

                onClicked: photoView.currentIndex = index

                onDoubleClicked: stack.currentIndex = 2
            }

            Keys.onReturnPressed: stack.currentIndex = 2
        }
        highlight: Item {
            Rectangle {
                anchors.centerIn: parent
                width: photoView.idealCellWidth
                height: photoView.idealCellHeight
                color: Qt.lighter(container.color)
            }
        }
        highlightFollowsCurrentItem: true
    }

    Slider {
        id: slider
        value: 200
        minimumValue: 50
        maximumValue: 300
        anchors.bottom: parent.bottom
        anchors.right: parent.right
    }
}
