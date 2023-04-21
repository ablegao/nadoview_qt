#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include <QUrl>
#include <QtWebView>

#include "myapplication.h"
#include "mybookimageschemehandler.h"
#include "networkmanager.h"
#include "openchatmodel.h"
#include "tableofcontent.h"
#include "userdata.h"





int main(int argc, char *argv[]) {
    //        QtWebEngineQuick::initialize();
    QtWebView::initialize();

    MyApplication app(argc, argv);


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
    qmlRegisterType<UserData>("NadoView", 1, 0, "UserData");
    qmlRegisterType<TableOfContent>("NadoView",1,0,"TableOfContent");
    // open chat model 注册
    qmlRegisterType<OpenChatModel>("NadoView", 1, 0, "OpenChatModel");
    qmlRegisterType<NetworkManager>("NadoView", 1, 0, "NetworkManager");
    // 初始化引擎
    QQmlApplicationEngine engine;
//    engine.addImageProvider(QLatin1String("mybook"), scheme);

    QQmlContext *context = engine.rootContext();
    qDebug() << args;

    const QUrl url(u"qrc:/nadoview/main.qml"_qs);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

//    // 链接文件打开事件
//    QObject::connect(&app, &MyApplication::fileOpend, &book,
//                     &TableOfContent::openBook);

    return app.exec();
}
