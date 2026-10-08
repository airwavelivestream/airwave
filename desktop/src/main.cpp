#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "protocol/AwtpReceiver.hpp"
#include "video/HwDecoder.hpp"
#include "video/VideoRendererItem.hpp"
#include "stats/LatencyTracker.hpp"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("Airwave");
    app.setOrganizationName("Airwave");

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

    const QUrl url(u"qrc:/qml/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);
    receiver->start();

    return app.exec();
}
