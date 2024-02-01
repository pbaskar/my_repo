import QtQuick 2.15
import QtQuick.Controls
import com.model.cfgModel 1.0

//Flickable {
//     width: 200; height: 200
//     contentWidth: image.width; contentHeight: image.height
//     clip: true

//     Image { id: image; source: "qrc:/bigImage.jpg" }
// }

Flickable {
    anchors.fill: parent
    contentHeight: nodesRepeat.height
    contentWidth: nodesRepeat.width
    clip: true

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
        implicitWidth: cfgModel.totalWidth + 25
        implicitHeight: cfgModel.totalHeight + 25
        model: cfgModel
        delegate: Rectangle {
            border.color: "green"
            color: "transparent"
            radius: 10
            implicitWidth: model.display.width
            implicitHeight: model.display.height
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
                anchors { left: parent.left; leftMargin: 15; rightMargin: 15; right: parent.right
                          top: parent.top; topMargin: 15; bottomMargin: 15; bottom: parent.bottom }
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
                    console.log("nodesrepeat width height ", nodesRepeat.height, nodesRepeat.width, nodesRepeat.childrenRect.width,
                                nodesRepeat.childrenRect.height)
                }
            }
        }
    }

    CFGEdge {
        height: 500
        width: 500
        anchors {left: parent.left; top: parent.top; }
    }

//    Repeater {
//        model: cfgModel
//        delegate: CFGEdge {
//            anchors.fill: parent
////            Drag.active: edgeMouseArea.drag.active
////            MouseArea {
////                id: edgeMouseArea
////                anchors.fill: parent
////                drag.target: parent
////                onClicked: {
////                    console.log("row, column " , rowNum, colNum)
////                }
////            }
//        }
//    }
}
