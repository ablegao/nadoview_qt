#include "ebooklib/epubbook.h"

#include <QRegularExpression>

EpubBook::EpubBook(QObject *parent) {}

EpubBook::~EpubBook() {
    if (m_epub != nullptr) {
//        qDebug() << " book closed";
        m_epub->close();
    }
}

// void EpubBook::close() {
//     if (m_epub != nullptr) {
//         m_epub->close();
//         delete m_epub;
//         m_epub = nullptr;
//     }
//     if (http != nullptr) {
//         delete http;
//         http = nullptr;
//     }

//    if (tmpDir != nullptr) {
//        delete tmpDir;
//        tmpDir = nullptr;
//    }
//}

int EpubBook::parseBook(const QString &epubfile) {
    bookPath = epubfile;
    m_epub = new QuaZip(epubfile);
    if (!m_epub->open(QuaZip::mdUnzip))
        return -1;

    return parseEpub();
}

int EpubBook::parseBook(QIODevice *ioDevice) {
//    bookPath = epubfile;
    m_epub = new QuaZip(ioDevice);
    if (!m_epub->open(QuaZip::mdUnzip))
        return -1;

    return parseEpub();
}

// bool EpubBook::isClose() { return m_epub == nullptr; }

QString EpubBook::parseContainer() {

    QDomDocument doc;
    doc.setContent(openFileByUrl("META-INF/container.xml"));
    QDomElement docElem = doc.documentElement();
    QDomNodeList root = docElem.elementsByTagName("rootfile");
    QString out;
    if (root.size() > 0) {
        out = root.at(0).toElement().attribute("full-path");
    }
    doc.clear();
    return out;
}

int EpubBook::parseEpub() {
    //  this.->m_epub = (epubdir);

    //    QString content;
    //  getEpubContentOpf(epubdir, &contentOpfFile);
    QDomDocument doc("ebookxml");
    contentOpfFile = parseContainer();
    if (contentOpfFile == "")
        return -1;
    // 读取 content.opb
    QuaZipFile openxml(m_epub);

    doc.setContent(openFileByUrl(contentOpfFile));

    QDomElement docElem = doc.documentElement();

    version = docElem.toElement().attribute("version");

    QDomNodeList metadata = docElem.elementsByTagName("metadata");
    if (metadata.size() <= 0) {
        return -4;
    }
    QStringList tags;
    tags << "dc:title"
         << "dc:language"
         << "dc:creator"
         << "dc:publisher"
         << "description"
         << "dc:date"
         << "dc:identifier"
         << "meta";
    QDomNode node = metadata.at(0).firstChild();
    QString tagname;
    QDomElement item;
    QString coverName;
    while (!node.isNull()) {
        item = node.toElement();
        if (!item.isNull()) {
            tagname = item.tagName();
            switch (tags.indexOf(tagname)) {
            case 0:
                if (item.text() != "") {
                    this->title = item.text();
                }
                break;
            case 1:
//                qDebug() << " 这里应该是language 标签,  " << tagname;
                this->language = item.text();
                break;
            case 2:
                this->creators << item.text();
                break;
            case 3:
                this->publisher = item.text();
                break;
            case 4:
                this->description = item.text();
                break;
            case 5:
                this->date = item.text();
                break;
            case 6:
                if (item.attribute("id") == "uuid_id") {
                    this->uuid = item.text();
                }
                break;
            case 7:
                if (item.attribute("name") == "cover") {
                    coverName = item.attribute("content");
                }
                break;
            }
        }
        node = node.nextSibling();
    }
    //  this->bookPath = epubdir;
    // QFileInfo(contentOpfFile).absolutePath();
    // 先判断在不在， 不在从xml 中读取

    QDomNodeList manifest = docElem.elementsByTagName("manifest");
    node = manifest.at(0).firstChild();

    tocBase = QFileInfo("/" + contentOpfFile).dir();
//    qDebug() << tocBase;
    //  QString contentBase = QFileInfo(contentOpfFile).absolutePath();
    while (!node.isNull()) {
        item = node.toElement();
        QJsonObject itemObj;
        itemObj["id"] = item.attribute("id");

        itemObj["href"] = item.attribute("href");
        itemObj["media_type"] = item.attribute("media-type");
        m_Manifest << itemObj;
        if (item.attribute("id") == coverName) {
            coverImg = QDir::cleanPath(
                tocBase.absoluteFilePath(item.attribute("href")));
        }
        node = node.nextSibling();
    }

    QDomNodeList spine = docElem.elementsByTagName("spine");
    node = spine.at(0).firstChild();
    while (!node.isNull()) {
        item = node.toElement();
        QJsonObject itemObj;
        itemObj["idref"] = item.attribute("idref");
        // 当值是no的时候， 表示这个元素不在列表中显示
        itemObj["linear"] = item.attribute("linear", "");
        m_Spine << itemObj;
        node = node.nextSibling();
    }

    doc.clear();

//    if (version == "2.0") {
    parseTocNcx();
//    } else if (version == "3.0") {
    parseNavXHtml();
//    } else {
//        qDebug() << "不知道什么版本。无法解析这个epub" << version;
//    }

    return 0;
}

// 解析epub 文件的nav.xhtml,  epub3.0
void EpubBook::parseNavXHtml() {
    QString navFile;
    for (const QVariant &item : m_Manifest) {
        QVariantMap map = item.toMap();
        if (map.contains("id") && map["id"] == "nav") {
            navFile = map["href"].toString();
            break;
        }
    }
    if (navFile == "")
        return;

    QDomDocument doc;
    doc.setContent(openFileByUrl(tocBase.absoluteFilePath(navFile)));

    QDomNodeList navPoints = doc.elementsByTagName("a");
    tableOfContent.clear();

    for (int i = 0; i < navPoints.size(); i++) {
        QDomElement navPoint = navPoints.at(i).toElement();
        QString label = navPoint.text();
        QString href = navPoint.attribute("href");
        QVariantMap entry;
        entry["chapterName"] = label;
        entry["chapterUrl"] = QDir::cleanPath(tocBase.filePath(href));
        entry["chapterLevel"] = 0;
        tableOfContent.append(entry);
    }
}

// 解析epub 的toc 文件， epub 2.0
void EpubBook::parseTocNcx() {
    QString ncxFile;
    for (const QVariant &item : m_Manifest) {
        QVariantMap map = item.toMap();
        if (map.contains("id") && map["id"] == "ncx") {
            ncxFile = map["href"].toString();
            break;
        }
    }
    if (ncxFile == "")
        return;
//    ncxFile = QDir::cleanPath(ncxFile);
    qDebug() << "ncx file" << ncxFile;

//    QuaZipFile openxml(m_epub);
    QDomDocument doc;
    doc.setContent(openFileByUrl(tocBase.absoluteFilePath(ncxFile)));

    QDomNodeList tocPoints = doc.elementsByTagName("navPoint");
    tableOfContent.clear();
    qDebug() << "table of content size..........." << tocPoints.size();
    // 插入图书icon

    for (int i = 0; i < tocPoints.size(); ++i) {
        QDomElement navPoint = tocPoints.at(i).toElement();

        QString label = navPoint.firstChildElement("navLabel")
                            .firstChildElement("text")
                            .text();
        QString href = navPoint.firstChildElement("content").attribute("src");
        //        qDebug() << " URL TESET" << tocBase.filePath(href);
        QVariantMap entry;
        entry["chapterName"] = label;
        entry["chapterUrl"] = QDir::cleanPath(tocBase.filePath(href));
        entry["chapterLevel"] = 0;
        tableOfContent.append(entry);
    }
    doc.clear();
//    qDebug() << "读取完毕";
}
QString EpubBook::indexToUrl(int index) {
    //    if (index < 0 || index >= tableOfContent.size()) {
    //        return "";
    //    }
    //    QVariantMap item = tableOfContent.at(index).toMap();
    //    return item["chapterUrl"].toString();
    if (index < 0 || index >= m_Spine.size()) {
        return "";
    }
    mSpineIndex = index;

    return getCurrentPageUrl();
}
int EpubBook::urlToIndex(const QString &url)
{
    for (int i = 0; i < m_Manifest.size(); i++) {
        QVariantMap item = m_Manifest.at(i).toVariantMap();
        if (item["href"].toString() == url || url.endsWith(item["href"].toString())) {
            for (int n = 0; n < m_Spine.size(); n++) {
                QVariantMap spineItem = m_Spine.at(n).toVariantMap();
//                qDebug() << " page url......" << spineItem["idref"].toString()
//                         << item["id"].toString();
                if (spineItem["idref"].toString() == "") {
                    continue;
                }
                if (spineItem["idref"].toString() == item["id"].toString()) {
                    mSpineIndex = n;
                    return n;
                }
            }
        }
    }
    return 0;
}
QString EpubBook::getFirstPageUrl() {
    for (int i = 0; i < m_Spine.size(); i++) {
        qDebug() << " row " << m_Spine.at(i).toVariantMap()["idref"]
                 << m_Spine.at(i).toVariantMap()["linear"];
        if (m_Spine.at(i).toVariantMap()["linear"].toString() == "no") {
            continue;
        }
        mSpineIndex = i;

        return getCurrentPageUrl();
    }
    return "";
}
int EpubBook::readIndex() { return mSpineIndex; }
// void EpubBook::setReadIndex(int index) { mSpineIndex = index; }

QString EpubBook::nextPage() {
    if (mSpineIndex + 1 >= m_Spine.size()) {
        return "";
    }
    mSpineIndex += 1;
    //    int i = 0;

    return getCurrentPageUrl();
}

QString EpubBook::prevPage() {
    if (mSpineIndex - 1 < 0) {
        return "";
    }
    mSpineIndex -= 1;
    //    int i = 0;

    return getCurrentPageUrl();
}

QString EpubBook::getCurrentPageUrl() {
    if (mSpineIndex < 0 || mSpineIndex >= m_Spine.size()) {
        return tocBase.absolutePath();
    }
    QVariantMap item = m_Spine.at(mSpineIndex).toVariantMap();
    for (int i = 0; i < m_Manifest.size(); i++) {
        if (item["idref"].toString() ==
            m_Manifest.at(i).toVariantMap()["id"].toString()) {
            QString href = m_Manifest.at(i).toVariantMap()["href"].toString();
            return QDir::cleanPath(tocBase.filePath(href));
        }
    }
    return "";
}
QVariantMap EpubBook::openChapter(const QString &url) {

    QVariantMap out;
    QString href = url;

    QJsonArray rows;
    QStringList hrefs = href.split("#");
    if (hrefs.size() > 1) {
        href = hrefs[0];
        out["link"] = hrefs[1];
    } else {
        out["link"] = "";
    }
    out["href"] = href;

    //    QTextDocument doc{};
    //    QByteArray html = openFileByUrl(href);
    //    doc.setHtml(html);
    //    QString desc = doc.toPlainText();
    //    if (desc.length() > 3000) {
    //        desc = desc.mid(0, 3000);
    //    }
    //    out["desc"] = desc;
    //    out["html"] = html;
    //    QTextBlock block = doc.begin();
    //    while (block.isValid()) {
    //        QTextBlock::iterator it;

    //        //        qDebug() << "段落：" << block.fragmentIndex() << doc.;
    //        QTextCursor cursor(&doc);
    //        cursor.setPosition(block.position());
    //        cursor.movePosition(QTextCursor::EndOfBlock,
    //        QTextCursor::KeepAnchor);
    //        // 将选择的内容转换为Markdown
    //        QTextDocumentFragment fragment = QTextDocumentFragment(cursor);
    //        // 输出内容以markdown 为标准，不在这里参与格式化
    //        rows << fragment.toMarkdown();
    //        block = block.next();
    //    }
    //    out["rows"] = rows;
    //    out["html"] = openChapterByUrl(href);

    //    qDebug() << "read page ..." << out.at(0);
    return out;
}

QByteArray EpubBook::openFileByUrl(const QString &url) {
    QString href = url;

    QuaZipFile openHtml(m_epub);
    QStringList hrefs = href.split("#");
    if (hrefs.size() > 1) {
        href = hrefs[0];
    }
//    qDebug() << "absoluteFilePath(href)" << absoluteFilePath(href);

    //    qDebug() << "尝试打开文件:" << QDir::cleanPath(tocBase.filePath(href))
    //             << href;

    if (m_epub->setCurrentFile(href) || m_epub->setCurrentFile(href.mid(1)) ||
         m_epub->setCurrentFile(absoluteFilePath(href))) {
        if (openHtml.open(QIODevice::ReadOnly)) {
            QByteArray data = openHtml.readAll();
            openHtml.close();
            return data;
        } else {
            qDebug() << "open file errors" << url << bookPath ;
        }

        //        openHtml.close();
    } else {
        qDebug() << "not open . " << href;
    }
    //    qDebug() << "read page ..." << out.at(0);
    return "";
}

QString EpubBook::absoluteFilePath(const QString &u) {
    //
    //    qDebug() << "current pageurl  " << getCurrentPageUrl();
    QList<QString> splitArr = getCurrentPageUrl().mid(1).split("/");
    QList<QString> splitArrU = u.split("/");
    if (splitArr.size() > 1 && splitArrU.size() > 1 &&
        splitArrU[0] == splitArr[0]) {
        return u;
    }

    QFileInfo fileInfo(getCurrentPageUrl());
    if (u.mid(0, 1) != "/") {
        return QDir::cleanPath(fileInfo.dir().absoluteFilePath(u));
    }

    return u;
}
