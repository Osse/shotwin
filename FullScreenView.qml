import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtMultimedia 5.9

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
        id: wrapper
        width: GridView.view.cellWidth
        height: GridView.view.cellHeight

        StackLayout {
            id: layout
            anchors.fill: parent
            Image {
                source: "image://thumbnails/" + thumbnail
                fillMode: Image.PreserveAspectFit
            }
            Loader {
                id: loader
                active: wrapper.visible
                sourceComponent: type == "photo" ? image : video

                Component {
                    id: image
                    Image {
                        asynchronous: true
                        source: "file:///" + shotwin.mappedFile(filename)
                        fillMode: Image.PreserveAspectFit
                        onStatusChanged: if (status == Image.Ready) { layout.currentIndex = 1; }
                        autoTransform: true

                        MouseArea {
                            anchors.fill: parent
                            onDoubleClicked: stack.currentIndex = 0
                        }
                    }
                }
                Component {
                    id: video
                    Video {
                        source: "file:///" + shotwin.mappedFile(filename)
                        onStatusChanged: if (status == MediaPlayer.Loaded) { layout.currentIndex = 1; }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (parent.playbackState != MediaPlayer.PlayingState)
                                    parent.play();
                                else
                                    parent.pause();
                            }
                            onDoubleClicked: {
                                parent.stop()
                                stack.currentIndex = 0
                            }
                        }
                    }
                }
            }
        }

        Keys.onEscapePressed:  stack.currentIndex = 0

        Keys.onUpPressed: GridView.view.moveCurrentIndexLeft()
        Keys.onLeftPressed: GridView.view.moveCurrentIndexLeft()
        Keys.onRightPressed: GridView.view.moveCurrentIndexRight()
        Keys.onDownPressed: GridView.view.moveCurrentIndexRight()
    }
}
