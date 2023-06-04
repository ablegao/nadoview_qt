#include "tableofcontent.h"

#include "QtCore/qbytearray.h"
#include "QtCore/qiodevice.h"
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsonobject.h"
#include "QtCore/qtmetamacros.h"

// // 这个写法，是为了初始化变量mData , C++ 可以在构造函数定义中初始化变量
// 不是在构造函数function 内部
// TableOfContent::TableOfContent(QObject *parent)
//     : QAbstractListModel(parent), mData({
//                                       QVariantMap{{"chapterName",
//                                       "Chapter01"}},
//                                       QVariantMap{{"chapterName",
//                                       "Chapter02"}},
//                                       QVariantMap{{"chapterName",
//                                       "Chapter03"}},
//                                   }) {}

TableOfContent::TableOfContent(QObject *parent) : QAbstractListModel(parent) {
    server = new QHttpServer();
    server->listen(QHostAddress::LocalHost);

    server->route("/transfer",
                  std::function<QByteArray(const QHttpServerRequest &)>(
                      [this](const QHttpServerRequest &request) {
                          return transferRequest(request);
                      }));
    server->route("/(.*)",
                  std::function<QByteArray(const QHttpServerRequest &)>(
                      [this](const QHttpServerRequest &request) {
                          return handleRequest(request);
                      }));

    qDebug() << "Server :" << hosts();
}
TableOfContent::~TableOfContent() { mBook = nullptr; }

QString TableOfContent::hosts() {
    return QString("http://localhost:%1").arg(server->serverPorts()[0]);
}

QByteArray TableOfContent::handleRequest(const QHttpServerRequest &request) {
    if (mBook != nullptr) {
        QString URI = request.url().path();
        qDebug() << "========== " << URI;
        QByteArray result = mBook->openFileByUrl(request.url().path());
        //        if (URI.endsWith("htm") || URI.endsWith("html")) {
        //            QString end_js
        //                = "<script type='text/javascript'
        //                src='https://code.jquery.com/jquery-3.7.0.min.js'></script>";
        //            result += end_js.toUtf8();
        //        }
        return result;
    }

    return "NONE";
}

QByteArray TableOfContent::transferRequest(const QHttpServerRequest &req) {
    QString text = req.body();
    emit selectedText(text);
    //    Aws::SDKOptions options;
    //    Aws::InitAPI(options);
    //    {
    //        Aws::Translate::TranslateClient client;
    //        Aws::Translate::Model::TranslateTextRequest request;
    //        request =
    //        request.WithSourceLanguageCode("auto").WithTargetLanguageCode("zh");
    //        request.SetText(text.toStdString().c_str());
    //        auto outcome = client.TranslateText(request);
    //        if (outcome.IsSuccess()) {
    //            auto outtext = outcome.GetResult().GetTranslatedText();
    //            //            emit resultReady(outtext.c_str());
    //            return outtext.c_str();
    //        } else {
    //            qDebug() << "ERRRRRRR";
    //            //            emit resultReady("Transfer Error");
    //        }
    //    }
    //    Aws::ShutdownAPI(options);
    return "";
}
QVariant TableOfContent::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    if (mBook == nullptr) return QVariant();

    if (role == ChapterID) {
        return mBook->tableOfContent.at(index.row())
            .toMap()
            .value("chapterID");  // 这里的chapterID 是mData中的key
    } else if (role == ChapterName) {
        return mBook->tableOfContent.at(index.row())
            .toMap()
            .value("chapterName");
    } else if (role == ChapterUrl) {
        return mBook->tableOfContent.at(index.row())
            .toMap()
            .value("chapterUrl");
    } else if (role == ChapterLevel) {
        return mBook->tableOfContent.at(index.row())
            .toMap()
            .value("chapterLevel");
    }

    return QVariant();
}

int TableOfContent::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    if (mBook == nullptr) {
        return 0;
    }
    return mBook->tableOfContent.size();
}

QHash<int, QByteArray> TableOfContent::roleNames() const {
    // 这里 chapterName 是QML 里面的调用名称
    QHash<int, QByteArray> roles;
    roles[ChapterName] = "chapterName";
    roles[ChapterID] = "chapterID";
    roles[ChapterUrl] = "chapterUrl";
    roles[ChapterLevel] = "chapterLevel";
    return roles;
}

// Q_INVOKABLE void TableOfContent::openChapter(int index) {
////    emit bookChapterReaded(mBook->openChapter(mBook->indexToUrl(index)));
//}

// Q_INVOKABLE void TableOfContent::openPageByUrl(const QString &url) {
////    emit bookChapterReaded(mBook->openChapter(url));
//}
// Q_INVOKABLE void TableOfContent::openChapterByUrl(const QString &url) {
//    //    emit bookChapterReaded(mBook->openChapter(index));
//}
Q_INVOKABLE void TableOfContent::openBook(const QString &bookUri) {
    mBook = new EpubBook();
    int status = mBook->parseBook(bookUri);
    if (status == 0) {
        qDebug() << "file open finished";
        emit layoutChanged();
        QVariantMap info = {
            {"book_name", mBook->title},
            {"bookPath", mBook->bookPath},
            {"auther", mBook->getCreators().join(",")},
            {"lang", mBook->getLanguage()},
            {"coverImg", mBook->coverImg},
            {"firstPageUrl", mBook->getFirstPageUrl()},
            {"lang", mBook->language},
            {"coverImg", mBook->coverImg},
            {"firstPageUrl", mBook->getFirstPageUrl()},
        };
        emit bookOpenFinishd(info);
    } else {
        qDebug() << "open file error " << status << bookUri;
    }

    //    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler("mybook",
    //                                                                 mBook);
}
Q_INVOKABLE QByteArray TableOfContent::openPage(const QString &url) {
    QRegularExpression re("<p(?![^>]*\\bstyle=)[^>]*>");

    QString html = mBook->openFileByUrl(url);
    if (!useMarkDown()) {
        html.replace(re, "<p style='text-indent: 32px;'>");
    }
    QString output = html;
    QRegularExpression reg(
        "<image .* xlink:href=\"(.*?)\" .* ?(/>|>.*?</image>)");
    QRegularExpressionMatchIterator it = reg.globalMatch(html);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        //        QString attributes = match.captured(1) + match.captured(3);
        QString src = match.captured(1);
        QString replacement = "<img src=\"" + src + "\">";
        output.replace(match.capturedStart(), match.capturedLength(),
                       replacement);
    }
    emit openPageFinishd(url, mBook->readIndex());
    //    qDebug() << "" << output;
    return output.toUtf8();
}

Q_INVOKABLE QString TableOfContent::nextPage() {
    if (mBook != nullptr)
        return mBook->nextPage();
    else
        return "";
}
Q_INVOKABLE QString TableOfContent::prevPage() {
    if (mBook != nullptr)
        return mBook->prevPage();
    else
        return "";
}
Q_INVOKABLE QString TableOfContent::markdownToHtml(const QString &markdown,
                                                   int fontSize, int w, int h) {
    QString blockHtml = markdown;

    if (mBook == nullptr) {
        return markdown;
    }
    //    qDebug() << "book language" << mBook->language
    //             << mBook->language.contains("zh") << w << h;

    blockHtml = blockHtml.replace(
        imageRegex, "<img src=\"\\2" + QString("?%1x%2").arg(w).arg(h) +
                        "\" alt=\"\\1\"/>");
    blockHtml = blockHtml.replace(linkRegex, "<a href =\"\\2\">\\1<a/>");
    blockHtml = blockHtml.replace(emStrongRegex, "<b><i>\\2</i></b>");
    blockHtml = blockHtml.replace(strongRegex, "<b>\\2</b>");
    blockHtml = blockHtml.replace(emRegex, "<i>\\2</i>");
    if (blockHtml.mid(0, 4) == "####") {
        blockHtml = "<h3>" + blockHtml.mid(4) + "</h3>";
    } else if (blockHtml.mid(0, 3) == "###") {
        blockHtml = "<h3>" + blockHtml.mid(3) + "</h3>";
    } else if (blockHtml.mid(0, 2) == "##") {
        blockHtml = "<h2>" + blockHtml.mid(2) + "</h2>";
    } else if (blockHtml.mid(0, 1) == "#") {
        blockHtml = "<h1>" + blockHtml.mid(1) + "</h1>";
    } else {
        if (useMarkDown()) {
            blockHtml = "<p>" + blockHtml + "</p>";
        } else {
            blockHtml =
                "<p style='text-indent: " + QString("%1px").arg(fontSize * 2) +
                ";'>" + blockHtml + "</p>";
        }
    }
    //        qDebug() << blockHtml;
    return blockHtml;
}

bool TableOfContent::useMarkDown() {
    if (mBook == nullptr) return false;
    if (mBook->language.contains("zh")) return false;
    return true;
}

QString TableOfContent::indexToUrl(int index) {
    return mBook->indexToUrl(index);
}
int TableOfContent::urlToIndex(const QString &url) {
    return mBook->urlToIndex(url);
}
int TableOfContent::readIndex() { return mBook->readIndex(); }
// int TableOfContent::getMenuIndex() { return mBook->readIndex(); }
int TableOfContent::getIndexOfTable() { return mBook->getIndexOfTable(); }
QString TableOfContent::absoluteFilePath(const QString &u) {
    return mBook->absoluteFilePath(u);
}

void TableOfContent::setSize(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    //    currentPageUrl = pageUrl;
}

// void TableOfContent::requestStarted(QWebEngineUrlRequestJob *request) {
//     qDebug() << "Request started for URL:" << request;
// }

// QWebEngineUrlSchemeHandler *TableOfContent::urlSchemeHandler() {}
void TableOfContent::shareToImage(const QString &text, const QString &bookName,
                                  const QString &outdir, int fontSize) {
    int fontCount = text.length();
    int width = 220;
    int height = fontCount * fontSize / width * fontSize;

    QImage image(width, height, QImage::Format_RGB32);
    QPainter painter(&image);
    //    painter.begin(this);
    //    painter.setFont(QFont("Times", 30, QFont::Bold));
    //    painter.begin(this);
    QRect textRect(10, 10, width - 20, height + 100);
    painter.drawText(textRect, text);
    painter.end();
    image.save(outdir);
}

QString TableOfContent::readfile(const QString &src) {
    QFile file(src);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString(file.readAll());
    } else {
        qDebug() << "open error:" << src << file.errorString();
    }
    return "//not read..";
}

QString TableOfContent::relativeFilePath(const QString &src) {
    return userDir.relativeFilePath(src);
}