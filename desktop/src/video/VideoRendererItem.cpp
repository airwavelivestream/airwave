#include "VideoRendererItem.hpp"
#include <QSGSimpleTextureNode>
#include <QQuickWindow>

namespace airwave::desktop {

VideoRendererItem::VideoRendererItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

VideoRendererItem::~VideoRendererItem() = default;

void VideoRendererItem::updateFrameRgb(const QImage &image) {
    {
        QMutexLocker locker(&m_frameMutex);
        m_currentImage = image;
        if (m_width != image.width() || m_height != image.height()) {
            m_width = image.width();
            m_height = image.height();
            emit videoDimensionsChanged();
        }
        m_hasNewFrame = true;
    }
    update(); // Request Scene Graph redraw
}

QSGNode *VideoRendererItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
    auto *node = static_cast<QSGSimpleTextureNode *>(oldNode);
    if (!node) {
        node = new QSGSimpleTextureNode();
    }

    QImage frameToRender;
    {
        QMutexLocker locker(&m_frameMutex);
        if (!m_hasNewFrame && node->texture()) {
            return node;
        }
        frameToRender = m_currentImage;
        m_hasNewFrame = false;
    }

    if (!frameToRender.isNull() && window()) {
        QSGTexture *texture = window()->createTextureFromImage(frameToRender, QQuickWindow::TextureHasAlphaChannel);
        node->setTexture(texture);
        node->setOwnsTexture(true);
        node->setRect(boundingRect());
        node->setFiltering(QSGTexture::Linear);
    }

    return node;
}

} // namespace airwave::desktop
