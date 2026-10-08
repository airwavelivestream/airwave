#include "LatencyTracker.hpp"

namespace airwave::desktop {

LatencyTracker::LatencyTracker(QObject *parent)
    : QObject(parent),
      m_lastCalcTime(QDateTime::currentMSecsSinceEpoch())
{
}

void LatencyTracker::recordFrame(quint32 senderPtsUs, quint32 decodeFinishUs) {
    m_frameCounter++;
    qint64 nowMs = QDateTime::currentMSecsSinceEpoch();

    if (decodeFinishUs >= senderPtsUs) {
        m_glassToGlassMs = (decodeFinishUs - senderPtsUs) / 1000.0;
    }

    if (nowMs - m_lastCalcTime >= 1000) {
        m_fps = (m_frameCounter * 1000.0) / (nowMs - m_lastCalcTime);
        m_frameCounter = 0;
        m_lastCalcTime = nowMs;
        emit metricsUpdated();
    }
}

void LatencyTracker::updateLoss(quint16 lost, quint16 total) {
    if (total > 0) {
        m_packetLossPercent = (static_cast<qreal>(lost) / total) * 100.0;
        emit metricsUpdated();
    }
}

} // namespace airwave::desktop
