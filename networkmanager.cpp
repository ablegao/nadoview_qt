#include "networkmanager.h"

NetworkManager::NetworkManager(QObject *parent)
    : QNetworkAccessManager{parent} {}

QNetworkReply *NetworkManager::createRequest(Operation op,
                                             const QNetworkRequest &request,
                                             QIODevice *outgoingData) {
    qDebug() << "debug ... http requset" << request.url().scheme();
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}
