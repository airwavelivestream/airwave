#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QFile>
#include <QDir>
#include <QDebug>
#include "protocol/AwtpReceiver.hpp"
#include "video/HwDecoder.hpp"
#include "video/VideoRendererItem.hpp"
#include "stats/LatencyTracker.hpp"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("Airwave");
    app.setOrganizationName("Airwave");

    QQuickStyle::setStyle("Basic");

    qmlRegisterType<airwave::desktop::VideoRendererItem>("Airwave", 1, 0, "VideoRendererItem");

    auto receiver = std::make_unique<airwave::desktop::AwtpReceiver>();
    auto decoder = std::make_unique<airwave::desktop::HwDecoder>();
    auto latencyTracker = std::make_unique<airwave::desktop::LatencyTracker>();

    decoder->init(AV_CODEC_ID_H264, AV_HWDEVICE_TYPE_D3D11VA);

    // Bind Network datagrams directly to the hardware decoder
    QObject::connect(receiver.get(), &airwave::desktop::AwtpReceiver::videoPacketReady,
                     [&](const QByteArray &data, quint32 ptsUs, bool) {
        decoder->decode(reinterpret_cast<const uint8_t*>(data.constData()), data.size(), ptsUs);
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("receiver", receiver.get());
    engine.rootContext()->setContextProperty("latencyTracker", latencyTracker.get());

    // Try loading from embedded QRC first, then from application directory if running standalone
    QUrl url(u"qrc:/qml/main.qml"_qs);
    if (!QFile::exists(u":/qml/main.qml"_qs)) {
        QString localQml = QDir(app.applicationDirPath()).filePath("qml/main.qml");
        if (QFile::exists(localQml)) {
            url = QUrl::fromLocalFile(localQml);
        }
    }

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "Failed to create root QML object for URL:" << objUrl;
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);
    if (engine.rootObjects().isEmpty()) {
        // Fallback: try direct local path if QRC lookup delayed
        QString fallbackPath = QDir(app.applicationDirPath()).filePath("qml/main.qml");
        if (QFile::exists(fallbackPath)) {
            engine.load(QUrl::fromLocalFile(fallbackPath));
        }
    }

    receiver->start();

    return app.exec();
}
