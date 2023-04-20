#include "userdata.h"



UserData::UserData(QObject *parent) : QObject{parent} {
    QDir userDir =
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    qDebug() << " out debug" << userDir;
    //    QString dataPath  =
    //    homeDir(userDir);
    dataFile = userDir.absoluteFilePath("mybook.sqlite3");

    qDebug() << "data file:::" << dataFile;
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
                        "book_name TEXT,"
                        "auther TEXT,"
                        "book_url TEXT,"
                        "book_image BLOB,"
                        "lang TEXT,"
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

//        if (!query.exec("CREATE TABLE book_list ("
//                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
//                        "book_id TEXT,"
//                        "book_name TEXT,"
//                        "book_url TEXT,"
//                        "book_image BLOB,"
//                        "last_read_time DATETIME," // 新增的时间字段
//                        "FOREIGN KEY(book_id) REFERENCES book(id)"
//                        ")")) {
//            qDebug() << "Failed to create book_list table:" << query.lastError().text();
//            return;
//        }

        // 创建settings表
        if (!query.exec("CREATE TABLE settings (json_settings TEXT)")) {
            qDebug() << "Failed to create settings table:" << query.lastError().text();
            return;
        }

        query.prepare("SELECT * FROM settings");
        if (!query.exec()) {
            qDebug() << "Failed to query settings:" << query.lastError().text();
            return;
        }

        // 如果settings表为空,初始化数据
        if (!query.next()) {
            query.prepare("INSERT INTO settings (json_settings) VALUES (:data)");
            QJsonObject obj{{"book_dirs", QJsonArray{QDir::homePath()+"/Books","/storage/emulated/0/Download"}}, {"font_size", 16}};
            query.bindValue(":data", QJsonDocument(obj).toJson());
            if (!query.exec()) {
                qDebug() << "Failed to init settings:" << query.lastError().text();
                return;
            }
        }
        bookSearch();
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


//    // 如果记录存在,更新last_read_time字段
//    query.prepare("UPDATE book_list SET last_read_time = :last_read_time WHERE book_id = :book_id");
//    query.bindValue(":last_read_time", QDateTime::currentDateTime());
//    query.bindValue(":book_id", bookId);
//    if (!query.exec()) {
//        qDebug() << "Failed to update book_list record:" << query.lastError().text();
//    }


    query.prepare("SELECT * FROM book WHERE id = :id");
    query.bindValue(":id", bookId);
    if (!query.exec()) {
        qDebug() << "Failed to query book record:" << query.lastError().text();
        return QJsonObject();
    }

    // 获取查询结果
    if (query.next()) {

        // 如果记录存在，则构造QJsonArray输出
        QJsonObject obj;
        obj.insert("id", QJsonValue(query.value("id").toString()));
        obj.insert("book_name", QJsonValue(query.value("book_name").toString()));
        obj.insert("book_url", QJsonValue(query.value("book_url").toString()));
        obj.insert("last_read_index",
                   QJsonValue(query.value("last_read_index").toInt()));
        obj.insert("last_read_scroll_number",
                   QJsonValue(query.value("last_read_scroll_number").toInt()));
        obj.insert("last_read_time", QJsonValue(query.value("last_read_time")
                                                    .toDateTime()
                                                    .toString(Qt::ISODate)));
        return obj;
    }
    return QJsonObject();
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


QJsonArray UserData::books(int count)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    // 查询book_list表,按last_read_time降序排序
    if (count > 0) {
        query.prepare(QStringLiteral("SELECT * FROM book ORDER BY last_read_time DESC,book_name DESC LIMIT %1").arg(count));
    }else{
        query.prepare("SELECT * FROM book ORDER BY last_read_time DESC,book_name DESC");
    }
    if (!query.exec()) {
        qDebug() << "Failed to query book_list:" << query.lastError().text();
        return QJsonArray();
    }

    // 构造QJsonArray输出
    QJsonArray array;
    while (query.next()) {
        QJsonObject obj;
        obj.insert("id", query.value("id").toString());
        obj.insert("book_name", query.value("book_name").toString());
        obj.insert("lang", query.value("lang").toString());
        obj.insert("auther", query.value("auther").toString());
        obj.insert("book_url", query.value("book_url").toString());
        obj.insert("last_read_time",
                   query.value("last_read_time").toDateTime().toString(Qt::ISODate));
//        qDebug() << "-------- " << query.value("id").toString() << query.value("book_name").toString();

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

        array.append(obj);
    }

    return array;
}
void UserData::addBook(const QString &bookUrl)
{
    EpubBook *book = new EpubBook();
    book->parseBook(bookUrl);
    if (book->title == "")
        return;

    QSqlDatabase db = QSqlDatabase::database();

    // 检查book_id是否存在,如果存在只更新book_url
    QString bookId = md5(book->title);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM book WHERE id = :book_id");
    query.bindValue(":book_id", bookId);
    if (query.exec() && query.next()) {
        // 更新book_url
        query.prepare("UPDATE book SET book_url = :book_url WHERE id = :book_id");
        query.bindValue(":book_url", bookUrl);
        if (!query.exec()) {
            qDebug() << "Failed to update book_list:" << query.lastError().text();
        }
    } else {
        if (book->title == "") {
            qDebug() << "IS empty title book" << book->title << bookUrl;
            return;
        }
        // 插入新记录
        query.prepare(
            "INSERT INTO book (id, book_name, book_url, book_image)VALUES "
            "(:id, :book_name, :book_url, :book_image)");
        // ... (绑定其他字段值的代码)
        query.bindValue(":id", bookId);
        query.bindValue(":book_name", book->title);
        query.bindValue(":book_url", bookUrl);
        //                    qDebug() << "search epub of name:" << book->title;

        QByteArray imgArr = book->openFileByUrl(book->coverImg);
        if (imgArr.size() > 0) {
            // 从QByteArray构造QImage
            QImage image;
            image.loadFromData(imgArr);
            // 将QImage转换为jpeg格式
            // 将QImage转换为jpeg格式
            //                    QImage jpegImage = image.convertToFormat();

            // 获取图片大小
            int width = image.width();
            int height = image.height();

            // 如果宽度大于200,计算缩放比例并缩放
            if (width > 200) {
                double scale = 200.0 / width;
                width = 200;
                height = height * scale;

                // 缩放QImage
                image = image.scaled(width, height, Qt::KeepAspectRatio);
            }
            // 将QImage保存为QByteArray
            QByteArray jpegData;
            QBuffer buf(&jpegData);
            image.save(&buf, "png");

            query.bindValue(":book_image", jpegData);
        }
        if (!query.exec()) {
            qDebug() << "Failed to insert book_list record:" << query.lastError().text();
        }


        delete book;
    }
}
void UserData::bookSearch()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.prepare("SELECT json_settings FROM settings");
    if (!query.exec()) {
        qDebug() << "Failed to query settings:" << query.lastError().text();
        return;
    }


    if (query.next()) {
        QJsonDocument doc = QJsonDocument::fromJson(query.value(0).toString().toUtf8());
        QJsonObject obj = doc.object();
        QJsonArray dirs = obj["book_dirs"].toArray();
        qDebug() << "book dirs" << dirs;
        QStringList filter;
        // 过滤得到文本文件(.txt, .epub, .mobi等)
        filter << "*.epub"; //<< "*.txt" << "*.epub" << "*.mobi" << "*.azw3";

        // 遍历book_dirs中的文件夹路径
        for (int i = 0; i < dirs.size(); i++) {
            QString dir = dirs[i].toString();

            QDir directory(dir);
            directory = directory.absolutePath();
//            qDebug() << " serach dir ..." << directory;

            // 获取文件夹下所有的文件
            QStringList files = directory.entryList(filter,QDir::Files);
            qDebug() << "files " << files.size();

            for (const QString &f : files) {

                QString bookUrl = directory.absoluteFilePath(f);
                addBook(bookUrl);
            // 对文本文件进行进一步处理,存入book_list表等
            }
            QStringList subDirs = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (const QString &subDir : subDirs) {
                QDir subDirPath = directory.absoluteFilePath(subDir);
                QStringList subFiles = subDirPath.entryList(filter, QDir::Files);
                for (const QString &f : files) {
                    QString bookUrl = subDirPath.absoluteFilePath(f);
                    addBook(bookUrl);
                }
            }
        }
    }
}


bool UserData::checkPermission(const QString &permission)
{
#ifdef Q_OS_ANDROID
    auto result = QtAndroidPrivate::checkPermission(permission);

    return result.result() != QtAndroidPrivate::Denied;
#endif
    return true;
}

bool UserData::requestPermission(const QString &permission)
{
#ifdef Q_OS_ANDROID
    auto result = QtAndroidPrivate::requestPermission(permission);
//                      .then([](QtAndroidPrivate::PermissionResult result) { return result; });
//    result.waitForFinished();
    auto out = result.result();
    qDebug() << "request out .:::::" << out ;
    return out != QtAndroidPrivate::Denied;
#endif
    return true;
}
