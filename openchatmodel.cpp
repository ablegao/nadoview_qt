#include "openchatmodel.h"

OpenChatModel::OpenChatModel(QObject *parent) : QAbstractListModel{parent} {
  //    chatData
}
int OpenChatModel::rowCount(const QModelIndex &parent) const {
  return chatData.size();
}
QVariant OpenChatModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  //    if (mBook == nullptr)
  //        return QVariant();

  if (role == ChatMessage) {
    return chatData.at(index.row())
        .toMap()
        .value("chatMessage");  // 这里的chapterID 是mData中的key
  }

  if (role == ChatRole) {
    return chatData.at(index.row())
        .toMap()
        .value("chatRole");  // 这里的chapterID 是mData中的key
  }

  if (role == ChatID) {
    return chatData.at(index.row()).toMap().value("chatID");
  }
  return QVariant();
}
void OpenChatModel::sendMessage(const QString &msg, const QString &modelName) {
  QJsonArray message;

  beginInsertRows(QModelIndex(), chatData.size(), chatData.size());
  chatData.append(QVariantMap{
      {"chatMessage", msg},
      {"chatRole", "user"},
      {"chatID", ""},
      {"chatModel", modelName},

  });
  endInsertRows();
  int index = chatData.size();

  beginInsertRows(QModelIndex(), chatData.size(), chatData.size());
  chatData.append(QVariantMap{
      {"chatMessage", "Please wait a moment..."},
      {"chatRole", "assistant"},
      {"chatID", ""},
      {"chatModel", modelName},

  });
  endInsertRows();

  QJsonObject system;
  system["role"] = "system";
  system["content"] = chatData.at(0).toMap()["chatMessage"].toString();
  message.append(system);

  QJsonObject item;
  item["role"] = "user";
  item["content"] = msg;

  message.append(item);

  QString out = openChat(message, modelName);
  //    chatData.at(chatData.size() - 1).toMap()["chatMessage"] = out;
  chatData[index] = QVariantMap{
      {"chatMessage", out},
      {"chatRole", "assistant"},
      {"chatID", ""},
      {"chatModel", modelName},
  };
  QModelIndex modelIndex = createIndex(index, 0);
  emit dataChanged(modelIndex, modelIndex);
}
Q_INVOKABLE void OpenChatModel::addMessage(const QString &msg,
                                           const QString &roleName,
                                           const QString &modelName) {
  if (roleName == "system") {
    if (chatData.size() > 0) {
      beginResetModel();
      chatData[0] = QVariantMap{
          {"chatMessage", msg},
          {"chatRole", roleName},
          {"chatID", ""},
          {"chatModel", modelName},

      };
      endResetModel();
      qDebug() << "book changed " << msg;
      //            beginResetModel();
      return;
    }
  }
  beginInsertRows(QModelIndex(), chatData.size(), chatData.size());
  chatData.append(QVariantMap{
      {"chatMessage", msg},
      {"chatRole", roleName},
      {"chatID", ""},
      {"chatModel", modelName},

  });
  endInsertRows();
}

QString OpenChatModel::openChat(QJsonArray message, const QString &modelName) {
  QNetworkAccessManager manager;
  //    QUrl url("http://107.150.124.239:3000/v1/chat/completions");
  QUrl url("https://api.openai.com/v1/chat/completions");
  QNetworkRequest request(url);

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString openapiKey = env.value("OPENAPI_KEY", "default_value");
  qDebug() << "OPENAPI KEY" << openapiKey;

  request.setRawHeader("Authorization",
                       QString("Bearer %1").arg(openapiKey).toUtf8());
  //    qDebug() << "处理的指令：" << fulltext;
  QJsonObject body;
  //    QString text;
  //    if (fulltext.size() > 3000) {
  //        text = fulltext.mid(0, 3000);
  //    } else {
  //        text = fulltext;
  //    }
  //    QJsonArray message;
  //    QJsonObject item;
  //    item["role"] = "user";
  //    item["content"] = text;

  //    message.append(item);
  body["messages"] = message;
  //    body["max_tokens"] = 150;
  body["model"] = modelName;
  body["n"] = 1;
  body["stop"] = QJsonValue::Null;
  body["temperature"] = 0.5;

  QJsonDocument jsonDoc(body);
  QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

  QNetworkReply *reply = manager.post(request, jsonData);

  QEventLoop loop;
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  QByteArray response = reply->readAll();
  qDebug() << QString(response);
  //    emit openAiDescFinished(response);
  QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
  QString summary;
  if (jsonResponse.object().contains("choices")) {
    summary = jsonResponse.object()["choices"]
                  .toArray()
                  .at(0)
                  .toObject()["message"]
                  .toObject()["content"]
                  .toString()
                  .trimmed();
  } else {
    summary = response;
  }
  qDebug() << "openai 反馈数据:" << summary;
  reply->deleteLater();
  return summary;
}
