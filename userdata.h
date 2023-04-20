#ifndef USERDATA_H
#define USERDATA_H

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QBuffer>
#include <QImage>
#include <QJsonDocument>
#include <QImage>
#include <ebooklib/epubbook.h>



#ifdef  Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>

#endif


class UserData : public QObject {
    Q_OBJECT
public:
    /*
使用QT 创建一个sqlite 文件库，存储在路径 dataFile 下。
当dataFile不存在时， 初始化这个sqlite文件，并创建下面几张表

一张 book ，
一张叫write_label,

book 包含几个字段：
字段名称与类型:
id 是个64位字符串，是name的md5值。
name 字符串，最大500个字符
last_read_file 字符串，最大300个字符
last_read_scroll_number 数字
last_read_time datetime


write_label，字段名与类型：

id: 自增长id
book_id  等于 book.id
read_file 字符串，一个文件路径， 与book.last_read_file 一致
label_source 字符串， 最大2000个字符，
label_color 字符串，一个颜色值 #FFFFFF 格式
label_postil 字符串 最大2000个字符

*/
    explicit UserData(QObject *parent = nullptr);
    QString md5(const QString &str);

    /*
    函数 QJsonObject UserData::openBook(const QString &bookName)
    当被访问，并且传参数 bookName 时，
    链接db , 检查表book 中是否存在相同book.id 的数据，如果有读取记录，
    读取的记录，按字段名拼接到QJsonArray 输出。

    如果不存在，则创建记录。
    当创建记录时， 字段last_read_file 为空, las_read_scroll_number 为0
    拼合QJsonObject 输出.

    */
    Q_INVOKABLE QJsonObject openBook(const QString &bookName);

    /*
    函数   Q_INVOKABLE void UserData::read(const QString &bookName,
                          const QString &last_read_file,
                          int last_read_scroll_number);

    当检测到将last_read_file与book.last_read_file上一次的结果不相同的时候，
   触发一个信号, emit readPageChanged,并读取表 write_label 中，关于book_id,
   read_file 的所有记录， read_file == last_read_file 结果汇总成QJsonArray
   labelHistory emit readPageChanged( labelHistory )

     然后， 将last_read_file 和last_read_scroll_number ，根据bookNameid
    ,更新到book表中， 并且更新last_read_time的时间

    */
    Q_INVOKABLE void read(const QString &bookName, int last_read_index,
                          int last_read_scroll_number);
    Q_INVOKABLE QJsonArray books(int count = 0);
    Q_INVOKABLE void addBook(const QString &bookPath);
    Q_INVOKABLE void bookSearch();
    Q_INVOKABLE bool checkPermission(const QString &permission);
    Q_INVOKABLE bool requestPermission(const QString &permission);
signals:
    void readPageChanged(QJsonArray labeHistory);

private:
    QString dataFile;
    //    QSqlDatabase db;
};

#endif // USERDATA_H
