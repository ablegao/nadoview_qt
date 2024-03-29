#ifndef TABLEOFCONTENT_H
#define TABLEOFCONTENT_H
// #include <aws/core/Aws.h>
// #include <aws/translate/TranslateClient.h>
// #include <aws/translate/TranslateRequest.h>
// #include <aws/translate/model/TranslateTextRequest.h>
// #include <aws/translate/model/TranslateTextResult.h>

#include <QAbstractListModel>
#include <QEventLoop>
#include <QFile>
#include <QFont>
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QImage>
#include <QJsonDocument>
#include <QPainter>
#include <QRegularExpression>

#include "QtCore/qtmetamacros.h"
#include "QtNetwork/qnetworkreply.h"
#include "ebooklib/epubbook.h"
#include "ebooklib/ibook.h"

// #include <QWebEngineProfile>
#include <qnetworkaccessmanager.h>

class TableOfContent : public QAbstractListModel {
        Q_OBJECT
    public:
        enum Roles {
            ChapterID = Qt::UserRole + 1,
            ChapterName,
            ChapterUrl,
            ChapterLevel,
        };
        QHttpServer *server;
        explicit TableOfContent(QObject *parent = nullptr);
        ~TableOfContent();
        IBook *mBook = nullptr;
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index,
                      int role = Qt::DisplayRole) const override;

        //    Q_INVOKABLE void openChapter(int index);
        //    Q_INVOKABLE void openPageByUrl(const QString &url);
        //    Q_INVOKABLE void openChapterByUrl(const QString &chapterUrl);
        Q_INVOKABLE void openBook(const QString &bookUri);
        Q_INVOKABLE QString nextPage();  // 下一页
        Q_INVOKABLE QString prevPage();  // 上一页的URL

        Q_INVOKABLE QString markdownToHtml(const QString &fulltext,
                                           int fontsize, int w, int h);
        Q_INVOKABLE QString indexToUrl(int index);
        Q_INVOKABLE int urlToIndex(const QString &url);
        // Q_INVOKABLE int getMenuIndex();
        Q_INVOKABLE QByteArray openPage(const QString &url);
        Q_INVOKABLE int readIndex();
        // 获取当前章节在目录中的索引
        Q_INVOKABLE int getIndexOfTable();
        //
        Q_INVOKABLE bool useMarkDown();
        //    void requestStarted(QWebEngineUrlRequestJob *request) override;
        int screenWidth, screenHeight;
        Q_INVOKABLE void setSize(int w, int h);

        Q_INVOKABLE QString absoluteFilePath(const QString &u);
        Q_INVOKABLE void shareToImage(const QString &text,
                                      const QString &bookName,
                                      const QString &outdir, int fontSize = 16);
        Q_INVOKABLE QString hosts();
        QByteArray handleRequest(const QHttpServerRequest &request);
        QByteArray transferRequest(const QHttpServerRequest &request);
        Q_INVOKABLE QString relativeFilePath(const QString &url);

        Q_INVOKABLE QString readfile(const QString &file);
        //    QWebEngineUrlSchemeHandler *urlSchemeHandler();
    signals:
        void bookOpenFinishd(const QVariantMap &bookInfo);
        void openPageFinishd(const QString &url, int index);
        void selectedText(const QString &text);
        //    void bookChapterReaded(const QVariantMap &info);
        //    void openAiDescFinished(const QString &desc);

    protected:
        QHash<int, QByteArray> roleNames() const override;

    private:
        QRegularExpression imageRegex = QRegularExpression(
            "\\!\\[(?<altText>[^\\]]*)\\]\\((?<url>[^\\)]+)\\)");

        QRegularExpression linkRegex = QRegularExpression(
            "\\[(?<altText>[^\\]]*)\\]\\((?<url>[^\\)]+)\\)");
        QRegularExpression emStrongRegex =
            QRegularExpression("(\\*\\*\\*)(.+)(\\*\\*\\*)");
        QRegularExpression strongRegex =
            QRegularExpression("(\\*\\*)(.+)(\\*\\*)");
        QRegularExpression emRegex = QRegularExpression("(\\*)(.+)(\\*)");
        QDir userDir =
            QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
                .absoluteFilePath(".cache/nadoview");
};

#endif  // TABLEOFCONTENT_H
