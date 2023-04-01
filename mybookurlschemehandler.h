#ifndef MYBOOKURLSCHEMEHANDLER_H
#define MYBOOKURLSCHEMEHANDLER_H
#include "tableofcontent.h"
#include <QQuickImageProvider>

class MyBookUrlSchemeHandler : public QQuickImageProvider {
    Q_OBJECT
public:
    explicit MyBookUrlSchemeHandler(QObject *parent = nullptr);
    ~MyBookUrlSchemeHandler();
    TableOfContent *book;
    QPixmap requestPixmap(const QString &id, QSize *size,
                          const QSize &requestedSize) override;
};

#endif // MYBOOKURLSCHEMEHANDLER_H
