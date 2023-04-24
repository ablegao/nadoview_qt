#include "transferworker.h"

void TransferWorker::doWork(const QString &text, const QString &fromLang,
                            const QString &toLang) {
    QString text1 = text.trimmed();
    if (text1.split(" ").size() > 1) {
        Aws::SDKOptions options;
        Aws::InitAPI(options);
        {
            Aws::Translate::TranslateClient client;
            Aws::Translate::Model::TranslateTextRequest request;
            request =
                request.WithSourceLanguageCode(fromLang.toStdString().c_str())
                    .WithTargetLanguageCode(toLang.toStdString().c_str());
            request.SetText(text1.toStdString().c_str());
            auto outcome = client.TranslateText(request);
            if (outcome.IsSuccess()) {
                auto outtext = outcome.GetResult().GetTranslatedText();
                emit resultReady(outtext.c_str());
            } else {
            }
        }
        Aws::ShutdownAPI(options);
    } else {
        // 使用QProcess 调用系统命令open dict://WORD, 这里面的WORD 是变量text1
        // QUrl cmd(QString("eudic://dict/%1").arg(text1), QUrl::TolerantMode);
        // QDesktopServices::openUrl(cmd);
        emit resultReady(text1);
    }
}
