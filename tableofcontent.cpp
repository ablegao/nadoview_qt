#include "tableofcontent.h"

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

TableOfContent::TableOfContent(QObject *parent) : QAbstractListModel(parent) {}
TableOfContent::~TableOfContent() { mBook = nullptr; }

QVariant TableOfContent::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    if (mBook == nullptr)
        return QVariant();

    if (role == ChapterID) {
        return mBook->tableOfContent.at(index.row())
            .toMap()
            .value("chapterID"); // 这里的chapterID 是mData中的key
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

Q_INVOKABLE void TableOfContent::openChapter(int index) {
    emit bookChapterReaded(mBook->openChapter(mBook->indexToUrl(index)));
}

Q_INVOKABLE void TableOfContent::openPageByUrl(const QString &url) {
    emit bookChapterReaded(mBook->openChapter(url));
}
Q_INVOKABLE void TableOfContent::openChapterByUrl(const QString &url) {
    //    emit bookChapterReaded(mBook->openChapter(index));
}
Q_INVOKABLE void TableOfContent::openBook(const QString &bookUri) {
    mBook = new EpubBook();
    int status = mBook->parseBook(bookUri);
    if (status == 0) {
        qDebug() << "file open finished";
        emit layoutChanged();
        QVariantMap info = {
            {"name", mBook->title},
            {"lang", mBook->language},
            {"coverImg", mBook->coverImg},
            {"firstPageUrl", mBook->getFirstPageUrl()},
        };
        emit bookOpenFinishd(info);
    } else {
        qDebug() << "open file error";
    }

    //    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler("mybook",
    //                                                                 mBook);
}

Q_INVOKABLE QString TableOfContent::nextPage() { return mBook->nextPage(); }
Q_INVOKABLE QString TableOfContent::prevPage() { return mBook->prevPage(); }
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
    if (mBook == nullptr)
        return false;
    if (mBook->language.contains("zh"))
        return false;
    return true;
}

QString TableOfContent::indexToUrl(int index) {
    return mBook->indexToUrl(index);
}
int TableOfContent::readIndex() { return mBook->readIndex(); }

// void TableOfContent::requestStarted(QWebEngineUrlRequestJob *request) {
//     qDebug() << "Request started for URL:" << request;
// }

// QWebEngineUrlSchemeHandler *TableOfContent::urlSchemeHandler() {}
