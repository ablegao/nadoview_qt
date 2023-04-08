#ifndef CUSTOMURLSCHEMEHANDLER_H
#define CUSTOMURLSCHEMEHANDLER_H
#include "tableofcontent.h"

#include <QBuffer>
#include <QMimeDatabase>
#include <QMimeType>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>

class CustomUrlSchemeHandler : public QWebEngineUrlSchemeHandler {
    Q_OBJECT
public:
    TableOfContent *book;
    explicit CustomUrlSchemeHandler(QObject *parent = nullptr);
    void requestStarted(QWebEngineUrlRequestJob *request);
};

#endif // CUSTOMURLSCHEMEHANDLER_H
