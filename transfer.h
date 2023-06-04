#ifndef TRANSFER_H
#define TRANSFER_H

#include <QDesktopServices>
#include <QObject>
#include <QThread>
#include <QUrl>

#include "QtCore/qtmetamacros.h"
#include "transferworker.h"

class Transfer : public QObject {
        Q_OBJECT
    public:
        explicit Transfer(QObject *parent = nullptr);
        QThread workerThread;
        ~Transfer() {
            workerThread.quit();
            workerThread.wait();
        }
        Q_INVOKABLE void dictView(const QString &text);
    signals:
        void resultReady(const QString &result);
        void transferText(const QString &text, const QString &fromLang = "auto",
                          const QString &toLang = "zh");

        //    void sendTransfer(const QString &text,const QString
        //    &fromLang="auto", const QString &toLang="zh");
};

#endif  // TRANSFER_H
