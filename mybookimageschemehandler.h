#ifndef MYBOOKURLSCHEMEHANDLER_H
#define MYBOOKURLSCHEMEHANDLER_H
#include "tableofcontent.h"
#include <QQuickImageProvider>

class MyBookImageSchemeHandler : public QQuickImageProvider {
    Q_OBJECT
public:
    explicit MyBookImageSchemeHandler(QObject *parent = nullptr);
    ~MyBookImageSchemeHandler();
    TableOfContent *book;
    QPixmap requestPixmap(const QString &id, QSize *size,
                          const QSize &requestedSize) override;
};

#endif // MYBOOKURLSCHEMEHANDLER_H
