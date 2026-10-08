#pragma once
#include <QObject>
#include <QString>
#include <memory>
#include <atomic>

extern "C" {
#include <libavformat/avformat.h>
}

namespace airwave::desktop::streaming {

enum class StreamOutputProtocol {
    LocalSrt,   // srt://127.0.0.1:9000 (Ideal zero-driver OBS ingestion)
    LocalUdp,   // udp://127.0.0.1:9001
    DirectRtmp  // rtmp://a.rtmp.youtube.com/live2/... or rtmp://live.twitch.tv/app/...
};

class StreamRelay : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isStreaming READ isStreaming NOTIFY streamingStateChanged)
    Q_PROPERTY(qint64 streamedBytes READ streamedBytes NOTIFY statsUpdated)

public:
    explicit StreamRelay(QObject *parent = nullptr);
    ~StreamRelay() override;

    bool start(const QString &targetUrl, int width = 1920, int height = 1080);
    void pushVideoChunk(const uint8_t *data, int size, quint32 ptsUs, bool isKeyframe);
    void stop();

    bool isStreaming() const { return m_streaming.load(); }
    qint64 streamedBytes() const { return m_streamedBytes.load(); }

signals:
    void streamingStateChanged(bool streaming);
    void statsUpdated();

private:
    std::atomic<bool> m_streaming{false};
    std::atomic<qint64> m_streamedBytes{0};
    AVFormatContext *m_outFormatCtx{nullptr};
    AVStream *m_outStream{nullptr};
    qint64 m_ptsBase{-1};
};

} // namespace airwave::desktop::streaming
