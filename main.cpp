#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtWebEngineQuick>

#include "customurlschemehandler.h"
#include "myapplication.h"
#include "mybookimageschemehandler.h"
#include "openchatmodel.h"
#include "tableofcontent.h"
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {

    QtWebEngineQuick::initialize();

    MyApplication app(argc, argv);

    //    QFont font("SimSun", 14);
    //    app.setFont(font);

    QWebEngineUrlScheme customScheme(QByteArrayLiteral("mybook"));
    QWebEngineUrlScheme::registerScheme(customScheme);
    TableOfContent book{};

    CustomUrlSchemeHandler *urlScheme = new CustomUrlSchemeHandler();
    urlScheme->book = &book;
    QWebEngineProfile *profile = QWebEngineProfile::defaultProfile();
    profile->installUrlSchemeHandler("mybook", urlScheme);

    MyBookImageSchemeHandler *scheme = new MyBookImageSchemeHandler();
    //    scheme = new (MyBookUrlSchemeHandler);
    scheme->book = &book;

    // TableOfContent 同时继承两个元素， 所以需要static_cast消除二义性
    //    QWebEngineProfile::defaultProfile()->installUrlSchemeHandler("mybook",
    //                                                                 &scheme);

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

    //    qmlRegisterType<TableOfContent>("NadoView", 1, 0,
    //    "TableOfContentModel");

    // open chat model 注册
    qmlRegisterType<OpenChatModel>("NadoView", 1, 0, "OpenChatModel");
    // 初始化引擎
    QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("mybook"), scheme);

    QQmlContext *context = engine.rootContext();
    if (args.size() > 1) {
        context->setContextProperty("bookUrl", args[1]);
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
    QObject::connect(&app, &MyApplication::FileOpend, &book,
                     &TableOfContent::openBook);

    return app.exec();
}
