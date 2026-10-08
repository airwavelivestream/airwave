#pragma once
#include <QObject>
#include <QDateTime>
#include <deque>

namespace airwave::desktop {

class LatencyTracker : public QObject {
    Q_OBJECT
    Q_PROPERTY(qreal glassToGlassMs READ glassToGlassMs NOTIFY metricsUpdated)
    Q_PROPERTY(qreal transportMs READ transportMs NOTIFY metricsUpdated)
    Q_PROPERTY(qreal fps READ fps NOTIFY metricsUpdated)
    Q_PROPERTY(qreal packetLossPercent READ packetLossPercent NOTIFY metricsUpdated)

public:
    explicit LatencyTracker(QObject *parent = nullptr);

    qreal glassToGlassMs() const { return m_glassToGlassMs; }
    qreal transportMs() const { return m_transportMs; }
    qreal fps() const { return m_fps; }
    qreal packetLossPercent() const { return m_packetLossPercent; }

public slots:
    void recordFrame(quint32 senderPtsUs, quint32 decodeFinishUs);
    void updateLoss(quint16 lost, quint16 total);

signals:
    void metricsUpdated();

private:
    qreal m_glassToGlassMs{0.0};
    qreal m_transportMs{0.0};
    qreal m_fps{60.0};
    qreal m_packetLossPercent{0.0};
    quint64 m_frameCounter{0};
    qint64 m_lastCalcTime{0};
};

} // namespace airwave::desktop
