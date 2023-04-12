#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QNetworkAccessManager>

class NetworkManager : public QNetworkAccessManager {
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request,
                                 QIODevice *outgoingData = nullptr) override;
};

#endif // NETWORKMANAGER_H
