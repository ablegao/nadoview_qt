import QtQuick 2.15
import QtQuick.Controls 2.15
import NadoView 1.0
import QtWebEngine 1.10
Window {
    id: root
    visible: true
    width: 1400
    height: 800
    title: "Nado View"
    property string lang: "cn"
    property int screenWidthThreshold: 1600
    property int itemsPerPage: width >= screenWidthThreshold ? 2 : 1
    property string pageHref:"";
    property string pageDesc:"";
//    FontLoader {
//            id: customFont
//            source: "qrc:/fonts/AlibabaPuHuiTi-2-55-Regular.ttf" // 修改为自定义字体文件的相对路径
//    }

//    FontLoader{
//        id:customBoldFont
//        source:"qrc:/fonts/CN-Bold.ttf"
//    }

    FontLoader{
        id:iconFont
        source:"qrc:/fonts/fontawesome-webfont.ttf"
    }

    //:/

    function bookOpenFinishd(obj){
        root.title = "Currently using NadoView 0.1 to read《" + obj.name + "》";
        root.lang = obj.lang;
        var source = "image://mybook/"+obj.coverImg+"?180x240";
        console.log(obj.firstPageUrl);
        bookIcon.source = source;
        runingChatModel.addMessage("Reading "+obj.name,"system");

        if(tableOfContent.useMarkDown()){
            pageView.textFormat = Text.MarkdownText;
        }

        tocListView.currentIndex = 0;
        pageView.url = "mybook://book.local"+obj.firstPageUrl;
//        tableOfContent.openChapter(0);
        console.log("language ............",obj.lang,obj.firstPageUrl);
    }

    property var pageRows:[];
    function bookChapterReaded(obj){
        root.pageHref = obj.href;
        pageView.loadHtml(obj.html,"mbook://book.local/");
        root.pageDesc = obj.desc;

    }
    Component.onCompleted: function(){
        console.log("BOOK address:",bookUrl);
        tableOfContent.onBookOpenFinishd.connect(bookOpenFinishd);
        tableOfContent.onBookChapterReaded.connect(bookChapterReaded);
        if(bookUrl!="") tableOfContent.openBook(bookUrl);

    }

    SplitView {
        id:mainSplitView
        orientation:Qt.Horizontal
        anchors.fill: parent
        Rectangle{
            implicitHeight: mainSplitView.orientation == Qt.Vertical? parent.height/2:parent.height
            implicitWidth:mainSplitView.orientation == Qt.Vertical?parent.width:parent.width-chatBox.implicitWidth

            SplitView {
                id: splitView
                anchors.fill: parent
                orientation:Qt.Horizontal
                Rectangle{
                    id:leftBox
                    SplitView.minimumWidth: 0
                    SplitView.maximumWidth:  400
                    implicitWidth: 0
                    color:"#fffffe"
                    Column{
                        Rectangle{
                            id:bookIconBox
                            width:leftBox.width
                            height:200 +20
            //                border.width:1
                            Image{
                                id:bookIcon
                                source:""
                                anchors.margins: 10
                                anchors.centerIn: parent
                            }
                        }
                        ListView {
                            id: tocListView
                            anchors.margins: 10
                            width:leftBox.width
                            height:leftBox.height - bookIconBox.height
                            model: tableOfContent
                            delegate:  Item{
                                width: tocListView.width
                                height: textItem.implicitHeight
                                Text {
                                    id:textItem
                                    text: chapterName
                                    font.pixelSize: 16
                                    padding: 8
                                    width:parent.width
                                    wrapMode:Text.Wrap
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: function(){

                                        tocListView.currentIndex = index;
                                        pageView.url = "mybook://book.local"+chapterUrl;

                                    }
                                }
                            }
                            highlight: Rectangle {
                                color: "#bae8e8"
                            }
                            highlightMoveDuration:100
                            focus: true
                            clip:true

                        }
                    }
                }
                Rectangle{
                    SplitView.minimumWidth: 300
                    SplitView.maximumWidth:  parent.width
                    id:centerBox
                    color:"#fffffe"
                    implicitWidth: (root.width-leftBox.width)/2


                    WebEngineView {
                            id:pageView

                            height:parent.height - 35
                            width: parent.width

                            onLoadingChanged: function (loadRequest) {
                                console.log(loadRequest.status);
                                if (loadRequest.status ===2) {
                                    console.log("page load success..");
                                    runJavaScript(`var paragraphs = document.getElementsByTagName('p');
                                                  for (var i = 0; i < paragraphs.length; i++)
                                                  { paragraphs[i].style.textIndent = '2em'; }

                                                  `);
                                    runJavaScript(`var imgs = document.getElementsByTagName('img');
                                                  for (var i = 0; i < imgs.length; i++)
                                                  { imgs[i].style.maxWidth = '100%'; }

                                                  `);
                                    runJavaScript("document.documentElement.style.overflowX = 'hidden';");
                                }
                            }

                    }

                    Rectangle{
                        color:"#bae8e8"
                        id:webview_btns
                        width:parent.width
                        anchors.top: pageView.bottom
                        height:35
    //                    border.width: 1

                        Row {
                            id:tool_row
                            anchors.centerIn: parent
                            spacing: 5
                            property int  cellHeight: 26
                            Repeater {
                                model: ["#FFA07A", "#98FB98", "#87CEFA", "#FFFACD","#FFFFFF"]
                                delegate: Rectangle {
                                    id: colorBlock
                                    width: parent.cellHeight
                                    height: parent.cellHeight
                                    color:  modelData
                                    radius: 2
                                    border.width:1
    //                                function getColor  (index) {
    //                                    console.log("index==== ",index);
    //                                    var colors = ["#FFA07A", "#98FB98", "#87CEFA", "#FFFACD","#FFFFFF"]
    //                                    return colors[index]
    //                                }
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            pageView.runJavaScript(`
                                                            var selectedText = window.getSelection();
                                                            var range = selectedText.getRangeAt(0);
                                                            var span = document.createElement('span');
                                                            span.style.backgroundColor ="`+modelData+`";
                                                            range.surroundContents(span);
                                                        `);
    //                                        pageView.text
    //                                        console.log("Clicked color:", colorBlock.color)
    //                                        pageView.text= pageView.text.replace(pageView.selectedText,"<span color='"+colorBlock.color+"'>"+pageView.selectedText+"</span>")
                                        }
                                    }


                                }
                            }

                            Text {
                                id: open_menu
                                width: parent.cellHeight
                                height: parent.cellHeight
    //                            radius: 2
                                font.pixelSize: parent.cellHeight
                                text:''

    //                            border.width:1
                                font.family: iconFont.name
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {

                                        if(leftBox.implicitWidth==0) leftBox.implicitWidth=200;
                                        else leftBox.implicitWidth=0;
                                    }
                                }
                            }
                            Text {

                                width: parent.cellHeight
                                height: parent.cellHeight
    //                            radius: 2
                                font.pixelSize: parent.cellHeight
                                text:''

    //                            border.width:1
                                font.family: iconFont.name
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: function(){
                                         // 上一页
                                       var u = tableOfContent.prevPage();
                                        if(u!=="") pageView.url = "mybook://book.local"+u;
                                    }
                                }
                            }

                            Text {
                                width: parent.cellHeight
                                height: parent.cellHeight
    //                            radius: 2
                                font.pixelSize: parent.cellHeight
                                text:''

    //                            border.width:1
                                font.family: iconFont.name
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked:function() {

                                       // 下一页
                                        var u= tableOfContent.nextPage();
                                        if(u!=="")
                                        pageView.url = "mybook://book.local"+u;
                                    }
                                }
                            }



                        }


                    }

                }

            }
        }


        Rectangle{
                color:"#fffffe"
                id:chatBox
                implicitWidth: mainSplitView.orientation == Qt.Vertical?parent.width:parent.width/2
                OpenChatModel {
                    id:runingChatModel
                }



                ListView{
//                    spacing: 10
                    anchors.margins: 20
                    id:message_list
                    width:parent.width
                    height:parent.height -input_box.height-controller_box.height
                    model:runingChatModel
                    spacing: 10
                    clip: true
                    delegate:  Rectangle{
                        color:chatRole=="user"?"#bae8e8":"#e3f6f5";
                        radius:5
                        x:10
                        y:5
                        width:message_list.width-20
                        height:chatItem.contentHeight + chatItem.padding*2
                        TextEdit{
                            text:chatMessage
                            width:parent.width
                            height:parent.height
                            padding:10
                            font.pixelSize: 14
                            id:chatItem
                            wrapMode:TextEdit.WordWrap
//                        anchors.margins: 10
                        }
                    }

                }

                Rectangle{
                    color:"#bae8e8"
                    id:controller_box
                    width:parent.width
                    anchors.top: message_list.bottom
                    height:35
//                    border.width: 1


                    Row {
//                        id:tool_row
                        anchors.centerIn: parent
                        spacing: 5
                        property int  cellHeight: 26
                        visible: parent.width>100?true:false
                        Text {
                            id: copy_code
                            width: parent.cellHeight
                            height: parent.cellHeight
//                            radius: 2
                            font.pixelSize: parent.cellHeight
                            text:''

//                            border.width:1
                            font.family: iconFont.name
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    pageView.runJavaScript("window.getSelection().toString();", function(selectedText) {
                                        chat_input_text.text+= selectedText;

                                    });
                                }
                            }
                        }
                        Text {
                            id: search_by_dict
                            width: parent.cellHeight
                            height: parent.cellHeight
//                            radius: 2
                            font.pixelSize: parent.cellHeight
                            text:''

//                            border.width:1
                            font.family: iconFont.name
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    chat_input_text.text+= pageView.selectedText;
//                                        console.log("Clicked color:", colorBlock.color)
                                }
                            }
                        }
                        ComboBox{
                            id:openai_mode_names
                            model: ListModel {
                                ListElement { text: "gpt-3.5-turbo" }
                                ListElement { text: "gpt-3.5" }
                                ListElement { text: "gpt-4" }
                                ListElement { text: "gpt-4-turbo" }
                            }
                        }
                    }


                }
                Rectangle{
                    id:input_box
                    anchors.top:controller_box.bottom
                    width:parent.width
//                    border.width: 1
                    color:"#e3f6f5"
                    height:chat_input_text.contentHeight>500?500:chat_input_text.contentHeight+50
                    ScrollView{
                        anchors.fill: parent
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOff

                        TextArea {
                            id:chat_input_text
    //                        width:parent.width
                            placeholderText:"Please enter your question, or select text and click “”.\nPress ⌘ + ↩ to send message."
                            font.pixelSize: 16
                            width:parent.width
                            height:input_box.height -50
                            font.family: iconFont.name
    //                        anchors.margins: 10
                            wrapMode:TextEdit.Wrap
                            padding: 10
                            focus: true
                            Keys.forwardTo: [keyHandler]
                        }
                        Item {
                            id: keyHandler
                            focus: true
                            Keys.enabled: true
                            Keys.onPressed: function(event) {
                                if (Qt.platform.os === "osx") {
                                        if (event.key === Qt.Key_Return && (event.modifiers & Qt.ControlModifier)) {
                                            console.log("Command + Return pressed");
                                            event.accepted = true;
                                            var msgInfo = chat_input_text.text;
                                            chat_input_text.text = "";
                                            runingChatModel.sendMessage(msgInfo,openai_mode_names.currentText);


                                        }
                                } else {
                                    if (event.key === Qt.Key_Return && (event.modifiers & Qt.MetaModifier)) {
                                        console.log("Control + Return pressed");
                                        event.accepted = true;
                                        var msgInfo = chat_input_text.text;
                                        chat_input_text.text = "";
                                        runingChatModel.sendMessage(msgInfo,openai_mode_names.currentText);
                                    }
                                }
                            }
                        }
                    }
                }
        }
    }

    Timer {
            id: selectionTimer
            interval: 1000 // 1 second delay
            running: false
            repeat: false
            onTriggered: {
//                console.log("Selected text after delay:", myTextEdit.selectedText);
//                floatingBox.open();
            }
    }

}
