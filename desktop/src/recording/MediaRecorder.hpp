#pragma once
#include <QObject>
#include <QString>
#include <memory>
#include <atomic>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

namespace airwave::desktop::recording {

class MediaRecorder : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isRecording READ isRecording NOTIFY recordingStateChanged)
    Q_PROPERTY(qint64 recordedBytes READ recordedBytes NOTIFY statsUpdated)

public:
    explicit MediaRecorder(QObject *parent = nullptr);
    ~MediaRecorder() override;

    bool start(const QString &outputPath, int width = 1920, int height = 1080, int fps = 60);
    void writeVideoPacket(const uint8_t *data, int size, quint32 ptsUs, bool isKeyframe);
    void writeAudioPacket(const uint8_t *data, int size, quint32 ptsUs);
    void stop();

    bool isRecording() const { return m_recording.load(); }
    qint64 recordedBytes() const { return m_bytesWritten.load(); }

signals:
    void recordingStateChanged(bool recording);
    void statsUpdated();

private:
    std::atomic<bool> m_recording{false};
    std::atomic<qint64> m_bytesWritten{0};
    AVFormatContext *m_formatCtx{nullptr};
    AVStream *m_videoStream{nullptr};
    AVStream *m_audioStream{nullptr};
    qint64 m_videoPtsBase{-1};
};

} // namespace airwave::desktop::recording
