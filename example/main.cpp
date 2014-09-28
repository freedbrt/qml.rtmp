#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QPluginLoader>
#include <QQmlExtensionPlugin>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

#ifdef QT_DEBUG
    QPluginLoader loader("d:\\dev\\rtmp\\proj\\qml_extension\\build-streamer-Qt_5_2_1_mingw32-Debug\\lib\\qml.rtmpd.dll");
#else
    QPluginLoader loader(QCoreApplication::applicationDirPath() + QDir::separator() + "qml.rtmpd.dll");
#endif

    QQmlExtensionPlugin *plugin = qobject_cast<QQmlExtensionPlugin *>(loader.instance());
    if (plugin)
        plugin->registerTypes("qml.rtmp");
    else {
        qDebug() << loader.errorString();
        QCoreApplication::quit();
    }

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
