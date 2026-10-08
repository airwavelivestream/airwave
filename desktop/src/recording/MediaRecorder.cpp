#include "MediaRecorder.hpp"
#include <QDebug>

namespace airwave::desktop::recording {

MediaRecorder::MediaRecorder(QObject *parent)
    : QObject(parent)
{
}

MediaRecorder::~MediaRecorder() {
    stop();
}

bool MediaRecorder::start(const QString &outputPath, int width, int height, int fps) {
    if (m_recording.load()) return false;

    int ret = avformat_alloc_output_context2(&m_formatCtx, nullptr, nullptr, outputPath.toUtf8().constData());
    if (ret < 0 || !m_formatCtx) {
        qWarning() << "[MediaRecorder] Could not deduce output format from file extension";
        return false;
    }

    // Video stream passthrough (lossless re-mux without re-encoding to save CPU)
    m_videoStream = avformat_new_stream(m_formatCtx, nullptr);
    m_videoStream->id = 0;
    m_videoStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    m_videoStream->codecpar->codec_id = AV_CODEC_ID_H264;
    m_videoStream->codecpar->width = width;
    m_videoStream->codecpar->height = height;
    m_videoStream->time_base = {1, 1000000}; // Microseconds

    if (!(m_formatCtx->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&m_formatCtx->pb, outputPath.toUtf8().constData(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            qWarning() << "[MediaRecorder] Could not open output file:" << outputPath;
            return false;
        }
    }

    ret = avformat_write_header(m_formatCtx, nullptr);
    if (ret < 0) {
        qWarning() << "[MediaRecorder] Error writing format header";
        return false;
    }

    m_videoPtsBase = -1;
    m_bytesWritten.store(0);
    m_recording.store(true);
    emit recordingStateChanged(true);
    qInfo() << "[MediaRecorder] Started recording to:" << outputPath;
    return true;
}

void MediaRecorder::writeVideoPacket(const uint8_t *data, int size, quint32 ptsUs, bool isKeyframe) {
    if (!m_recording.load() || !m_formatCtx || !m_videoStream) return;

    if (m_videoPtsBase < 0) {
        m_videoPtsBase = ptsUs;
    }

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = const_cast<uint8_t*>(data);
    pkt.size = size;
    pkt.stream_index = m_videoStream->index;
    pkt.pts = ptsUs - m_videoPtsBase;
    pkt.dts = pkt.pts;
    if (isKeyframe) {
        pkt.flags |= AV_PKT_FLAG_KEY;
    }

    av_interleaved_write_frame(m_formatCtx, &pkt);
    m_bytesWritten.fetch_add(size);
    emit statsUpdated();
}

void MediaRecorder::writeAudioPacket(const uint8_t *data, int size, quint32 ptsUs) {
    // Interleave audio packets directly into muxer
}

void MediaRecorder::stop() {
    if (m_recording.exchange(false)) {
        if (m_formatCtx) {
            av_write_trailer(m_formatCtx);
            if (!(m_formatCtx->oformat->flags & AVFMT_NOFILE)) {
                avio_closep(&m_formatCtx->pb);
            }
            avformat_free_context(m_formatCtx);
            m_formatCtx = nullptr;
        }
        emit recordingStateChanged(false);
        qInfo() << "[MediaRecorder] Recording stopped and finalized.";
    }
}

} // namespace airwave::desktop::recording
