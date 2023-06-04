#ifndef CUSTOMURLSCHEMEHANDLER_H
#define CUSTOMURLSCHEMEHANDLER_H
#include <QBuffer>
#include <QMimeDatabase>
#include <QMimeType>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>

#include "tableofcontent.h"

class CustomUrlSchemeHandler : public QWebEngineUrlSchemeHandler {
        Q_OBJECT
    public:
        TableOfContent *book;
        explicit CustomUrlSchemeHandler(QObject *parent = nullptr);
        void requestStarted(QWebEngineUrlRequestJob *request);
};

#endif  // CUSTOMURLSCHEMEHANDLER_H
