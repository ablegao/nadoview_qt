import QtQuick 2.15
import QtQuick.Controls 2.15
import NadoView 1.0
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
        console.log(source);
        bookIcon.source = source;
        runingChatModel.addMessage("Reading "+obj.name,"system");

        if(tableOfContent.useMarkDown()){
            pageView.textFormat = Text.MarkdownText;
        }

//        console.log(obj.coverImg,bookIcon.source,"图片........");
        tocListView.currentIndex = 0;
        tableOfContent.openChapter(0);
        console.log("language ............",obj.lang);
    }

    property var pageRows:[];
    function bookChapterReaded(obj){
//        pageView.loadHtml(obj.html,obj.baseUrl);

//        pageView.url = obj.url;
//        console.log(pageView.url);
//        pageView.reload();
        root.pageHref = obj.href;
//        pageView.model = obj.rows;
        pageView.text = obj.html;


//        root.pageRows = obj.rows;
//        for(var row in obj.rows){
//        if(root.pageRows>3){
//            for(var i=0;i<3;i++)
//            pageView.text = pageView.text + tableOfContent.markdownToHtml(obj.rows[i],pageView.font.pixelSize,pageView.width,pageView.height);
//        }else{
//            for(var i=0;i<root.pageRows.length;i++)
//                pageView.text = pageView.text + tableOfContent.markdownToHtml(obj.rows[i],pageView.font.pixelSize,pageView.width,pageView.height);
//        }

//        }
        root.pageDesc = obj.desc;
//        openai_desc_text.text = "<a href='openAPI' >使用OpenAI 获取这一章节的简介</a>";

    }
    Component.onCompleted: function(){
        console.log("BOOK address:",bookUrl);
        tableOfContent.onBookOpenFinishd.connect(bookOpenFinishd);
        tableOfContent.onBookChapterReaded.connect(bookChapterReaded);
        if(bookUrl!="") tableOfContent.openBook(bookUrl);

    }
    SplitView {
        id: splitView
        anchors.fill: parent
        Rectangle{
            id:leftBox
            SplitView.minimumWidth: 100
            SplitView.maximumWidth:  400
            implicitWidth: 200
            color:"#fffffe"
            Column{
                Rectangle{
                    id:bookIconBox
//                    anchors.top: parent.top
//                    anchors.left: parent.left
//                    anchors.right: parent.right
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

//                    anchors.top: bookIconBox.bottom
//                    anchors.left: parent.left
//                    anchors.right: parent.right
//                    anchors.bottom: parent.bottom
                    anchors.margins: 10
//                    anchors.centerIn: parent
                    width:leftBox.width
                    height:leftBox.height - bookIconBox.height
    //                anchors.fill: parent
                    model: tableOfContent
                    delegate:  Item{
    //                    property  string indexId: chapterID
                        //// Unable to assign [undefined] to QString mData 中没有chapterID字段，需要数据中提供
    //                    property  string level: chapterLevel
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
                                tableOfContent.openChapter(index);
    //                            openai_desc_text.text= "<a href='openAPI' >使用OpenAI 获取这一章节的简介</a>";

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
            SplitView.maximumWidth:  900
            color:"#fffffe"
            implicitWidth: (root.width-leftBox.width)/2
            Flickable {
                id: flick

                anchors.fill: parent
                width: parent.width
                contentHeight: pageView.contentHeight+(pageView.padding*2)
                clip: true

                onContentYChanged: {
                    if (contentY >= contentHeight - height * 1.1) { // Adjust the multiplier as needed
//                        pageView.text += "\nAdditional text added.";
                    }
                }


                function ensureVisible(r)
                 {
                     if (contentX >= r.x)
                         contentX = r.x;
                     else if (contentX+width <= r.x+r.width)
                         contentX = r.x+r.width-width;
                     if (contentY >= r.y)
                         contentY = r.y;
                     else if (contentY+height <= r.y+r.height)
                         contentY = r.y+r.height-height;
                 }
                TextEdit{
                    id:pageView
                    anchors.fill: parent
//                    text:tableOfContent.markdownToHtml(modelData,pageView.fontSize,pageView.width,pageView.height)
                    text:""
                    width:parent.width
                    textFormat:  Text.RichText
                    baseUrl: "image://mybook"
//                    font.pixelSize: pageView.fontSize
                    wrapMode:Text.Wrap
    //                    font.family:  customBoldFont.name
//                    font.family: customFont.name
                    // +","+ customBoldFont.name
                    font.pixelSize: 16
                    selectByMouse: true
                    readOnly:true
                    onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
                    padding:10
                    onSelectionEndChanged:function(){
//                           chat_input_text.text = selectedText;
    //                        selectionTimer.restart();
    //                        floatingBox.selectedText = this.selectedText;
    //                        floatingBox.selectedIndex = index;
    //                        selectBoxText.text = selectedText;

    //                        let cursorPosition = pageView.currentItem.mapToItem(root.contentItem, pageView.currentItem.cursorRectangle.x, pageView.currentItem.cursorRectangle.y);

    //                        floatingBox.x =  cursorPosition.x;
    //                        floatingBox.y = cursorPosition.y;

                    }
                    onLinkActivated: function(link){
    //                        console.log(link);
                        if(link.lenght>4 && link.substring(4) == "http" ){
                            console.log("外部链接",link);
                        }else{
                            console.log("内部连击",link);

                            if(link.substring(0,3)!="../" && link.substring(0,1)!="/"){
                                var pathNum = root.pageHref.split("/");
                                var selfPathNum = link.split("/");
                                pathNum.pop();
                                if(selfPathNum[0]=="."){
                                    selfPathNum.shift();
                                }
                                var path = pathNum.concat(selfPathNum).join("/");

                                tableOfContent.openPageByUrl(path);
                            }else{
                                tableOfContent.openPageByUrl(link);
                            }
                        }
                    }

                }

            }
        }

        Rectangle{
                color:"#fffffe"

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
                        id:tool_row
                        anchors.centerIn: parent
                        spacing: 5
                        property int  cellHeight: 26
                        Repeater {
                            model: 4
                            delegate: Rectangle {
                                id: colorBlock
                                width: tool_row.cellHeight
                                height: tool_row.cellHeight
                                color: getColor(index)
                                radius: 2
                                border.width:1
                                function getColor  (index) {
                                    console.log("index==== ",index);
                                    var colors = ["#FFA07A", "#98FB98", "#87CEFA", "#FFFACD"]
                                    return colors[index]
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        pageView.text
//                                        console.log("Clicked color:", colorBlock.color)
                                        pageView.text= pageView.text.replace(pageView.selectedText,"<span color='"+colorBlock.color+"'>"+pageView.selectedText+"</span>")
                                    }
                                }


                            }
                        }

                        Text {
                            id: copy_code
                            width: tool_row.cellHeight
                            height: tool_row.cellHeight
//                            radius: 2
                            font.pixelSize: tool_row.cellHeight
                            text:''

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
        //
                        Text {
                            id: search_by_dict
                            width: tool_row.cellHeight
                            height: tool_row.cellHeight
//                            radius: 2
                            font.pixelSize: tool_row.cellHeight
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
