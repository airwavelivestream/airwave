#include "HwDecoder.hpp"
#include <QDebug>

namespace airwave::desktop {

static enum AVPixelFormat g_targetHwPixFmt = AV_PIX_FMT_NONE;

enum AVPixelFormat HwDecoder::getHwFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts) {
    Q_UNUSED(ctx);
    const enum AVPixelFormat *p;
    for (p = pix_fmts; *p != -1; p++) {
        if (*p == g_targetHwPixFmt) {
            return *p;
        }
    }
    qWarning() << "[HwDecoder] Target hardware pixel format not found, falling back to software decode";
    return AV_PIX_FMT_NONE;
}

HwDecoder::HwDecoder(QObject *parent)
    : QObject(parent),
      m_packet(av_packet_alloc()),
      m_swFrame(av_frame_alloc()),
      m_hwFrame(av_frame_alloc())
{
}

HwDecoder::~HwDecoder() {
    release();
    if (m_packet) av_packet_free(&m_packet);
    if (m_swFrame) av_frame_free(&m_swFrame);
    if (m_hwFrame) av_frame_free(&m_hwFrame);
}

bool HwDecoder::init(AVCodecID codecId, AVHWDeviceType hwType) {
    release();
    m_hwType = hwType;

    const AVCodec *codec = avcodec_find_decoder(codecId);
    if (!codec) {
        qWarning() << "[HwDecoder] Decoder not found for codec ID:" << codecId;
        return false;
    }

    m_codecCtx = avcodec_alloc_context3(codec);
    if (!m_codecCtx) return false;

    // Enable ultra-low-latency flags: zero-delay, single frame multithreading or slice
    m_codecCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;
    m_codecCtx->flags2 |= AV_CODEC_FLAG2_FAST;
    m_codecCtx->thread_count = 1; // 1 thread avoids frame buffering latency

    // Hardware acceleration setup
    if (hwType != AV_HWDEVICE_TYPE_NONE) {
        int err = av_hwdevice_ctx_create(&m_hwDeviceCtx, hwType, nullptr, nullptr, 0);
        if (err == 0) {
            m_codecCtx->hw_device_ctx = av_buffer_ref(m_hwDeviceCtx);
            m_hwPixFmt = (hwType == AV_HWDEVICE_TYPE_D3D11VA) ? AV_PIX_FMT_D3D11 :
                         (hwType == AV_HWDEVICE_TYPE_CUDA) ? AV_PIX_FMT_CUDA :
                         (hwType == AV_HWDEVICE_TYPE_VIDEOTOOLBOX) ? AV_PIX_FMT_VIDEOTOOLBOX : AV_PIX_FMT_VAAPI;
            g_targetHwPixFmt = m_hwPixFmt;
            m_codecCtx->get_format = HwDecoder::getHwFormat;
            qInfo() << "[HwDecoder] Hardware acceleration initialized successfully for:" << av_hwdevice_get_type_name(hwType);
        } else {
            qWarning() << "[HwDecoder] Failed to init hardware context, fallback to CPU";
            m_hwDeviceCtx = nullptr;
        }
    }

    if (avcodec_open2(m_codecCtx, codec, nullptr) < 0) {
        qWarning() << "[HwDecoder] Failed to open codec context";
        return false;
    }

    return true;
}

void HwDecoder::setFrameCallback(FrameDecodedCallback callback) {
    m_callback = std::move(callback);
}

bool HwDecoder::decode(const uint8_t *data, int size, quint32 ptsUs) {
    if (!m_codecCtx || !data || size <= 0) return false;

    m_packet->data = const_cast<uint8_t*>(data);
    m_packet->size = size;
    m_packet->pts = ptsUs;

    int ret = avcodec_send_packet(m_codecCtx, m_packet);
    if (ret < 0) {
        return false;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(m_codecCtx, m_hwFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            return false;
        }

        AVFrame *renderTargetFrame = m_hwFrame;
        // If hardware frame and consumer needs system memory or staging:
        if (m_hwFrame->format == m_hwPixFmt) {
            // Can be rendered directly via zero-copy D3D11 / Metal texture view
            renderTargetFrame = m_hwFrame;
        }

        if (m_callback) {
            m_callback(renderTargetFrame, static_cast<quint32>(m_hwFrame->pts));
        }
    }

    return true;
}

void HwDecoder::flush() {
    if (m_codecCtx) {
        avcodec_flush_buffers(m_codecCtx);
    }
}

void HwDecoder::release() {
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
    }
    if (m_hwDeviceCtx) {
        av_buffer_unref(&m_hwDeviceCtx);
        m_hwDeviceCtx = nullptr;
    }
}

} // namespace airwave::desktop
