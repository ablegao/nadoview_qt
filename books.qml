import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtNetwork
import NadoView 1.0
//import QtQuick.Effects
import Qt5Compat.GraphicalEffects

import QtWebView 1.1

Item {
    id:bookListComponent

    UserData{
        id:userdata
    }
    TableOfContent {
        id:tableOfContent
    }
    GridView {
       id:book_list_grid
       anchors.fill: parent
       anchors.margins: 10
       cellWidth: 180
       cellHeight: 320
       clip:true


       model:userdata.books()

       delegate: Rectangle {
            width:book_list_grid.cellWidth
            height:book_list_grid.cellHeight
            anchors.margins: 4
            color:"#fffffe"
            Column{
                Image {
                    id:book_icon
                    width: book_list_grid.cellWidth-4
                    height: width/0.704
                    fillMode: Image.PreserveAspectFit
                    asynchronous:true
                    visible: modelData.book_image.length>0?true:false
                    source: visible?"data:image/png;base64,"+modelData.book_image:""  // 模拟图片源

                }
                Text {
                    id:title_icon
                    text:modelData.book_name
                    elide: Text.ElideRight
                    width:book_icon.width
                    height:book_icon.visible?book_list_grid.cellHeight-book_icon.height:book_list_grid.cellHeight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode :Text.Wrap
                    color:"#272343" // book_icon.visible? :"#e3f6f5"
//                            background:book_icon.visible?"":"#e3f6f5"
                    anchors.margins: 2
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: function(){

                    screen_win.setSource("qrc:/nadoview/main.qml",
                                         {
                                             bookUrl:modelData.book_url,
                                         });
                }
            }
        }
    }
}

