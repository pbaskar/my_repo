import QtQuick 2.15

Item {
    Component.onCompleted: {
        nodes.insert(0,{xPos:50, yPos:50});
        nodes.insert(1,{xPos:100, yPos:50});
        nodes.insert(2,{xPos:50, yPos:100});
    }

    ListModel {
        id: nodes
        /*ListElement {
            xPos: 50
            yPos: 50
        }
        ListElement {
            xPos: 100
            yPos: 50
        }
        ListElement {
            xPos: 50
            yPos: 100
        }*/
    }

//    Component {
//        id: ifElseBlockComponent
//        function addToIf() {
//            var basicBlock = basicBlockComponent.createObject(ifBlockColumn)
//        }

//        Rectangle {
//            color: "green"
//            implicitWidth: ifelseBlock.width
//            implicitHeight: ifelseBlock.height

//            Row {
//                id: ifelseBlock
//                spacing: 20
//                anchors.margins: 10
//                Column {
//                    id: ifBlockColumn
//                    spacing: 20
//                    Component.onCompleted: {
//                        //var basicBlock = basicBlockComponent.createObject(ifBlockColumn)
//                        //var basicBlock = basicBlockComponent.createObject(ifBlockColumn)
//                        console.log("Component.onCompleted: ifElseBlockComponent")
//                    }
//                }
//                Column {
//                    id: elseBlockColumn
//                    spacing: 20
//                    Component.onCompleted: {
//                        //var basicBlock = basicBlockComponent.createObject(elseBlockColumn)
//                        //var basicBlock = basicBlockComponent.createObject(elseBlockColumn)
//                        console.log("Component.onCompleted: ifElseBlockComponent")
//                    }
//                }
//            }
//        }
//    }

//    Component {
//        id: basicBlockComponent
//        Rectangle {
//            height: 50
//            width: 50
//            color: "red"
//        }
//    }

//    Component.onCompleted: {
//        //var ifElseBlock = ifElseBlockComponent.createObject(parent)
//        //var ifElseBlock = ifElseBlockComponent.createObject(parent)
//        console.log("factorial value " ,factorial(1))
//        console.log("Component.onCompleted: CFGView.qml")
//    }

//    DropArea {
//        anchors.fill: parent
//    }

    Repeater {
        id: nodesRepeat
        model: nodes
        delegate: Rectangle {
            width: 25
            height: 25
            color: "green"
            x: xPos
            y: yPos
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
//    Repeater {
//        anchors.top: nodesRepeat.bottom
//        model: edges
//        delegate: Rectangle {
//            width: 1
//            height: 100
//            color: "red"
//            x: colNum*101
//            y: rowNum*101
//            rotation: 30
//            Drag.active: mouseArea.drag.active
//            MouseArea {
//                id: mouseArea
//                anchors.fill: parent
//                drag.target: parent
//                onClicked: {
//                    console.log("row, column " , rowNum, colNum)
//                }
//            }
//        }
//    }
}
