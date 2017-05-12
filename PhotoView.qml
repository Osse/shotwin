import QtQuick 2.7

Rectangle {
    id: container
    color: "grey"

    GridView {
        id: photoView

        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 20

        // The standard size. height is always equal to this, but
        // the size changes to make stuff centered
        property int idealCellHeight: 200
        property int idealCellWidth: 200

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

                color: "transparent"
                border.color: "white"
                border.width: 1
                Image {
                    id: img
                    fillMode: Image.PreserveAspectFit
                    width: parent.width - 2*parent.border.width
                    height: parent.height - 2*parent.border.width
                    sourceSize.width: width
                    sourceSize.height: height
                    anchors.centerIn: parent
                    source: "image://thumbnails/" + display
                }
            }
        }
    }
}
