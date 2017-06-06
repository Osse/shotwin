import QtQuick 2.7
import QtQuick.Controls 1.4

Item {
    anchors.fill: parent
    GridView {
        id: photoView
        clip: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: slider.top
        anchors.right: parent.right

        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 20

        // The standard size. height is always equal to this, but
        // the size changes to make stuff centered
        property int idealCellHeight: slider.value
        property int idealCellWidth: slider.value

        // The actual cell height is always as desired, but the cell width
        // is calculated from the current width of the view and how many cells fit
        cellHeight: idealCellHeight
        cellWidth: width / Math.floor(width / idealCellWidth)

        model: photoListModel

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

                Image {
                    id: img
                    fillMode: Image.PreserveAspectFit
                    width: parent.width - 2
                    height: parent.height - 2
                    sourceSize.width: width
                    sourceSize.height: height
                    anchors.centerIn: parent
                    source: "image://thumbnails/" + thumbnail

                    Rectangle {
                        anchors.centerIn: parent
                        width: parent.paintedWidth + 2
                        height: parent.paintedHeight + 2
                        border.width: 1
                        border.color: "white"
                        color: "transparent"
                    }
                }
            }
        }
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
