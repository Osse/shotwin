import QtQuick 2.7
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1

CenteredGridView {
    id: eventView
    objectName: 'eventView'
    clip: true
    anchors.fill: parent

    anchors.leftMargin: 20
    anchors.rightMargin: 20
    anchors.topMargin: 20

    idealCellHeight: 305
    idealCellWidth: 300

    model: eventListModel

    focus: visible

    signal eventDoubleClicked(int index)

    delegate: Item {
        // The delegate size is equal to the cell size
        height: GridView.view.cellHeight
        width: GridView.view.cellWidth

        Item {
            // The visible part is not. The height is as mentioned above,
            // But the width is here set to the ideal width.
            anchors.centerIn: parent
            width: parent.GridView.view.idealCellWidth - 20
            height: parent.GridView.view.idealCellHeight - 20

            BorderedImage {
                id: img
                asynchronous: true
                fillMode: Image.PreserveAspectCrop
                width: parent.width - 3 - 50
                height: width
                sourceSize.width: width
                sourceSize.height: height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                source: "image://thumbnails/" + thumbnail
                borderColor: "white"
                borderWidth: 4
            }

            function eventCountString(photos, videos) {
                var format = "%1 %2%3";
                var string = []
                if (photos)
                    string.push(format.arg(photos).arg("photo").arg(photos > 1 ? "s" : ""))
                if (videos)
                    string.push(format.arg(videos).arg("video").arg(videos > 1 ? "s" : ""))

                return string.join(", ");
            }

            ColumnLayout {
                anchors.top: img.bottom
                anchors.topMargin: 5
                //anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 2
                property string color: container.shadef > 0.6 ? "black" : "white"
                Repeater {
                    model: display.length ? [ display, timespan, parent.parent.eventCountString(photocount, videocount) ] :
                                            [ timespan, parent.parent.eventCountString(photocount, videocount) ]
                    Text {
                        text: modelData
                        color: parent.color
                        font.bold: index == 0 ? true : false
                    }
                }
            }

            function goToEventPhotos() {
                eventView.eventDoubleClicked(index)
                stack.currentIndex = 0
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                onContainsMouseChanged: img.hovered = containsMouse

                onClicked: eventView.currentIndex = index

                onDoubleClicked: parent.goToEventPhotos()
            }
            Keys.onEnterPressed: goToEventPhotos()
        }
    }

    highlight: Item {
        Rectangle {
            anchors.centerIn: parent
            width: eventView.idealCellWidth
            height: eventView.idealCellHeight
            color: Qt.lighter(container.color)
        }
    }
    highlightFollowsCurrentItem: true
}
