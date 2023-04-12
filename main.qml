import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtNetwork
import NadoView 1.0
//import QtWebView 1.1


Window {
    id: root
    visible: true
    width: 760
    height: 900
    title: "Nado View"

    property string lang: "cn"
    property int screenWidthThreshold: 1600
    property int itemsPerPage: width >= screenWidthThreshold ? 2 : 1
    property string pageHref:"";
    property string pageDesc:"";
    property  string bookName: ""
    property int onstartScrollTo: 0
    UserData{
        id:userdata
    }
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
        root.bookName = obj.name;
        root.lang = obj.lang;
        var source = "image://mybook/"+obj.coverImg+"?180x240";
        console.log(obj.firstPageUrl);
        bookIcon.source = source;
        runingChatModel.addMessage("Reading "+obj.name,"system");

//        if(tableOfContent.useMarkDown()){
//            pageView.textFormat = Text.MarkdownText;
//        }
        tableOfContent.setSize(flick.width,flick.height);
        tocListView.currentIndex = 0;
        var read = userdata.openBook(obj.name);
        console.log(JSON.stringify(read));
//        if(read.last_read_file!=="") pageView.url = "mybook://book.local"+tableOfContent.indexToUrl(read.last_read_index);
//        else pageView.url = "mybook://book.local"+obj.firstPageUrl;
        if(read.last_read_file!=="") pageView.text =  tableOfContent.openPage(tableOfContent.indexToUrl(read.last_read_index));
        else pageView.text = tableOfContent.openPage(obj.firstPageUrl);
//        pageView.text = "<p>test01</p><p style='text-indent:20px'>test01</p>"

        onstartScrollTo = read.last_read_scroll_number;
//        console.log(JSON.stringify(out));
//        tableOfContent.openChapter(0);
//        console.log("language ............",obj.lang,obj.firstPageUrl);
    }

    property var pageRows:[];
    function bookChapterReaded(obj){
//        root.pageHref = obj.href;
//        pageView.loadHtml(obj.html,"mbook://book.local/");
//        root.pageDesc = obj.desc;

    }
    Component.onCompleted: function(){
        console.log("BOOK address:",bookUrl);
        tableOfContent.onBookOpenFinishd.connect(bookOpenFinishd);
        tableOfContent.onBookChapterReaded.connect(bookChapterReaded);
        if(bookUrl!="") tableOfContent.openBook(bookUrl);

    }


    Rectangle{
        id:centerBox
        color:"#fffffe"
        width:parent.width
        height:parent.height
        NetworkManager {
            id:network_access_manager
        }
        Flickable {
             id: flick

             width: parent.width
             height:parent.height - webview_btns.height-20
             contentWidth: parent.width
             contentHeight: pageView.contentHeight +40
             onWidthChanged: function(){
//                console.log(flick.width);
                 tableOfContent.setSize(flick.width,flick.height);
             }
//             property int maximumFlickableX: contentWidth - width // 计算滚动的最大值
//             onContentXChanged: contentX = Math.min(Math.max(0, contentX), maximumFlickableX) // 当滚动超过最大值时，将滚动位置限制在最大值内
             clip: true
             function getScrollBarPosition(searchString) {
                 // 获取Flickable中的内容高度和可见高度
                 var contentHeight = flick.contentHeight
                 var visibleHeight = flick.height

                 // 获取TextEdit中的文本字符串
                 var text = pageView.toPlainText();

                 // 使用QFontMetrics测量TextEdit的字体和文本宽度
                 var fontMetrics = new QFontMetrics(textEdit.font)
                 var textWidth = fontMetrics.width(text)

                 // 使用QRegularExpression在文本字符串中查找所需的字符串，并计算它在整个字符串中的位置
                 var re = new QRegularExpression(searchString)
                 var match = re.match(text)
                 var position = match.capturedStart()

                 // 使用已知的内容高度、可见高度、文本宽度和字符串位置计算滚动条的位置
                 var scrollBarPosition = position * (contentHeight - visibleHeight) / (text.length - visibleHeight / fontMetrics.lineSpacing) / textWidth

                 return scrollBarPosition
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

            TextEdit {
                    id:pageView
//                    width: flick.width-20
//                    x:10
                    x:20
                    y:20
                    width: flick.width-40


                    textFormat: Text.RichText
                    wrapMode:TextEdit.Wrap
                    readOnly: true
                    selectByMouse: true
                    focus: true
                    baseUrl: "image://mybook/"
                    font.pixelSize: 16

                    onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
                    onLinkActivated:function(url){
//                        console.log();
                        pageView.text = tableOfContent.openPage(tableOfContent.absoluteFilePath(url));
                    }

            }
        }

        Rectangle{
            color:"#bae8e8"
            id:webview_btns
            width:parent.width
            anchors.top: flick.bottom
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
                    font.pixelSize: parent.cellHeight
                    text:''
                    font.family: iconFont.name
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            leftBox.open();
                        }
                    }
                }
                Text {
                    id: open_chat
                    width: parent.cellHeight
                    height: parent.cellHeight
                    font.pixelSize: parent.cellHeight
                    text:''
                    font.family: iconFont.name
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                              chatBox.open();
//                                        if(chatBox.implicitWidth==0) chatBox.implicitWidth=300;
//                                        else chatBox.implicitWidth=0;
                        }
                    }
                }

                Text {

                    width: parent.cellHeight
                    height: parent.cellHeight
//                            radius: 2
                    font.pixelSize: parent.cellHeight
                    text:''

//                            border.width:1
                    font.family: iconFont.name
                    MouseArea {
                        anchors.fill: parent
                        onClicked: function(){
                             // 标记

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
                            if(u!=="") pageView.text =tableOfContent.openPage(u);
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
                            pageView.text =tableOfContent.openPage(u);
                            root.onstartScrollTo = 0;
                        }
                    }
                }

                Text {
                    width: parent.cellHeight
                    height: parent.cellHeight
//                            radius: 2
                    font.pixelSize: parent.cellHeight
                    text:''

//                            border.width:1
                    font.family: iconFont.name
                    MouseArea {
                        anchors.fill: parent
                        onClicked:function() {

//                           // 下一页
//                            var u= tableOfContent.nextPage();
//                            if(u!=="")
//                            pageView.url = "mybook://book.local"+u;
//                            root.onstartScrollTo = 0;
                        }
                    }
                }
            }
        }
    }






    Popup{
        id:leftBox
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        width:300
        height:root.height-webview_btns.height
//        Rectangle {
//            anchors.fill: parent
//            color: "#fffffe"
//            radius: 10
//        }
        Rectangle {
                    anchors.fill: parent
                    color: "#fffffe"
                    radius: 10
        }
        Column{
            Rectangle{
                id:bookIconBox
                width:leftBox.width-20
                height:200 +20
                radius: 10
//                visible: leftBox.implicitWidth>=200?true:false


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
//                anchors.margins: 10
                width:leftBox.width
                height:leftBox.height - bookIconBox.height -20
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
//                            pageView.url = "mybook://book.local"+chapterUrl;
                            pageView.text = tableOfContent.openPage(chapterUrl);
                            console.log(chapterUrl);

                        }
                    }
                }
                highlight: Rectangle {
                    color: "#bae8e8"
                    width:leftBox.width-20
                }
                highlightMoveDuration:100
                focus: true
                clip:true

            }
        }
    }

    Popup{
//            color:"#fffffe"
            id:chatBox
            modal: true
            focus: true
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
            width:500
            height:root.height -webview_btns.height
            x: parent.width - width

            OpenChatModel {
                id:runingChatModel
            }
            Rectangle {
                        anchors.fill: parent
                        color: "#fffffe"
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


    Timer {
            id: selectionTimer
            interval: 800 // 1 second delay
            running: false
            repeat: false
            onTriggered: {
                userdata.read(root.bookName,tableOfContent.readIndex(),pageView.scrollPosition.y);
            }
    }

}
