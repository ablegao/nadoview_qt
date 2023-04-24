#ifndef MYAPPSINGLE_H
#define MYAPPSINGLE_H

#include <QObject>

class MyAppSingle : public QObject {
    Q_OBJECT
  public:
    MyAppSingle();
  signals:
    void fileOpend(const QString &filePath);
    void noOpenFile();
};

#endif  // MYAPPSINGLE_H
