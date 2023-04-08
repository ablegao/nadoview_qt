#ifndef IBOOK_H
#define IBOOK_H

//#include "cssparse.h"
#include <QtCore/QJsonArray>
#include <QtCore/QObject>
// #include <QtQuick/QQuickItem>
#include <QtXml/QDomDocument>

class IBook : public QObject {
public:
    QString title;
    QString cover;
    QString coverImg = "";
    QString date;
    QStringList creators;
    QString publisher;
    QString language;
    QString description;
    QString bookPath;
    QString uuid;
    QVariantList tableOfContent;
    //  QJsonArray bookMenus;

    //    Q_INVOKABLE virtual QDomDocument readViewPage() =0;
    //    Q_INVOKABLE virtual QDomNode getPageById(int id) =0;
    IBook(QObject *parent = nullptr);
    ~IBook();
    Q_INVOKABLE QString getTitle() { return this->title; }
    Q_INVOKABLE QString getCover() { return this->cover; }
    Q_INVOKABLE QStringList getCreators() { return this->creators; }
    Q_INVOKABLE QString getPublsiher() { return this->publisher; }
    Q_INVOKABLE QString getLanguage() { return this->language; }
    Q_INVOKABLE QString getDescription() { return this->description; }
    Q_INVOKABLE QString getDate() { return this->date; }

    Q_INVOKABLE virtual int parseBook(const QString &epudfile = "") = 0;

    // 打开一个章节， 内容需要解析放到QVariantList 中。QML 处理实际分野逻辑
    virtual QString indexToUrl(int index) = 0;
    Q_INVOKABLE virtual QVariantMap openChapter(const QString &url) = 0;
    Q_INVOKABLE virtual QString getFirstPageUrl() = 0;
    virtual QString nextPage() = 0;
    virtual QString prevPage() = 0;
    Q_INVOKABLE virtual QByteArray openFileByUrl(const QString &url) = 0;

    //    virtual int Save();
    // signals:
    //     virtual void parseMenuCompleted(QVector<QJsonObject>) = 0;
    //     virtual void parseMenuError(QString) = 0;
};

#endif // IBOOK_H
