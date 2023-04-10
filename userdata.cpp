#include "userdata.h"

UserData::UserData(QObject *parent) : QObject{parent} {
    QDir userDir =
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    qDebug() << " out debug" << userDir;
    //    QString dataPath  =
    //    homeDir(userDir);
    dataFile = userDir.absoluteFilePath("mybook.sqlite3");

    qDebug() << dataFile;
    if (!QFile::exists(dataFile)) {
        qDebug() << "初始化文件";

        // 如果库文件不存在，创建库文件和表
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dataFile);
        // 打开数据库
        if (!db.open()) {
            qDebug() << "Failed to open database:" << db.lastError().text();
            return;
        }

        // 创建book表
        QSqlQuery query(db);
        if (!query.exec("CREATE TABLE book ("
                        "id TEXT PRIMARY KEY,"
                        "name TEXT,"
                        "last_read_index INTEGER,"
                        "last_read_scroll_number INTEGER,"
                        "last_read_time DATETIME"
                        ")")) {
            qDebug() << "Failed to create book table:"
                     << query.lastError().text();
            return;
        }

        // 创建write_label表
        if (!query.exec("CREATE TABLE write_label ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "book_id TEXT,"
                        "read_index INTEGER,"
                        "label_source TEXT,"
                        "label_color TEXT,"
                        "label_postil TEXT,"
                        "FOREIGN KEY(book_id) REFERENCES book(id)"
                        ")")) {
            qDebug() << "Failed to create write_label table:"
                     << query.lastError().text();
            return;
        }
    } else {
        // 读取数据库文件
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dataFile);
    }
}

QString UserData::md5(const QString &str) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(str.toUtf8());
    return QString(hash.result().toHex());
}

QJsonObject UserData::openBook(const QString &bookName) {
    // 连接数据库
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    // 查询书籍记录
    QString bookId = md5(bookName);
    query.prepare("SELECT * FROM book WHERE id = :id");
    query.bindValue(":id", bookId);
    if (!query.exec()) {
        qDebug() << "Failed to query book record:" << query.lastError().text();
        return QJsonObject();
    }

    // 获取查询结果
    if (!query.next()) {
        // 如果记录不存在，则创建一条记录
        query.prepare("INSERT INTO book (id, name, last_read_index, "
                      "last_read_scroll_number, last_read_time) "
                      "VALUES (:id, :name, :last_read_file, "
                      ":last_read_scroll_number, :last_read_time)");
        query.bindValue(":id", bookId);
        query.bindValue(":name", bookName);
        query.bindValue(":last_read_INTEGER", 0);
        query.bindValue(":last_read_scroll_number", 0);
        query.bindValue(":last_read_time", QDateTime::currentDateTime());
        if (!query.exec()) {
            qDebug() << "Failed to create book record:"
                     << query.lastError().text();
            return QJsonObject();
        }

        // 构造QJsonArray输出
        QJsonObject obj;
        obj.insert("id", QJsonValue(bookId));
        obj.insert("name", QJsonValue(bookName));
        obj.insert("last_read_index", QJsonValue(0));
        obj.insert("last_read_scroll_number", QJsonValue(0));
        obj.insert(
            "last_read_time",
            QJsonValue(QDateTime::currentDateTime().toString(Qt::ISODate)));
        return obj;
    } else {
        // 如果记录存在，则构造QJsonArray输出
        QJsonObject obj;
        obj.insert("id", QJsonValue(query.value("id").toString()));
        obj.insert("name", QJsonValue(query.value("name").toString()));
        obj.insert("last_read_index",
                   QJsonValue(query.value("last_read_index").toInt()));
        obj.insert("last_read_scroll_number",
                   QJsonValue(query.value("last_read_scroll_number").toInt()));
        obj.insert("last_read_time", QJsonValue(query.value("last_read_time")
                                                    .toDateTime()
                                                    .toString(Qt::ISODate)));
        return obj;
    }
}

// 保存书籍的阅读记录并获取labelHistory
void UserData::read(const QString &bookName, int last_read_index,
                    int last_read_scroll_number) {
    // 连接数据库
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    // 查询书籍记录
    QString bookId = md5(bookName);
    query.prepare("SELECT last_read_index FROM book WHERE id = :id");
    query.bindValue(":id", bookId);
    if (!query.exec()) {
        qDebug() << "Failed to query book record:" << query.lastError().text();
        return;
    }

    // 获取上一次阅读的文件名
    int lastReadIndex;
    if (query.next()) {
        lastReadIndex = query.value("last_read_index").toInt();
    }

    // 如果与上一次阅读的文件名不同，则获取labelHistory并发送信号
    if (lastReadIndex != last_read_index) {
        QSqlQuery labelQuery(db);
        labelQuery.prepare("SELECT * FROM write_label WHERE book_id = :book_id "
                           "AND read_index = :read_index");
        labelQuery.bindValue(":book_id", bookId);
        labelQuery.bindValue(":read_index", last_read_index);
        if (labelQuery.exec()) {
            QJsonArray labelHistory;
            while (labelQuery.next()) {
                QJsonObject obj;
                obj.insert("id", QJsonValue(labelQuery.value("id").toInt()));
                obj.insert("book_id",
                           QJsonValue(labelQuery.value("book_id").toString()));
                obj.insert("read_index",
                           QJsonValue(labelQuery.value("read_index").toInt()));
                obj.insert(
                    "label_source",
                    QJsonValue(labelQuery.value("label_source").toString()));
                obj.insert(
                    "label_color",
                    QJsonValue(labelQuery.value("label_color").toString()));
                obj.insert(
                    "label_postil",
                    QJsonValue(labelQuery.value("label_postil").toString()));
                labelHistory.append(QJsonValue(obj));
            }
            emit readPageChanged(labelHistory);
        } else {
            qDebug() << "Failed to query write_label records:"
                     << labelQuery.lastError().text();
        }
    }

    // 更新书籍记录
    query.prepare("UPDATE book SET last_read_index = :last_read_index, "
                  "last_read_scroll_number = :last_read_scroll_number, "
                  "last_read_time = :last_read_time WHERE id = :id");
    query.bindValue(":id", bookId);
    query.bindValue(":last_read_index", last_read_index);
    query.bindValue(":last_read_scroll_number", last_read_scroll_number);
    query.bindValue(":last_read_time", QDateTime::currentDateTime());
    if (!query.exec()) {
        qDebug() << "Failed to update book record:" << query.lastError().text();
    }
}
