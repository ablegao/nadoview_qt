#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
//#include <QtWebEngineQuick>

//#include "customurlschemehandler.h"
#include "myapplication.h"
#include "mybookimageschemehandler.h"
#include "networkmanager.h"
#include "openchatmodel.h"
#include "tableofcontent.h"
#include "userdata.h"
#include <QLocale>
#include <QTranslator>
#include <QDebug>

//#ifdef Q_OS_ANDROID
//#include <QJniObject>
//#endif


int main(int argc, char *argv[]) {

    //    QtWebEngineQuick::initialize();

    MyApplication app(argc, argv);

    //    QFont font("SimSun", 14);
    //    app.setFont(font);

    //    QWebEngineUrlScheme customScheme(QByteArrayLiteral("mybook"));
    //    QWebEngineUrlScheme::registerScheme(customScheme);
    TableOfContent book{};

    //    CustomUrlSchemeHandler *urlScheme = new CustomUrlSchemeHandler();
    //    urlScheme->book = &book;
    //    QWebEngineProfile *profile = QWebEngineProfile::defaultProfile();
    //    profile->installUrlSchemeHandler("mybook", urlScheme);

    MyBookImageSchemeHandler *scheme = new MyBookImageSchemeHandler();
    //    scheme = new (MyBookUrlSchemeHandler);
    scheme->book = &book;

    // TableOfContent 同时继承两个元素， 所以需要static_cast消除二义性
    //    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler("mybook",
    //                                                                 &scheme);

//#ifdef Q_OS_ANDROID

//    QJniObject activity = QJniObject::callStaticObjectMethod("org/qtproject/qt/android/QtNative",
//                                                             "activity",
//                                                             "()Landroid/app/Activity;");
//    QJniObject intent = activity.callObjectMethod("getIntent", "()Landroid/content/Intent;");
//    QJniObject action = intent.callObjectMethod("getAction", "()Ljava/lang/String;");
////    QJniObject contentResolver = activity.callObjectMethod("getContentResolver",
////                                                           "()Landroid/content/ContentResolver;");

//    if (action.toString() == "android.intent.action.VIEW") {
//        QJniObject data = intent.callObjectMethod("getData", "()Landroid/net/Uri;");
//        if (data.isValid()) {
//            // 获取文件路径
//            //            QJniObject data = data.callObjectMethod("getPath", "()Ljava/lang/String;");
//            //            QJniObject outobj = filePath.object();
//            QJniObject contentResolver
//                = activity.callObjectMethod("getContentResolver",
//                                            "()Landroid/content/ContentResolver;");
//            QJniObject inputStream
//                = contentResolver.callObjectMethod("openInputStream",
//                                                   "(Landroid/net/Uri;)Ljava/io/InputStream;",
//                                                   data.object());

////            jint fd = inputStream.callMethod<jint>("readAllBytes", "()I");
////            qDebug()  << "stream id :" << fd;

////            qDebug() << "get a file..." << filePath.toString();
////            jint fd = QJniObject::callStaticMethod<jint>("android/content/ContentResolver",
////                                                                "openFileDescriptor",
////                                                                "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;",
////                                                                filePath.object(),
////                                                                "r");
////            qDebug() << "GET ID:"  << fd;

////            QJniObject parcelFileDescriptor = contentResolver.callObjectMethod(
////                "openFileDescriptor",
////                "(Landroid/net/Uri;Ljava/lang/String;)Landroid/os/ParcelFileDescriptor;",
////                data.object(),
////                "r");
////            jint fd = parcelFileDescriptor.callMethod<jint>("getFd");

////            QJniObject os = QJniObject::callStaticObjectMethod(
////                "android/system/Os",
////                "open",
////                "(Ljava/lang/String;I)Ljava/io/FileDescriptor;",
////                "/proc/self/fd",
////                fd);
////            QJniObject realPath = os.callObjectMethod("readlink", "()Ljava/lang/String;");
////            QString path = realPath.toString();
////            qDebug() << path << "old..........";

//        }
//    }

//#endif



    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "nadoview_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
    QStringList args = QCoreApplication::arguments();
    qDebug() << "ARGS....." << args;

    //    qmlRegisterType<TableOfContent>("NadoView", 1, 0,
    //    "TableOfContentModel");
    qmlRegisterType<UserData>("NadoView", 1, 0, "UserData");

    // open chat model 注册
    qmlRegisterType<OpenChatModel>("NadoView", 1, 0, "OpenChatModel");
    qmlRegisterType<NetworkManager>("NadoView", 1, 0, "NetworkManager");
    // 初始化引擎
    QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("mybook"), scheme);

    QQmlContext *context = engine.rootContext();
    qDebug() << args;
    if (args.size() > 1) {
        context->setContextProperty("bookUrl", args[1]);
        qDebug() << "============ " << args[1];
//        book.openBook(args[1]);
    } else {
        context->setContextProperty("bookUrl", "");
    }
    //    QObject *bookAsQObject = qobject_cast<QObject *>(&book);

    engine.rootContext()->setContextProperty("tableOfContent", &book);

    const QUrl url(u"qrc:/nadoview/main.qml"_qs);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    // 链接文件打开事件
    QObject::connect(&app, &MyApplication::fileOpend, &book,
                     &TableOfContent::openBook);

    return app.exec();
}
