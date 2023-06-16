import QtQuick 2.15
import QtQuick.Controls
//import QtNetwork
import com.tap2happy.nadoview 1.0
//import QtQuick.Effects
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import Qt.labs.platform


import QtWebView 1.1
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
    property string opf:"";
    property string uuid: "";
    property int onstartScrollTo: 0
    property int pos_y:0
    property  bool loaded:false;
    visible: true
	
    width: 1060
    height: 900
    UserData{
        id:userdata
    }
    TableOfContent {
        id:tableOfContent
        onSelectedText: function(text){
            transfer_box_text.text=text;
//            transferBox.open();
//            myTransfer.transferText(text);
        }
    }
//    AwsTransfer {
//        id:myTransfer
//        onResultReady: function(out){
//			//            console.log("transfer out:",out);
//			// if(out.length<=4 || out.split(" ").length ==1){
//			//
//            // 	transfer_box_text.text = "<a href='eudic://dict/"+out+"'>" +out+"</a>";
//			// 	return;
//			// }
//            transfer_box_text.text = out;
//        }
//    }


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

        runingChatModel.addMessage("Use the content in \""+obj.book_name+"\" to answer my question.","system");
//        if(tableOfContent.useMarkDown()){
//            pageView.textFormat = Text.MarkdownText;
//        }
        tableOfContent.setSize(pageView.width,pageView.height);
        tocListView.currentIndex = 0;
        var read = userdata.openBook(obj.bookPath,obj.book_name,obj.auther,obj.lang);
		root.bookUrl = obj.bookPath;
        root.opf = obj.opf;
        root.uuid = obj.uuid;
        bookIcon.source ="file://"+obj.bookPath+"/"+obj.uuid+".jpg" ;
//        if(read.last_read_file!=="") pageView.url = "mybook://book.local"+tableOfContent.indexToUrl(read.last_read_index);
//        else pageView.url = "mybook://book.local"+obj.firstPageUrl;
//        var url  = obj.firstPageUrl;
        root.pageIndex = read.last_read_index;
        pageView.url="qrc:///res/index.html";

//        if(read.last_read_file!=="")  url = tableOfContent.indexToUrl(read.last_read_index);
//        console.log(url);
//        pageView.runJavaScript(`rendition.display("`+url+`");`);
//        var directoryPath = '';
//        var pathArray = url.split('/');
//        for (var i = 0; i < pathArray.length - 1; i++) {
//          directoryPath += pathArray[i] + '/';
//        }
//        pageView.baseUrl = "image://mybook"+directoryPath;


//        pageView.text = tableOfContent.openPage(url);
//        pageView.url = tableOfContent.hosts()+ "";
//        console.log(pageView.url);
		if(read.last_read_scroll_number) onstartScrollTo = read.last_read_scroll_number;
//        if(root.loaded){
//            root.open_book();
//        }
		book_lists.close();
//        flick.contentY = onstartScrollTo;
    }



    property var pageRows:[];
    function bookChapterReaded(obj,index){
        root.pageIndex=index;
//        userdata.read(root.bookUrl,root.bookName,index,0);

    }
    function onFileOpen(url){
        tableOfContent.openBook(url);

    }
    //没有打开的文件时
    function noOpenFile(){
        var books = userdata.books(1);
        console.log(JSON.stringify(books));
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
        console.log("loaded...");

	}
		Rectangle{
    	    id:leftBox
    	    //modal: true
    	    //focus: true
    	    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
			width:300
            y:pos_y
			color:"#fffffe"
            height:root.height-pos_y
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
//                                pageView.url = tableOfContent.hosts()+chapterUrl;
                                pageView.runJavaScript(`rendition.display("`+chapterUrl+`");`);
                                userdata.read(root.bookUrl,root.bookName,index,0);
//                                console.log(pageView.url);

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
            y:pos_y
            height:parent.height-pos_y
            WebView {
    	        id:pageView
//  	                  width: flick.width-20
//  	                  x:10
//                url:"qrc:///res/index.html"
    	        width: parent.width
    	        height: parent.height -webview_btns.height
    	        onLoadingChanged:function(event){
                    if(event.status!==2){
                        return;
                    }
//                    console.log(pageView.url+" opened...");
//                    root.open_book();
                    pageView.runJavaScript("book.open('"+tableOfContent.hosts()+"/"+root.uuid+"/"+root.opf+"');",function(result){

                    });
                    var url = tableOfContent.indexToUrl(root.pageIndex);

                    pageView.runJavaScript(`rendition.display("`+url+`");`);
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
//    	                       var u = tableOfContent.prevPage();
                                pageView.runJavaScript("rendition.prev();rendition.currentLocation()",function(obj){
                                    tocListView.currentIndex = obj["start"]['index'];
                                    userdata.read(root.bookUrl,root.bookName,tocListView.currentIndex,0);
                                });
//    	                        if(u!=="") pageView.url =tableOfContent.hosts()+u;
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
                                pageView.runJavaScript("rendition.next();rendition.currentLocation()",function(obj){
                                    tocListView.currentIndex = obj["start"]['index'];
                                    userdata.read(root.bookUrl,root.bookName,tocListView.currentIndex,0);
                                });

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

                    // settings.
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
                                setting_config.open();
//
    	                    }
    	                }
					}

  	              Text {
  	                  width: parent.cellHeight
  	                  height: parent.cellHeight
//	                            radius: 2
  	                  font.pixelSize: parent.cellHeight
  	                  text:''

//	                            border.width:1
  	                  font.family: iconFont.name
  	                  MouseArea {
  	                      anchors.fill: parent
  	                      onClicked:function() {
							  book_lists.open();
  	                      }
  	                  }
  	              }
    	        }
    	    }
		}

    Popup {
        id: setting_config
        modal:true
        focus:true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        width:parent.width
        height:parent.height - webview_btns.height-pos_y
        y:pos_y

        // 设置界面的主要内容
        Item {
            anchors.fill: parent


            Column {
                spacing: 10

                // OPNEAI API KEY
                Row {
                    spacing: 10

                    Text {
                        text: "OPNEAI API KEY:"
                        width: 150
                    }

                    TextField {
                        id: apiKeyField
                        width:400
                        placeholderText: "Enter API Key"
                    }
                }

                // Font Size
                Row {
                    spacing: 10

                    Text {
                        text: "Font Size:"
                        width: 150
                    }

                    SpinBox {
                        id: fontSizeSpinBox
                        from: 1
                        to: 100
                        value: 16
                    }
                }

                // Book Dir
                Row {
                    spacing: 10

                    Text {
                        text: "Book Dir:"
                        width: 150
                    }

                    ListView{
                        id:bookDir
                        width:400
                        height:100
                        model:ListModel{
                        }
                        delegate: Text {
                                text: dir_path
                        }
                    }
                }

                Row {
                    spacing: 10

                    Text {
                        text: "Book Dir:"
                        width: 150
                    }
                    Button{
                        text:"Select Book Folder"
                        onClicked: function(){
                            fileDialog.open();
                        }
                    }

                    FolderDialog {
                        id: fileDialog
                        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                        onAccepted:function(){
                            var path= fileDialog.currentFolder.toString();
                            if(path.substring(0,7)==="file://"){
                                path = path.substring(7);
                            }
                            for(var i=0;i<bookDir.model.count;i++){
                                var old = bookDir.model.get(i);
                                if(old['dir_path']=== path){
                                    console.log("path exists: "+ path);
                                    return;
                                }
                            }
                            bookDir.model.append({"dir_path":path});
                        }
                    }
                }

                Row{
                    spacing: 10
                    Button {
                        text: "Save"
                        onClicked: {
                            var apiKey = apiKeyField.text;
                            var fontSize = fontSizeSpinBox.value;
                            // 在这里保存设置值
                            console.log("API Key:", apiKey);
                            console.log("Font Size:", fontSize);
                            console.log("Book Dir:", bookDir);
                        }
                    }
                }
            }
        }

        // 底部按钮

        onOpened: function(){
            var config = userdata.getSettings();
            apiKeyField.text = config.openai_api_key;
            fontSizeSpinBox.value = config.font_size;
            for(var item in config.book_dirs){
                bookDir.model.append({"dir_path":config.book_dirs[item]});
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
            TextEdit {
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

	Popup{
		id:book_lists
		width:root.width
        height:root.height-pos_y
        y:pos_y

        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
		Rectangle {
            anchors.fill: parent
            color: "#fffffe"
		}
		onOpened:function(){
            books_loader.setSource("qrc:/nadoview/books.qml",{userdata:userdata,appSingle:appSingle});
            console.log("on opened....");
		}
		Loader{
			id:books_loader
			anchors.fill:parent
			// source:"books.qml"
		    asynchronous: true
				
		}
        Shortcut {
            sequence: "Esc"
            onActivated: {
                console.log("Esc: cancel a few things")
                book_lists.close()
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
