#include "customurlschemehandler.h"
#include "QtWebEngineCore/qwebengineurlrequestjob.h"

CustomUrlSchemeHandler::CustomUrlSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler{parent} {}
void CustomUrlSchemeHandler::requestStarted(QWebEngineUrlRequestJob *request) {
    // 处理自定义URL模式的请求
    //    QUrl url = request->requestUrl();
    QString prefix = "mybook://book.local/";
    qDebug() << "Custom scheme request:"
             << request->requestUrl().toString().mid(prefix.length());
    if (book == nullptr) {
        return;
    }
    QByteArray body = book->mBook->openFileByUrl(
        request->requestUrl().toString().mid(prefix.length()));
    QMimeDatabase mimeDatabase;
    QMimeType mimeType = mimeDatabase.mimeTypeForData(body);
    QBuffer *readBody = new QBuffer(request);
    readBody->setData(body);
    readBody->open(QIODevice::ReadOnly);
    //    QBuffer readBody(&body);
    request->reply(mimeType.name().toUtf8(), readBody);
}
