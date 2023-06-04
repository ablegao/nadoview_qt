#include "transferworker.h"
#include "QtCore/qminmax.h"

void TransferWorker::doWork(const QString &text, const QString &fromLang,
                            const QString &toLang) {
    QString text1 = text.trimmed();
    // if (text1.split(" ").size() > 1) {
    if (containsChinese(text) == false) {
//        Aws::SDKOptions options;
//        Aws::InitAPI(options);
//        {
//            Aws::Translate::TranslateClient client;
//            Aws::Translate::Model::TranslateTextRequest request;
//            request =
//                request.WithSourceLanguageCode(fromLang.toStdString().c_str())
//                    .WithTargetLanguageCode(toLang.toStdString().c_str());
//            request.SetText(text1.toStdString().c_str());
//            auto outcome = client.TranslateText(request);
//            if (outcome.IsSuccess()) {
//                auto outtext = outcome.GetResult().GetTranslatedText();
//                emit resultReady(outtext.c_str());
//            } else {
//            }
//        }

//        Aws::ShutdownAPI(options);
        emit resultReady(text1);
    } else {
        emit resultReady(text1);
    }
}

bool TransferWorker::containsChinese(const QString &text) {
    int textCount = qMin(text.length(), 10);

    for (int i = 0; i < textCount; i++) {
        if (text[i].unicode() >= 0x4e00 && text[i].unicode() <= 0x9fa5) {
            return true;
        }
    }
    return false;
}
