import QtQuick 2.7
import QtQuick.Controls 1.4

Item {
    anchors.fill: parent
    CenteredGridView {
        id: photoView
        objectName: 'photoView'

        clip: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: slider.top
        anchors.right: parent.right

        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 20

        property int tagTextHeight: 20

        idealCellHeight: slider.value + tagTextHeight/2
        idealCellWidth: slider.value - tagTextHeight/2

        focus: visible

        signal tagClicked(string tag)

        model: photoListModel

        Connections {
            target: photoListModel
            onFilterChanged: photoView.currentIndex = -1
        }

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
                height: parent.height - photoView.tagTextHeight

                BorderedImage {
                    id: img
                    asynchronous: true
                    fillMode: Image.PreserveAspectFit
                    width: parent.width
                    height: parent.height - 20

                    // This makes the image provider get a proper requestedSize
                    sourceSize.width: width
                    sourceSize.height: height

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    source: "image://thumbnails/" + thumbnail //+ "/" + id + "/" + type + "/" + mappedfilename

                    borderWidth: 1
                    borderColor: "white"
                }
                Text {
                    function formatTagsList(strings, hovered) {
                        var linkformat = "<a href=\"%1\">%1</a>";
                        var hoveredlinkformat = "<a href=\"%1\"><b>%1</b></a>";
                        for(var i = 0; i < strings.length; i++) {
                            if (strings[i] === hovered)
                               strings[i] = hoveredlinkformat.arg(strings[i]);
                            else
                               strings[i] = linkformat.arg(strings[i]);
                        }

                        return strings.join(", ");
                    }
                    height: photoView.tagTextHeight
                    verticalAlignment: Text.AlignBottom
                    anchors.bottom: parent.bottom
                    color: "white"
                    linkColor: "white"
                    text: formatTagsList(shotwin.getTagsForPhoto(id), hoveredLink)
                    onLinkActivated: {
                        photoView.tagClicked(link)
                        stack.currentIndex = 0
                    }
                }
                MouseArea {
                    anchors.fill: img
                    hoverEnabled: true

                    onContainsMouseChanged: img.hovered = containsMouse

                    onClicked: photoView.currentIndex = index

                    onDoubleClicked: stack.currentIndex = 2
                }
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
