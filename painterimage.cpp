#include "painterimage.h"

PainterImage::PainterImage(QWidget *parent)
    : QWidget{parent}
{

}
Q_INVOKABLE void PainterImage::shareToImage(const QString &text,
                                            const QString &bookName,
                                            const QString &outdir,
                                            int fontSize)
{
    int fontCount = text.length();
    int width = 220;
    int height = fontCount * fontSize / width * fontSize;

    QImage image(width, height, QImage::Format_RGB32);
    QPainter painter(&image);
    painter.begin(this);
    //    painter.setFont(QFont("Times", 30, QFont::Bold));
    //    painter.begin(this);
    QRect textRect(10, 10, width - 20, height + 100);
    painter.drawText(textRect, text);
    painter.end();
    image.save(outdir);

}
