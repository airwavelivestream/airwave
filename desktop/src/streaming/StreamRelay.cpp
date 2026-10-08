#include "StreamRelay.hpp"
#include <QDebug>

namespace airwave::desktop::streaming {

StreamRelay::StreamRelay(QObject *parent)
    : QObject(parent)
{
}

StreamRelay::~StreamRelay() {
    stop();
}

bool StreamRelay::start(const QString &targetUrl, int width, int height) {
    if (m_streaming.load()) return false;

    const char *formatName = "flv";
    if (targetUrl.startsWith("srt://") || targetUrl.startsWith("udp://")) {
        formatName = "mpegts";
    }

    int ret = avformat_alloc_output_context2(&m_outFormatCtx, nullptr, formatName, targetUrl.toUtf8().constData());
    if (ret < 0 || !m_outFormatCtx) {
        qWarning() << "[StreamRelay] Failed to allocate format context for:" << targetUrl;
        return false;
    }

    m_outStream = avformat_new_stream(m_outFormatCtx, nullptr);
    m_outStream->id = 0;
    m_outStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    m_outStream->codecpar->codec_id = AV_CODEC_ID_H264;
    m_outStream->codecpar->width = width;
    m_outStream->codecpar->height = height;
    m_outStream->time_base = {1, 1000000};

    // Open connection to local OBS pipe or remote RTMP server
    ret = avio_open(&m_outFormatCtx->pb, targetUrl.toUtf8().constData(), AVIO_FLAG_WRITE);
    if (ret < 0) {
        qWarning() << "[StreamRelay] Could not connect to streaming endpoint:" << targetUrl;
        return false;
    }

    ret = avformat_write_header(m_outFormatCtx, nullptr);
    if (ret < 0) {
        qWarning() << "[StreamRelay] Could not write stream header";
        return false;
    }

    m_ptsBase = -1;
    m_streamedBytes.store(0);
    m_streaming.store(true);
    emit streamingStateChanged(true);
    qInfo() << "[StreamRelay] Live streaming output active ->" << targetUrl;
    return true;
}

void StreamRelay::pushVideoChunk(const uint8_t *data, int size, quint32 ptsUs, bool isKeyframe) {
    if (!m_streaming.load() || !m_outFormatCtx || !m_outStream) return;

    if (m_ptsBase < 0) {
        m_ptsBase = ptsUs;
    }

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = const_cast<uint8_t*>(data);
    pkt.size = size;
    pkt.stream_index = m_outStream->index;
    pkt.pts = ptsUs - m_ptsBase;
    pkt.dts = pkt.pts;
    if (isKeyframe) {
        pkt.flags |= AV_PKT_FLAG_KEY;
    }

    av_interleaved_write_frame(m_outFormatCtx, &pkt);
    m_streamedBytes.fetch_add(size);
    emit statsUpdated();
}

void StreamRelay::stop() {
    if (m_streaming.exchange(false)) {
        if (m_outFormatCtx) {
            av_write_trailer(m_outFormatCtx);
            if (!(m_outFormatCtx->oformat->flags & AVFMT_NOFILE)) {
                avio_closep(&m_outFormatCtx->pb);
            }
            avformat_free_context(m_outFormatCtx);
            m_outFormatCtx = nullptr;
        }
        emit streamingStateChanged(false);
        qInfo() << "[StreamRelay] Live streaming terminated.";
    }
}

} // namespace airwave::desktop::streaming
