import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects
import QtQml.Models
import QtWebView 1.1

Item {
	id:bookListComponent
	property var userdata;	
	property var appSingle;

    Component.onCompleted: {
        userdata.booksAsync();
        userdata.onFetchBook.connect(function(obj){
            if(book_list_grid)
            book_list_grid.model.append(obj);

        });
        var colors=["#e3f6f5","#ffd803","#FFA07A", "#98FB98", "#87CEFA", "#FFFACD"];
        userdata.onTags.connect(function(tags){
            var tagMap={};
            for(var i=0;i<tags.length;i++){
                if(tagMap[tags[i]]===undefined) tagMap[tags[i]]=0;
                tagMap[tags[i]]++;
            }
            var tagConfig = [];

            var n=0;
            for(var i in tagMap){
//                tagConfig.push({text:i+"("+tagMap[i]+")",fontSize:14});
                tagConfig.push({text:i,fontSize:14,color:colors[n%colors.length],num:tagMap[i]});
                n++;
            }
            tagConfig.push({text:"All",fontSize:16,color:"#98FB98"});
            if(tagCloudRepeater) tagCloudRepeater.model = tagConfig;
        });

    }
    function fullWidth(max){
        var min_width  = 160;
        var num = Math.floor(max/min_width);
        var remainder = max % min_width;
        if(remainder>0){
            return Math.floor(remainder/num+min_width);
        }else{
            console.log(min_width);
            return min_width;
        }
    }
    Flow {
            id: tagCloud
            width:parent.width
            spacing: 8
            clip: true
            Repeater {
                id:tagCloudRepeater
                model: []

                delegate: Rectangle {
                    width: textBox.width + 16
                    height: textBox.height + 8
                    color:modelData.color
                    Text {
                        id: textBox
                        text: modelData.text
                        font.pixelSize: modelData.fontSize
//                        color: "black"
                        anchors.centerIn: parent
                    }
                    MouseArea {
                            anchors.fill: parent
                            onClicked: function(){
                                book_list_grid.model.clear();
                                if(modelData.text==="All")userdata.booksAsync()
                                else userdata.booksAsync(modelData.text);
                            }
                    }
                }
            }
    }
    GridView {
       id:book_list_grid
       width:parent.width
       height: parent.height - tagCloud.height
       y:tagCloud.height+4
       anchors.margins: 0
       cellWidth: fullWidth(parent.width)
       cellHeight: fullWidth(parent.width) +100
       clip:true

       model:ListModel {
       }
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
                    visible: book_image.length>0?true:false
                    source: visible?"file://"+book_image:""  // 模拟图片源
                }
            
                Text {
                    id:title_icon
                    text:book_name
                    elide: Text.ElideRight
                    width:book_icon.width
                    height:book_icon.visible?book_list_grid.cellHeight-book_icon.height:book_list_grid.cellHeight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode :Text.Wrap
                    color:"#272343" // book_icon.visible? :"#e3f6f5"
//                            background:book_icon.visible?"":"#e3f6f5"
                    anchors.margins: 0
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: function(){
                    appSingle.fileOpend(book_url);
                    //screen_win.setSource("qrc:/nadoview/main.qml",
                    //                     {
                    //                         bookUrl:modelData.book_url,
                    //                     });
                }
            }
        }
    }
}
