#include "myapplication.h"

#include <QFileOpenEvent>
MyApplication::MyApplication(int &argc, char **argv)
    : QGuiApplication(argc, argv) {}
bool MyApplication::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *fileOpenEvent = static_cast<QFileOpenEvent *>(event);
        QString fileName = fileOpenEvent->file();
        qDebug() << "File open requested:" << fileName;

        // Handle the file open request here
        // For example, open the file in your application
        emit fileOpend(fileName);
        return false;
    }

    return QGuiApplication::event(event);
}
