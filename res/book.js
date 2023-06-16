// body 样式
document.body.style.margin = '20px';
document.body.style.fontSize = '18px';
document.body.style.overflowX = 'hidden';
var images = document.querySelectorAll('img');
Array.prototype.forEach.call(images, function (image) {
    image.style.maxWidth = '90%';
});
var ps = document.querySelectorAll('p');
Array.prototype.forEach.call(ps, function (p) {
    p.style.textIndent = '2em';
});


// 添加css 和js 

function create_link(f) {
    var link = document.createElement('link');
    link.ref = 'stylesheet';
    link.href = f;
    link.type = "text/css";
    link.media = "screen,print";
    document.getElementsByTagName('head')[0].appendChild(link);
}

$.rloader([
            {src:"qrc:/res/jquery-ui.min.css"},
            {src:"qrc:/res/jquery-ui.structure.min.css"},
            {src:"qrc:/res/jquery-ui.theme.min.css"}
          ]);


var div = document.getElementById("dialog");
if (div == undefined) {
    div = document.createElement("div");
    div.id = "dialog";
    document.getElementsByTagName('head')[0].appendChild(div);
}
////div.style.display = "none";
//document.addEventListener('mouseup', function (event) {
//    var selection = window.getSelection().toString();
//    if (selection !== '') {

////        $("body").html(selection);
//        $("#dialog").html(selection);
//         $("#dialog").dialog();

//        // showFloatingBox(100, event.y, selection);
//    }
//});

