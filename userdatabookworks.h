#ifndef USERDATABOOKWORKS_H
#define USERDATABOOKWORKS_H

#include <QBuffer>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QRunnable>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QThread>

#include "QtCore/qjsonarray.h"
class UserDataBookWorks : public QObject, public QRunnable {
        Q_OBJECT
    public:
        QString tag = "";
        void run();
    signals:
        void fetchBook(QJsonObject);
        void tags(QJsonArray);

    private:
        QDir userDir =
            QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
                .absoluteFilePath(".cache/nadoview");
};

#endif  // USERDATABOOKWORKS_H
