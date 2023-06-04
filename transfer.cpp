#include "transfer.h"

#include "QtCore/qlogging.h"
#include "transferworker.h"

Transfer::Transfer(QObject *parent) : QObject{parent} {
    TransferWorker *worker = new TransferWorker;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Transfer::transferText, worker, &TransferWorker::doWork);
    connect(worker, &TransferWorker::resultReady, this, &Transfer::resultReady);
    workerThread.start();
}
void Transfer::dictView(const QString &text) {
    QUrl cmd(QString("eudic://dict/%1").arg(text), QUrl::TolerantMode);
    QDesktopServices::openUrl(cmd);
    qDebug() << " open view system dict" << cmd;
}
