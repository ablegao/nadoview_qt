import QtQuick 2.15
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Controls
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

            TextArea {
                id: bookDirTextArea
                placeholderText: "Enter book directory"
                width: 200
                height: 100
            }
        }
    }

// 底部按钮
Row {

    spacing: 10
    padding: 10

    Button {
        text: "Save"
        onClicked: {
            var apiKey = apiKeyField.text;
            var fontSize = fontSizeSpinBox.value;
            var bookDir = bookDirTextArea.text;
            // 在这里保存设置值
            console.log("API Key:", apiKey);
            console.log("Font Size:", fontSize);
            console.log("Book Dir:", bookDir);
        }
    }
}


}
