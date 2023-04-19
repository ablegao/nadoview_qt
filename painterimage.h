#ifndef PAINTERIMAGE_H
#define PAINTERIMAGE_H

#include <QWidget>
#include <QString>
#include <QPainter>
#include <QFont>
#include <QImage>

class PainterImage : public QWidget
{
    Q_OBJECT
public:
    explicit PainterImage(QWidget *parent = nullptr);
    Q_INVOKABLE void shareToImage(const QString &text, const QString &bookName, const QString &outdir, int fontSize=16);

signals:

};

#endif // PAINTERIMAGE_H
