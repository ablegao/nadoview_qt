#include "ebooklib/epubbook.h"

#include <QRegularExpression>

#include "QtXml/qdom.h"

EpubBook::EpubBook(QObject *parent) {}

EpubBook::~EpubBook() {
    // if (m_epub != nullptr) {
    //     //        qDebug() << " book closed";
    //     m_epub->close();
    // }
}

int EpubBook::parseBook(const QString &epubfile) {
    // 生成路径

    bookPath = epubfile;
    if (epubfile.endsWith(".epub")) {
        m_epub = new QuaZip(epubfile);
        if (!m_epub->open(QuaZip::mdUnzip)) return -1;
        //    QDomDocument doc;
        // 优先解析出 container.xml中的
        // <rootfile full-path="content.opf"
        // media-type="application/oebps-package+xml"/> content.opf 文件
        QuaZipFile openzip(m_epub);
        m_epub->setCurrentFile("META-INF/container.xml");
        if (!openzip.open(QIODevice::ReadOnly)) {
            qDebug() << "reaed container.xml error";
            return -1;
        }
        QByteArray data = openzip.readAll();
        QDomDocument doc;
        doc.setContent(data);
        QDomElement docElem = doc.documentElement();
        QDomNodeList root = docElem.elementsByTagName("rootfile");

        if (root.size() > 0) {
            contentOpfFile = root.at(0).toElement().attribute("full-path");
        }
        doc.clear();
        openzip.close();

        // 解析contentOpfFile , 获取到里面的dc:title标签
        //    qDebug() << "contentOpfFile" << contentOpfFile;
        m_epub->setCurrentFile(contentOpfFile);
        if (!openzip.open(QIODevice::ReadOnly)) {
            qDebug() << "opf file read error";
            return -1;
        }
        data = openzip.readAll();
        doc.setContent(data);
        docElem = doc.documentElement();
        QDomNodeList nodes = docElem.elementsByTagName("dc:title");
        if (nodes.length() == 0) {
            qDebug() << "read title error";
            return -1;
        }
        title = nodes.at(0).toElement().text();
        openzip.close();
        QCryptographicHash hash(QCryptographicHash::Md5);
        hash.addData(this->title.toUtf8());
        uuid = QString(hash.result().toHex());

        bookPath = userDir.absoluteFilePath(this->uuid);
        if (!QDir(bookPath).exists()) {
            JlCompress::extractDir(epubfile, bookPath);
        }

        m_epub->close();
    }
    return parseEpub();
}

QByteArray EpubBook::openFileByUrl(const QString &url) {
    // QString href = url;

    QFile file(tocBase.absoluteFilePath(url));
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        return data;
    } else {
        qDebug() << "open file errors," << url << bookPath;
    }

    return "";
}

QString EpubBook::parseContainer() {
    QDomDocument doc;
    QFile file(bookPath + "/META-INF/container.xml");
    if (file.open(QIODevice::ReadOnly)) {
        doc.setContent(file.readAll());
        file.close();
    }
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
    if (contentOpfFile == "") {
        qDebug() << "read container.xml error:not foud";
        return -1;
    }
    // 读取 content.opb
    //    QuaZipFile openxml(m_epub);
    // 需要换成文件读取
    contentOpfFile = bookPath + "/" + contentOpfFile;
    tocBase = QFileInfo(contentOpfFile).dir();
    QFile file(contentOpfFile);
    if (file.open(QIODevice::ReadOnly)) {
        doc.setContent(file.readAll());
    } else {
        qDebug() << "not open error" << contentOpfFile;
        return -3;
    }
    qDebug() << "OPF" << contentOpfFile;
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
                        QCryptographicHash hash(QCryptographicHash::Md5);
                        hash.addData(this->title.toUtf8());
                        uuid = QString(hash.result().toHex());
                    }
                    break;
                case 1:
                    //                qDebug() << " 这里应该是language 标签,  "
                    //                << tagname;
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
                    // if (item.attribute("id") == "uuid_id") {
                    //     this->uuid = item.text();
                    // }
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
    tocName = spine.at(0).toElement().attribute("toc", "");

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
    parseTocNcx();
    parseNavXHtml();
    qDebug() << "解析完成" << tableOfContent.toList().size() << coverImg;
    QFileInfo img(bookPath + "/" + uuid + ".jpg");
    qDebug() << "---------------- |||||| " << img;
    if (!img.exists() && QFile(coverImg).exists()) {
        // QFile::copy(coverImg, bookPath + "/" + uuid + ".jpg");
        QImage image;
        if (image.load(coverImg)) {
            image.scaled(170, 200, Qt::KeepAspectRatio);
            QFile img(bookPath + "/" + uuid + ".jpg");
            if (img.open(QIODevice::WriteOnly)) {
                image.save(&img, "JPEG");
            }
        }
    }
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
    if (navFile == "") return;

    QDomDocument doc;
    doc.setContent(openFileByUrl(navFile));

    QDomNodeList navPoints = doc.elementsByTagName("a");
    tableOfContent.clear();

    for (int i = 0; i < navPoints.size(); i++) {
        QDomElement navPoint = navPoints.at(i).toElement();
        QString label = navPoint.text();
        QString href = navPoint.attribute("href");
        QVariantMap entry;
        entry["chapterName"] = label;
        entry["chapterUrl"] = href;

        qDebug()  << "DEBUGl;;;" << entry["chapterUrl"];
        entry["chapterLevel"] = 0;
        tableOfContent.append(entry);
    }
}

// 解析epub 的toc 文件， epub 2.0
void EpubBook::parseTocNcx() {
    QString ncxFile;
    for (const QVariant &item : m_Manifest) {
        QVariantMap map = item.toMap();
        if (map.contains("id") && map["id"] == tocName) {
            ncxFile = map["href"].toString();
            break;
        }
    }
    if (ncxFile == "") return;
    //    ncxFile = QDir::cleanPath(ncxFile);
    qDebug() << "ncx file" << ncxFile;

    QuaZipFile openxml(m_epub);
    QDomDocument doc;
    doc.setContent(openFileByUrl(ncxFile));

    QDomNodeList tocPoints = doc.elementsByTagName("navPoint");
    tableOfContent.clear();
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
        entry["chapterUrl"] = href ;//QDir(bookPath).relativeFilePath(QDir::cleanPath(tocBase.filePath(href)));
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
int EpubBook::urlToIndex(const QString &url) {
    for (int i = 0; i < m_Manifest.size(); i++) {
        QVariantMap item = m_Manifest.at(i).toVariantMap();
        if (item["href"].toString() == url ||
            url.endsWith(item["href"].toString())) {
            for (int n = 0; n < m_Spine.size(); n++) {
                QVariantMap spineItem = m_Spine.at(n).toVariantMap();
                //                qDebug() << " page url......" <<
                //                spineItem["idref"].toString()
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
int EpubBook::getIndexOfTable() {
    QString url = getCurrentPageUrl();
    for (int i = 0; i < tableOfContent.size(); i++) {
        if (tableOfContent.at(i).toMap()["chapterUrl"].toString() == url) {
            return i;
        }
    }
    return 0;
}
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
            return href;
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
