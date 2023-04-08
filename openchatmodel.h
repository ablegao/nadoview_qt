#ifndef OPENCHATMODEL_H
#define OPENCHATMODEL_H

#include <QAbstractListModel>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcessEnvironment>

class OpenChatModel : public QAbstractListModel {
    Q_OBJECT

    Q_PROPERTY(QString bookName MEMBER bookName NOTIFY bookNameChanged)
public:
    enum Roles {
        ChatMessage = Qt::UserRole + 1,
        ChatRole,
        ChatID,
        //        ChapterLevel,
    };
    QString bookName;
    explicit OpenChatModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void sendMessage(const QString &msg, const QString &modelName);
    Q_INVOKABLE void addMessage(const QString &msg, const QString &roleName,
                                const QString &modelName = "");
signals:
    void bookNameChanged();

protected:
    QString openChat(QJsonArray message, const QString &modelName);
    QHash<int, QByteArray> roleNames() const override {
        // 这里 chapterName 是QML 里面的调用名称
        QHash<int, QByteArray> roles;
        roles[ChatMessage] = "chatMessage";
        roles[ChatRole] = "chatRole";
        roles[ChatID] = "chatID";
        //        roles[ChapterLevel] = "chapterLevel";
        return roles;
    }
    QVariantList chatData{
        //        QVariantMap{{"chatMessage", "Reading <" + bookName + ">"},
        //                    {"chatRole", "system"},
        //                    {"chatID", "id11"}},
        //        QVariantMap{{"chatMessage",
        //                     "我在阅读这本书，当我提供一些文字的时候，请给我一些指导"},
        //                    {"chatRole", "user"},
        //                    {"chatID", "id11"}},

    };
};

#endif // OPENCHATMODEL_H
