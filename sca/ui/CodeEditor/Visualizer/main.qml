import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

//    CFGView {
//    }

    Flickable {
         width: 200; height: 200
         contentWidth: image.width; contentHeight: image.height
         clip: true

         Image { id: image; source: "qrc:/bigImage.jpg" }
     }
}
