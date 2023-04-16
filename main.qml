import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtNetwork
import NadoView 1.0
//import QtQuick.Effects
import Qt5Compat.GraphicalEffects

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
//    property  string bookUrl: ""
    property  int  pageIndex: 0;
    property int onstartScrollTo: 0
//    property var books : userdata.books()
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
        root.title = "Currently using NadoView 0.1 to read《" + obj.book_name + "》";
        root.bookName = obj.book_name;
        root.lang = obj.lang;
        var source = "image://mybook/"+obj.coverImg+"?180x240";
//        console.log("image......",obj.firstPageUrl,source);
        bookIcon.source = source;
        runingChatModel.addMessage("Reading "+obj.name,"system");

//        if(tableOfContent.useMarkDown()){
//            pageView.textFormat = Text.MarkdownText;
//        }
        tableOfContent.setSize(flick.width,flick.height);
        tocListView.currentIndex = 0;
        var read = userdata.openBook(obj.book_name);
        console.log("========= open book",JSON.stringify(read));
//        if(read.last_read_file!=="") pageView.url = "mybook://book.local"+tableOfContent.indexToUrl(read.last_read_index);
//        else pageView.url = "mybook://book.local"+obj.firstPageUrl;
        var url  = obj.firstPageUrl;
        if(read.last_read_file!=="")  url = tableOfContent.indexToUrl(read.last_read_index);

        var directoryPath = '';
        var pathArray = url.split('/');
        for (var i = 0; i < pathArray.length - 1; i++) {
          directoryPath += pathArray[i] + '/';
        }
        pageView.baseUrl = "image://mybook"+directoryPath;


        pageView.text = tableOfContent.openPage(url);
        onstartScrollTo = read.last_read_scroll_number;
        flick.contentY = onstartScrollTo;
    }

    property var pageRows:[];
    function bookChapterReaded(obj,index){
        root.pageIndex=index;
        userdata.read(root.bookName,index,0);

    }
    Component.onCompleted: function(){
        console.log("BOOK address:",root.bookUrl);
        var books = userdata.books(1);
//        book_list_grid.model = books;
        tableOfContent.onBookOpenFinishd.connect(bookOpenFinishd);
        tableOfContent.onOpenPageFinishd.connect(bookChapterReaded);
        if(bookUrl!=""){

            tableOfContent.openBook(bookUrl);
            userdata.addBook(bookPath);
        }
        else tableOfContent.openBook(books[0].book_url);

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
             height:parent.height - webview_btns.height
             contentWidth: parent.width
             contentHeight: pageView.contentHeight +30
             onWidthChanged: function(){
//                console.log(flick.width);
                 tableOfContent.setSize(flick.width,flick.height);
             }
//             property int maximumFlickableX: contentWidth - width // 计算滚动的最大值
//             onContentXChanged: contentX = Math.min(Math.max(0, contentX), maximumFlickableX) // 当滚动超过最大值时，将滚动位置限制在最大值内
             clip: true
            onMovementEnded:function(){
//                console.log("scroll move.",flick.contentY);
                userdata.read(root.bookName,root.pageIndex,flick.contentY);
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
//                    baseUrl: "image://mybook/"
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
                                var start = pageView.selectionStart
                                var end = pageView.selectionEnd
                                console.log(start,end);
                                var extraSelection = {
                                    "cursor": {"selectionStart": start, "selectionEnd": end},
                                    "format": {"background": "yellow"}
                                }
                                pageView.extraSelections.push(extraSelection)
                                pageView.extraSelections = pageView.extraSelections;
//                                pageView.runJavaScript(`
//                                                var selectedText = window.getSelection();
//                                                var range = selectedText.getRangeAt(0);
//                                                var span = document.createElement('span');
//                                                span.style.backgroundColor ="`+modelData+`";
//                                                range.surroundContents(span);
//                                            `);
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

                Text {
                    width: parent.cellHeight
                    height: parent.cellHeight
//                            radius: 2
                    font.pixelSize: parent.cellHeight
                    text:''

//                            border.width:1
                    font.family: iconFont.name
                    MouseArea {
                        anchors.fill: parent
                        onClicked:function() {
                            bookLists.open();
                            sync_open_load.start();
                        }
                    }
                }
            }
        }
    }


    Popup {
        id: bookLists
        modal: true
        focus: true

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        width:root.width
        height:root.height - webview_btns.height
        Rectangle {
                    anchors.fill: parent
                    color: "#fffffe"
                    radius: 10
        }
        Text {
            id:bookLists_Loading
            text:"loading...."
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent

        }
        onClosed: function(){
            bookLists_Loading.visible=false;
        }
        onOpened: function(){
            bookLists_Loading.visible=true;
        }
        Loader {
                id: bookListLoader
                anchors.fill: parent
                active: false
                asynchronous: true
                visible: status == Loader.Ready
                sourceComponent: bookListComponent
                onLoaded: function(){
                    bookLists_Loading.visible=false;
                }
        }
//        onOpened:  {
//                sync_open_load.start();
////                bookListLoader.sourceComponent = bookListComponent
////                bookLists.contentItem = bookListLoader.item // 设置Popup内容为加载的GridView
////            bookListLoader.setSource(bookListComponent)

////            console.log("=----------");
//        }
        Timer {
            id:sync_open_load
            interval: 10
            repeat: false
            running: false
            onTriggered: function(){
                bookListLoader.active = false  // 关闭Loader
                bookListLoader.active = true   // 重新加载,获取最新数据模型
            }
        }

        Component {
            id:bookListComponent

            GridView {
               id:book_list_grid
               anchors.fill: parent
               cellWidth: 124
               cellHeight: 220
               clip:true


               model:userdata.books()

               delegate: Rectangle {
                    width:book_list_grid.cellWidth
                    height:book_list_grid.cellHeight
                    anchors.margins: 4
                    color:"#fffffe"
    //                DropShadow {
    //                   anchors.fill: parent
    //                   radius: 8.0
    //                   samples: 17
    //                   color: "#80000000"
    //                   horizontalOffset: 3
    //                   verticalOffset: 3
    //                   spread: 0
    //               }
                   Column{
                        Image {
                            id:book_icon
                            width: book_list_grid.cellWidth-4
                            height: 180
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
    //                    console.log(modelData.book_name);
                        tableOfContent.openBook(modelData.book_url);
                        bookLists.close();
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
                        height: parent.cellHeight
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
