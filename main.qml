import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtNetwork
import com.tap2happy.nadoview 1.0
//import QtQuick.Effects
import Qt5Compat.GraphicalEffects

import QtWebView 1.1
//import QtWebEngine 1.10
import QtWebChannel 1.5

Window {
    id: root
    title: "Nado View 0.1 Preview"
    property string lang: "cn"
    property int screenWidthThreshold: 1600
    property int itemsPerPage: width >= screenWidthThreshold ? 2 : 1
    property string pageHref:"";
    property string pageDesc:"";
    property  string bookName: ""
    property  string bookUrl: ""
    property  int  pageIndex: 0;
    property int onstartScrollTo: 0
    visible: true
	
    width: 1060
    height: 900
    UserData{
        id:userdata
    }
    TableOfContent {
        id:tableOfContent
        onSelectedText: function(text){
            transfer_box_text.text="loading...";
            transferBox.open();
            myTransfer.transferText(text);
        }
    }
    AwsTransfer {
        id:myTransfer
        onResultReady: function(out){
			//            console.log("transfer out:",out);
			if(out.split(" ").length ==1 || out.length==1){
				
            	transfer_box_text.text = "<a href='eudic://dict/"+out+"'>" +out+"</a>";
				return;
			}
            transfer_box_text.text = out;
        }
    }


    FontLoader{
        id:iconFont
        source:"qrc:/fonts/fontawesome-webfont.ttf"
    }

    //:/

    function bookOpenFinishd(obj){
        console.log(JSON.stringify(obj));
        root.title = "Currently using NadoView 0.1 to read《" + obj.book_name + "》";
        root.bookName = obj.book_name;
        root.lang = obj.lang;
//        var source = "image://mybook/"++"?180x240";
//        console.log("image......",obj.firstPageUrl,source);
        bookIcon.source = tableOfContent.hosts()+obj.coverImg;
        runingChatModel.addMessage("Use the content in \""+obj.book_name+"\" to answer my question.","system");
//        if(tableOfContent.useMarkDown()){
//            pageView.textFormat = Text.MarkdownText;
//        }
        tableOfContent.setSize(pageView.width,pageView.height);
        tocListView.currentIndex = 0;
        var read = userdata.openBook(obj.book_name);

//        if(read.last_read_file!=="") pageView.url = "mybook://book.local"+tableOfContent.indexToUrl(read.last_read_index);
//        else pageView.url = "mybook://book.local"+obj.firstPageUrl;
        var url  = obj.firstPageUrl;
        if(read.last_read_file!=="")  url = tableOfContent.indexToUrl(read.last_read_index);

        var directoryPath = '';
        var pathArray = url.split('/');
        for (var i = 0; i < pathArray.length - 1; i++) {
          directoryPath += pathArray[i] + '/';
        }
//        pageView.baseUrl = "image://mybook"+directoryPath;


//        pageView.text = tableOfContent.openPage(url);
        pageView.url = tableOfContent.hosts()+ url;
        if(read.last_read_scroll_number) onstartScrollTo = read.last_read_scroll_number;
//        flick.contentY = onstartScrollTo;
    }

    property var pageRows:[];
    function bookChapterReaded(obj,index){
        root.pageIndex=index;
        userdata.read(root.bookName,index,0);

    }
    function onFileOpen(url){
        tableOfContent.openBook(url);

    }
    //没有打开的文件时
    function noOpenFile(){
        var books = userdata.books(1);
        tableOfContent.openBook(books[0].book_url);
    }
    Component.onCompleted: function(){
        console.log("BOOK address:",root.bookUrl);
//        var books = userdata.books(1);
//        book_list_grid.model = books;
        tableOfContent.onBookOpenFinishd.connect(bookOpenFinishd);
        tableOfContent.onOpenPageFinishd.connect(bookChapterReaded);
        appSingle.onFileOpend.connect(onFileOpen);
        appSingle.onNoOpenFile.connect(noOpenFile);

	}
		Rectangle{
    	    id:leftBox
    	    //modal: true
    	    //focus: true
    	    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
			width:300
			color:"#fffffe"
    	    height:root.height
//  	      Rectangle {
//  	          anchors.fill: parent
//  	          color: "#fffffe"
//  	          radius: 10
//  	      }
	  	    function open(){
				if(width==300){
					width=0;
					root.width = root.width-300;	
					root.x = root.x+300;
				}else{
					width = 300;
					root.width = root.width+300;
					root.x = root.x-300;
				}
	  	    }
    	    Column{
    	        Rectangle{
    	            id:bookIconBox
    	            width:leftBox.width-20
    	            height:200 +20
    	            radius: 10
    	            Image{
    	                id:bookIcon
//  	                  source:""

    	                anchors.margins: 10
    	                anchors.fill: parent
//  	                  width:parent.width-40
//  	                  height:parent.height-40
    	                fillMode: Image.PreserveAspectFit
//  	                  anchors.centerIn: parent

    	            }
    	        }
    	        ListView {
    	            id: tocListView
//  	              anchors.margins: 10
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
    	                        pageView.url = tableOfContent.hosts()+chapterUrl;
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


    	Rectangle{
    	    id:centerBox
    	    color:"#fffffe"
			width:parent.width-leftBox.width
			x:leftBox.width
    	    height:parent.height
    	    WebView {
    	        id:pageView
//  	                  width: flick.width-20
//  	                  x:10
    	        width: parent.width
    	        height: parent.height -webview_btns.height
    	        onLoadingChanged:function(event){
    	            if(event.status!=2){
    	                return;
    	            }
    	            pageView.runJavaScript(` var transfer_url = "`+tableOfContent.hosts()+`/transfer";
    	                                   `);

    	            pageView.runJavaScript("document.body.style.margin='20px';");
//  	              pageView.runJavaScript("document.body.style.textIndent='2em';");
//  	              pageView.runJavaScript("document.body.style.scrollX='hidden';");
    	            pageView.runJavaScript("document.body.style.overflowX='hidden';");
//  	              pageView.runJavaScript("document.body.style.overflowY='hidden';");
    	            pageView.runJavaScript(`
    	                                   var images = document.querySelectorAll('img');
    	                                   Array.prototype.forEach.call(images, function(image) {
    	                                     image.style.maxWidth = '90%';
    	                                   });
    	                                   `);
    	            pageView.runJavaScript(`
    	                                   var ps = document.querySelectorAll('p');
    	                                   Array.prototype.forEach.call(ps, function(p) {
    	                                     p.style.textIndent = '2em';
    	                                   });
    	                                   `);

    	            pageView.runJavaScript(`document.addEventListener('mouseup', function() {
    	                                   var selection = window.getSelection().toString();
    	                                   if (selection !== '') {
    	                                   var xhr = new XMLHttpRequest();
    	                                   xhr.open('POST', transfer_url);
    	                                   xhr.send(selection);
    	                                   }
    	                                 });`,function(result){
    	                    console.log(result);
    	            });
    	            console.log(tableOfContent.urlToIndex(url));
    	            console.log(root.bookName, tableOfContent.readIndex());
    	            userdata.read(root.bookName,tableOfContent.readIndex(),0);
    	        }

    	    }


    	    Rectangle{
    	        color:"#bae8e8"
    	        id:webview_btns
				width:centerBox.width
    	        anchors.top: pageView.bottom
    	        height:35
//  	                  border.width: 1

    	        Row {
    	            id:tool_row
    	            anchors.centerIn: parent
    	            spacing: 5
    	            property int  cellHeight: 26

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
//  	                                      if(chatBox.implicitWidth==0) chatBox.implicitWidth=300;
//  	                                      else chatBox.implicitWidth=0;
    	                    }
    	                }
    	            }

    	            Text {

    	                width: parent.cellHeight
    	                height: parent.cellHeight
//  	                          radius: 2
    	                font.pixelSize: parent.cellHeight
    	                text:''

//  	                          border.width:1
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
//  	                          radius: 2
    	                font.pixelSize: parent.cellHeight
    	                text:''

//  	                          border.width:1
    	                font.family: iconFont.name
    	                MouseArea {
    	                    anchors.fill: parent
    	                    onClicked: function(){
    	                         // 上一页
    	                       var u = tableOfContent.prevPage();
    	                        if(u!=="") pageView.url =tableOfContent.hosts()+u;
    	                    }
    	                }
    	            }

    	            Text {
    	                width: parent.cellHeight
    	                height: parent.cellHeight
//  	                          radius: 2
    	                font.pixelSize: parent.cellHeight
    	                text:''

//  	                          border.width:1
    	                font.family: iconFont.name
    	                MouseArea {
    	                    anchors.fill: parent
    	                    onClicked:function() {

    	                       // 下一页
    	                        var u= tableOfContent.nextPage();
    	                        if(u!=="")
    	                        pageView.url =tableOfContent.hosts()+u;
//  	                          root.onstartScrollTo = 0;
    	                    }
    	                }
    	            }

    	            Text {
    	                id:share_text
    	                width: parent.cellHeight
    	                height: parent.cellHeight
    	                font.pixelSize: parent.cellHeight
    	                text:''
    	                font.family: iconFont.name
    	                MouseArea {
    	                    anchors.fill: parent
    	                    onClicked:function() {
    	                        console.log(pageView.selectedText);
//  	                          tableOfContent.shareToImage(pageView.selectedText,root.bookName,16,"/Users/ablegao/code/andctrol/share.jpg");
    	                        painterBox_text.text = pageView.selectedText;

    	                        painterBox.open();
    	                    }
    	                }
    	            }


    	            Text {
    	                width: parent.cellHeight
    	                height: parent.cellHeight
//  	                          radius: 2
    	                font.pixelSize: parent.cellHeight
    	                text:''

//  	                          border.width:1
    	                font.family: iconFont.name
    	                MouseArea {
    	                    anchors.fill: parent
    	                    onClicked:function() {

//  	                         // 下一页
//  	                          var u= tableOfContent.nextPage();
//  	                          if(u!=="")
//  	                          pageView.url = "mybook://book.local"+u;
//  	                          root.onstartScrollTo = 0;
    	                    }
    	                }
    	            }

//  	              Text {
//  	                  width: parent.cellHeight
//  	                  height: parent.cellHeight
////	                            radius: 2
//  	                  font.pixelSize: parent.cellHeight
//  	                  text:''

////	                            border.width:1
//  	                  font.family: iconFont.name
//  	                  MouseArea {
//  	                      anchors.fill: parent
//  	                      onClicked:function() {
//  	                          screen_win.setSource("qrc:/nadoview/books.qml",{});
//  	                      }
//  	                  }
//  	              }
    	        }
    	    }
		}
    Popup {
        id: painterBox
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        width:220
//        QQuickPaintedItem {
            Text{
                id:painterBox_text
            }
//        }
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
                        font.family: iconFont.name
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                chat_input_text.text+= pageView.selectedText;
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

    Popup {
        id:transferBox
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        width:root.width
        height:transferBoxColumn.height+20
        y:root.height - height - webview_btns.height
        Rectangle {
                    anchors.fill: parent
                    color: "#fffffe"
        }
        Column{
            id:transferBoxColumn
            width:parent.width
//            anchors.fill: parent
            Row {
                height:26
                spacing: 5
                property int  cellHeight: 26
                Repeater {
                    model: ["#FFA07A", "#98FB98", "#87CEFA", "#FFFACD","#FFFFFF"]
                    delegate: Rectangle {
                        width: parent.cellHeight
                        height: parent.cellHeight
                        color:  modelData
                        radius: 2
                        border.width:1
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                            }
                        }
                    }
                }
            }
            Text {
                id:transfer_box_text
				text:""
				textFormat: Text.RichText
                wrapMode:Text.Wrap
                width:parent.width-40
				anchors.margins: 20
				onLinkActivated:(link)=>Qt.openUrlExternally(link)
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
