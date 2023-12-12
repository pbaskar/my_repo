import QtQuick 2.15

Item {
    ListModel {
        id: nodes
        ListElement {
            rowNum: 0
            colNum: 0
        }
        ListElement {
            rowNum: 1
            colNum: 0
        }
        ListElement {
            rowNum: 1
            colNum: 1
        }
    }
    ListModel {
        id: edges
        ListElement {
            rowNum: 0
            colNum: 0
        }
        ListElement {
            rowNum: 1
            colNum: 0
        }
        ListElement {
            rowNum: 1
            colNum: 1
        }
    }

    DropArea {
        anchors.fill: parent
    }

    Repeater {
        id: nodesRepeat
        model: nodes
        delegate: Rectangle {
            width: 100
            height: 100
            color: "green"
            x: colNum*101
            y: rowNum*101
            Drag.active: mouseArea.drag.active
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                drag.target: parent
                onClicked: {
                    console.log("row, column " , rowNum, colNum)
                }
            }
        }
    }
    Repeater {
        anchors.top: nodesRepeat.bottom
        model: edges
        delegate: Rectangle {
            width: 1
            height: 100
            color: "red"
            x: colNum*101
            y: rowNum*101
            rotation: 30
            Drag.active: mouseArea.drag.active
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                drag.target: parent
                onClicked: {
                    console.log("row, column " , rowNum, colNum)
                }
            }
        }
    }
}
