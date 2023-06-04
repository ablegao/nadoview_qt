#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QUrl>
#include <QtWebView>
//#include <QtWebEngineQuick>
#include "QtQml/qqml.h"
#include "myapplication.h"
#include "myappsingle.h"
#include "openchatmodel.h"
#include "tableofcontent.h"
//#include "macos.h"

//#include "transfer.h"
#include "userdata.h"

int main(int argc, char *argv[]) {
//    QtWebEngineQuick::initialize();
    QtWebView::initialize();

    MyApplication app(argc, argv);
    MyAppSingle appSingle;

    //    TableOfContent book{};
    //    MyBookImageSchemeHandler *scheme = new MyBookImageSchemeHandler();
    //    scheme->book = &book;

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
    qmlRegisterType<UserData>("com.tap2happy.nadoview", 1, 0, "UserData");
    qmlRegisterType<TableOfContent>("com.tap2happy.nadoview", 1, 0,
                                    "TableOfContent");
    // open chat model 注册
    qmlRegisterType<OpenChatModel>("com.tap2happy.nadoview", 1, 0,
                                   "OpenChatModel");
//    qmlRegisterType<Transfer>("com.tap2happy.nadoview", 1, 0, "AwsTransfer");
    //    qmlRegisterSingletonType<MyApplication>("NadoView",1,0,"FileOpend",
    //    MyApplication::fileOpend);
    // 初始化引擎
    QQmlApplicationEngine engine;
    //    engine.addImageProvider(QLatin1String("mybook"), scheme);

    QQmlContext *context = engine.rootContext();
    qDebug() << args;

    const QUrl url(u"qrc:/nadoview/main.qml"_qs);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    //    engine.rootContext()->setContextProperty("appSingle",appSingle);
    engine.rootContext()->setContextProperty("appSingle", &appSingle);

    // 链接文件打开事件
    QObject::connect(&app, &MyApplication::fileOpend, &appSingle,
                     &MyAppSingle::fileOpend);

    engine.load(url);

    if (args.size() > 1) {
        emit appSingle.fileOpend(args[1]);
    } else {
        emit appSingle.noOpenFile();
    }
//    MacOS::removeTitlebarFromWindow();

    return app.exec();
}
