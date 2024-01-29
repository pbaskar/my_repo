import QtQuick 2.15
import com.model.cfgModel 1.0

Item {
    Component.onCompleted: {
        console.log("Component.onCompleted: CFGView.qml")
        nodes.insert(0,{xPos:50, yPos:50});
        nodes.insert(1,{xPos:100, yPos:50});
        nodes.insert(2,{xPos:50, yPos:100});
    }

    CFGModel {
        id: cfgModel
    }

    ListModel {
        id: nodes
        ListElement {
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
        }
    }

//    DropArea {
//        anchors.fill: parent
//    }

    Repeater {
        id: nodesRepeat
        model: cfgModel
        delegate: Rectangle {
            border.color: "green"
            color: "transparent"
            width: model.display.width
            height: model.display.height
            x: model.display.xPos
            y: model.display.yPos
            Drag.active: mouseArea.drag.active
            property var stmts : model.display.stmtList
            ListModel {
                id: stmtListModel
            }
            Component.onCompleted: {
                for(var i=0; i<stmts.length; i++) {
                    stmtListModel.append({"name":stmts[i]});
                    console.log("stmtListModel ", stmtListModel.count)
                }
            }
            ListView {
                id: listView
                anchors.fill: parent
                model: stmtListModel
                delegate: Text {
                       text: name
                    }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                drag.target: parent
                onClicked: {
                    console.log("x, y " , model.display.xPos, model.display.yPos, model.display.stmtList)
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
