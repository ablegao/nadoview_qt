#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#include <QGuiApplication>

class MyApplication : public QGuiApplication {
    Q_OBJECT

public:
    MyApplication(int &argc, char **argv);
signals:
    void FileOpend(const QString &path);

protected:
    bool event(QEvent *event) override;
};

#endif // MYAPPLICATION_H
