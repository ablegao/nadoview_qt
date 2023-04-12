#ifndef TABLEOFCONTENT_H
#define TABLEOFCONTENT_H
#include "QtNetwork/qnetworkreply.h"
#include "ebooklib/epubbook.h"
#include "ebooklib/ibook.h"
#include <QAbstractListModel>
#include <QEventLoop>
#include <QJsonDocument>
#include <QRegularExpression>

//#include <QWebEngineProfile>
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
    explicit TableOfContent(QObject *parent = nullptr);
    ~TableOfContent();
    IBook *mBook = nullptr;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void openChapter(int index);
    Q_INVOKABLE void openPageByUrl(const QString &url);
    Q_INVOKABLE void openChapterByUrl(const QString &chapterUrl);
    Q_INVOKABLE void openBook(const QString &bookUri);
    Q_INVOKABLE QString nextPage(); //下一页
    Q_INVOKABLE QString prevPage(); //上一页的URL

    Q_INVOKABLE QString markdownToHtml(const QString &fulltext, int fontsize,
                                       int w, int h);
    Q_INVOKABLE QString indexToUrl(int index);
    Q_INVOKABLE QByteArray openPage(const QString &url);
    Q_INVOKABLE int readIndex();
    Q_INVOKABLE bool useMarkDown();
    //    void requestStarted(QWebEngineUrlRequestJob *request) override;
    int screenWidth, screenHeight;
    Q_INVOKABLE void setSize(int w, int h);

    Q_INVOKABLE QString absoluteFilePath(const QString &u);

    //    QWebEngineUrlSchemeHandler *urlSchemeHandler();
signals:
    void bookOpenFinishd(const QVariantMap &bookInfo);
    void bookChapterReaded(const QVariantMap &info);
    void openAiDescFinished(const QString &desc);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QRegularExpression imageRegex =
        QRegularExpression("\\!\\[(?<altText>[^\\]]*)\\]\\((?<url>[^\\)]+)\\)");

    QRegularExpression linkRegex =
        QRegularExpression("\\[(?<altText>[^\\]]*)\\]\\((?<url>[^\\)]+)\\)");
    QRegularExpression emStrongRegex =
        QRegularExpression("(\\*\\*\\*)(.+)(\\*\\*\\*)");
    QRegularExpression strongRegex = QRegularExpression("(\\*\\*)(.+)(\\*\\*)");
    QRegularExpression emRegex = QRegularExpression("(\\*)(.+)(\\*)");
};

#endif // TABLEOFCONTENT_H
