#ifndef TRANSFERWORKER_H
#define TRANSFERWORKER_H

// #include <aws/comprehend/ComprehendClient.h>
// #include <aws/comprehend/ComprehendRequest.h>
// #include <aws/comprehend/ComprehendServiceClientModel.h>
//#include <aws/core/Aws.h>
//#include <aws/translate/TranslateClient.h>
//#include <aws/translate/TranslateRequest.h>
//#include <aws/translate/model/TranslateTextRequest.h>
//#include <aws/translate/model/TranslateTextResult.h>

#include <QDebug>
#include <QDesktopServices>
#include <QObject>
#include <QRegularExpression>
#include <QThread>
#include <QUrl>

class TransferWorker : public QObject {
        Q_OBJECT

    public:
        bool containsChinese(const QString &text);
    public slots:
        void doWork(const QString &text = "", const QString &fromLang = "auto",
                    const QString &toLang = "zh");

    signals:
        void resultReady(const QString &result);
        //    void resultError(const QString &result);
};

#endif  // TRANSFERWORKER_H
