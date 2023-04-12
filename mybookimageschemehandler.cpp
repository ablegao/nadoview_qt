#include "mybookimageschemehandler.h"

MyBookImageSchemeHandler::MyBookImageSchemeHandler(QObject *parent)
    : QQuickImageProvider(QQuickImageProvider::Pixmap) {}

QPixmap MyBookImageSchemeHandler::requestPixmap(const QString &id, QSize *size,
                                                const QSize &requestedSize) {

    //    QString href = request->requestUrl().url().mid(21);
    //    qDebug() << "REQUEST  : " << request->requestUrl().url() << href;
    QStringList arr = id.split("?");
    int w = book->screenWidth, h = book->screenHeight;
    QString u = id;
    if (arr.size() > 1) {
        QStringList size = arr.last().split("x");
        if (size.size() == 2) {
            w = size[0].toInt();
            h = size[1].toInt();
        }
        u = arr[0];
    }

    QByteArray data =
        book->mBook->openFileByUrl(book->mBook->absoluteFilePath(u));

    QPixmap img;
    img.loadFromData(data);
    //    qDebug() << img.width();

    if (img.width() > w - 40 && w > 0) {
        img = img.scaled(QSize(w - 40, h - 20), Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);
    }
    //    qDebug() << data.size() << img.width();

    return img;
    //    QBuffer *buffer = new QBuffer(request);

    //    buffer->setData(data);

    //    QMimeDatabase db;
    //    QMimeType mimeType = db.mimeTypeForData(data);
    //    request->reply(mimeType.name().toUtf8(), buffer);
}

MyBookImageSchemeHandler::~MyBookImageSchemeHandler() { book = nullptr; }
