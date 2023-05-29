#include "userdatabookworks.h"
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsonvalue.h"

void UserDataBookWorks::run()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QDir userDir =
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    qDebug() << " out debug" << userDir;
    //    QString dataPath  =
    //    homeDir(userDir);

    db.setDatabaseName(userDir.absoluteFilePath("mybook.sqlite3"));

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return;
    }

    QSqlQuery query(db);
    bool success = query.exec("PRAGMA journal_mode = WAL;");
    if (!success) {
        // 处理设置日志模式失败的情况
        db.close();
        return;
    }

    if (tag != "") {
        query.prepare(
            "SELECT * FROM book where tags like :tags ORDER BY last_read_time DESC,book_name "
            "DESC");
        query.bindValue(":tags", "%" + tag + "%");
        qDebug() << "like find.";
    } else {
        query.prepare("SELECT * FROM book ORDER BY last_read_time DESC,book_name "
                      "DESC");
    }

    if (!query.exec()) {
        qDebug() << "Failed to query book_list:" << query.lastError().text();
        db.close();
        return;
    }
    QJsonArray tagsArr;
    while (query.next()) {
        QJsonObject obj;
        obj.insert("id", query.value("id").toString());
        obj.insert("book_name", query.value("book_name").toString());
        obj.insert("lang", query.value("lang").toString());
        obj.insert("auther", query.value("auther").toString());
        obj.insert("book_url", query.value("book_url").toString());

        obj.insert("last_read_time",
                   query.value("last_read_time").toDateTime().toString(Qt::ISODate));
        //        qDebug() << "-------- " << query.value("id").toString() <<
        //        query.value("book_name").toString();
        QString tagN = query.value("tags").toString();
        obj.insert("tags",tagN);

        QList tagItem = tagN.split(",");

        // 从BLOB字段读取图片
        QByteArray imgData = query.value("book_image").toByteArray();
        QBuffer buffer(&imgData);
        QImage image;
        image.load(&buffer, "PNG");
        // 序列化QImage到BASE64字符串
        QByteArray ba;
        QBuffer buf(&ba);
        image.save(&buf, "PNG");
        obj.insert("book_image", QString(ba.toBase64()));
        emit fetchBook(obj);

        for (int i = 0; i < tagItem.length(); i++) {
            if (QString(tagItem[i]) == "")
                continue;
            tagsArr.append(QJsonValue(tagItem[i]));
        }
    }
    if (tag == "") {
        emit tags(tagsArr);
    }
    db.close();

}
