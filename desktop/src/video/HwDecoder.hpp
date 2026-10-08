#pragma once
#include <QObject>
#include <QSize>
#include <functional>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/hwcontext.h>
#include <libavutil/imgutils.h>
}

namespace airwave::desktop {

class HwDecoder : public QObject {
    Q_OBJECT

public:
    using FrameDecodedCallback = std::function<void(AVFrame* frame, quint32 ptsUs)>;

    explicit HwDecoder(QObject *parent = nullptr);
    ~HwDecoder() override;

    bool init(AVCodecID codecId = AV_CODEC_ID_H264, AVHWDeviceType hwType = AV_HWDEVICE_TYPE_D3D11VA);
    void setFrameCallback(FrameDecodedCallback callback);
    bool decode(const uint8_t *data, int size, quint32 ptsUs);
    void flush();
    void release();

    AVHWDeviceType activeHwDeviceType() const { return m_hwType; }
    bool isHardwareAccelerated() const { return m_hwDeviceCtx != nullptr; }

private:
    static enum AVPixelFormat getHwFormat(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts);

    AVCodecContext *m_codecCtx{nullptr};
    AVBufferRef *m_hwDeviceCtx{nullptr};
    AVPacket *m_packet{nullptr};
    AVFrame *m_swFrame{nullptr};
    AVFrame *m_hwFrame{nullptr};
    AVHWDeviceType m_hwType{AV_HWDEVICE_TYPE_NONE};
    enum AVPixelFormat m_hwPixFmt{AV_PIX_FMT_NONE};
    FrameDecodedCallback m_callback;
};

} // namespace airwave::desktop
