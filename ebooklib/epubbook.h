#ifndef EPUBBOOK_H
#define EPUBBOOK_H
#include <QTextBlock>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextFragment>

#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QTemporaryDir>
#include <QtCore/QUrl>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlTableModel>
#include <QtXml/QDomDocument>

#include "ebooklib/ibook.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

// struct EpubItem {
//   QString id;
//   QString href;
//   QString mediaType;
// };

class EpubBook : public IBook {
    Q_OBJECT
public:
    //  EpubBook();
    EpubBook(QObject *parent = nullptr);
    //  EpubBook(QObject *parent = nullptr, const QString &epudfile = "");
    ~EpubBook();

    // 打开书籍
    Q_INVOKABLE int parseBook(const QString &epudfile = "") override;
    Q_INVOKABLE QVariantMap openChapter(const QString &url) override;
    Q_INVOKABLE QByteArray openFileByUrl(const QString &index) override;
    QString indexToUrl(int index) override;
    //    // 反馈两个id , 一个是url 所在的startPage =1 的id ,第二个是rowid
    //    标记的id ,
    //    // 如果没有找到，会和第一个一样
    //    Q_INVOKABLE QVector<int> getMenuByTagInfo(const QString &url,
    //                                              const QString &rowid);

    //    //  Q_INVOKABLE QByteArray getEpubFileData(const QString &file);
    //    Q_INVOKABLE QString parseContainer();
    //    Q_INVOKABLE int saveCover(const QString &outfile);
    //    Q_INVOKABLE void close();
    //    Q_INVOKABLE bool isClose();
    //    //    Q_INVOKABLE QString runHttpServer();
    //    Q_INVOKABLE void currentPageInfo(const QString &url, const QString
    //    &rowid,
    //                                     const QString &rowText);

private:
    // 解析 epub 中的 "META-INF/container.xml" 获得
    // <rootfile full-path="content.opf"
    // media-type="application/oebps-package+xml"/>
    // 中的 full-path
    QString parseContainer();
    void parseTocNcx();
    void parseNavXHtml();

    // 解析整个epub文件
    int parseEpub();
    QuaZip *m_epub = nullptr;
    QString contentOpfFile;
    QString tocFile;
    QDir tocBase;
    QString tocXhtml = "";
    QString version; // epub 版本号

    // content.opf 中的manifest字段下的所有元素
    QVector<QJsonObject> m_Manifest;

    // content.opf 中的 spine 字段下的所有元素
    // 这个字段应该是本书的正常阅读顺序
    QVector<QJsonObject> m_Spine;
};

#endif // EPUBBOOK_H
