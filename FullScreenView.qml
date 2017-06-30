import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

GridView {
    id: fullScreenView
    anchors.fill: parent

    model: photoListModel
    cellHeight: height
    cellWidth: width

    focus: visible

    interactive: false
    highlightMoveDuration: 0

    Binding {
        target: fullScreenView
        property: "currentIndex"
        value: stack.viewIndex
    }

    onCurrentIndexChanged: stack.viewIndex = currentIndex

    delegate: Item {
        width: GridView.view.cellWidth
        height: GridView.view.cellHeight

        StackLayout {
            anchors.fill: parent
            Image {
                source: "image://thumbnails/" + thumbnail
                fillMode: Image.PreserveAspectFit
            }
            Image {
                asynchronous: true
                source: "image://pictures/" + filename
                fillMode: Image.PreserveAspectFit
                onStatusChanged: if (status == Image.Ready) { parent.currentIndex = 1 }
            }
        }

        Keys.onEscapePressed:  stack.currentIndex = 0

        Keys.onUpPressed: GridView.view.moveCurrentIndexLeft()
        Keys.onLeftPressed: GridView.view.moveCurrentIndexLeft()
        Keys.onRightPressed: GridView.view.moveCurrentIndexRight()
        Keys.onDownPressed: GridView.view.moveCurrentIndexRight()
    }
}
