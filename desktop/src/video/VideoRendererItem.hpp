#pragma once
#include <QQuickItem>
#include <QImage>
#include <QMutex>

namespace airwave::desktop {

class VideoRendererItem : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(int videoWidth READ videoWidth NOTIFY videoDimensionsChanged)
    Q_PROPERTY(int videoHeight READ videoHeight NOTIFY videoDimensionsChanged)
    Q_PROPERTY(qreal renderedFps READ renderedFps NOTIFY fpsChanged)

public:
    explicit VideoRendererItem(QQuickItem *parent = nullptr);
    ~VideoRendererItem() override;

    int videoWidth() const { return m_width; }
    int videoHeight() const { return m_height; }
    qreal renderedFps() const { return m_renderedFps; }

public slots:
    void updateFrameRgb(const QImage &image);

signals:
    void videoDimensionsChanged();
    void fpsChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    QImage m_currentImage;
    QMutex m_frameMutex;
    int m_width{1920};
    int m_height{1080};
    qreal m_renderedFps{60.0};
    bool m_hasNewFrame{false};
};

} // namespace airwave::desktop
