import QtQuick 2.7

Rectangle {
    id: container
    color: "grey"


    Rectangle {
        anchors.margins: 20
        anchors.fill: parent
        color: "transparent"

        GridView {
            id: photoView

            width: parent.width - 20
            height: parent.height

            anchors.centerIn: parent

            // The standard size. height is always equal to this, but
            // the size changes to make stuff centered
            property var idealCellHeight: 200
            property var idealCellWidth: 200

            // The actual cell height is always as desired, but the cell width
            // is calculated from the current width of the view and how many cells fit
            cellHeight: idealCellHeight
            cellWidth: width / Math.floor(width / idealCellWidth)

            model: photoListModel

            delegate: Item {
                // The delegate size is equal to the cell size
                height: GridView.view.cellHeight
                width: GridView.view.cellWidth
                Rectangle {
                    // The visible part is not. The height is as mentioned above,
                    // But the width is here set to the ideal width.
                    anchors.centerIn: parent
                    width: parent.GridView.view.idealCellWidth - 20
                    height: parent.height - 20

                    color: "lightsteelblue"
                    border.color: "white"
                    border.width: 1
                    radius: 5
                    Image {
                        height: parent.height
                        width: parent.width
                        anchors.centerIn: parent
                        source: "image://thumbnails/kek"
                    }

                    Text {
                        width: parent.width
                        text: display
                        anchors.centerIn: parent
                        wrapMode: Text.Wrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}
