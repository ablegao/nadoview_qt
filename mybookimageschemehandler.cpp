#include "mybookimageschemehandler.h"

MyBookImageSchemeHandler::MyBookImageSchemeHandler(QObject *parent)
    : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

QPixmap MyBookImageSchemeHandler::requestPixmap(const QString &id, QSize *size,
                                                const QSize &requestedSize) {

    //    QString href = request->requestUrl().url().mid(21);
    //    qDebug() << "REQUEST  : " << request->requestUrl().url() << href;
    QStringList arr = id.split("?");
    int w = 0, h = 0;

    if (arr.size() > 1) {
        QStringList size = arr.last().split("x");
        if (size.size() == 2) {
            w = size[0].toInt();
            h = size[1].toInt();
        }
    }
    QByteArray data = book->mBook->openFileByUrl(arr[0]);
    QPixmap img;
    img.loadFromData(data);

    if (img.width() > w - 40 && w > 0) {
        img = img.scaled(QSize(w - 40, h - 20), Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);
    }

    return img;
    //    QBuffer *buffer = new QBuffer(request);

    //    buffer->setData(data);

    //    QMimeDatabase db;
    //    QMimeType mimeType = db.mimeTypeForData(data);
    //    request->reply(mimeType.name().toUtf8(), buffer);
}

MyBookImageSchemeHandler::~MyBookImageSchemeHandler() { book = nullptr; }
