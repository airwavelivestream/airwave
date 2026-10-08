#pragma once
#include <QObject>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QByteArray>
#include <memory>
#include <atomic>
#include "awtp_protocol.hpp"

namespace airwave::desktop {

class AwtpReceiver : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStateChanged)
    Q_PROPERTY(quint64 receivedFrames READ receivedFrames NOTIFY frameReceived)

public:
    explicit AwtpReceiver(QObject *parent = nullptr);
    ~AwtpReceiver() override;

    bool start(quint16 port = protocol::DEFAULT_MEDIA_PORT);
    void stop();

    bool isConnected() const { return m_connected.load(); }
    quint64 receivedFrames() const { return m_receivedFrames.load(); }

signals:
    void videoPacketReady(const QByteArray &data, quint32 timestampUs, bool isKeyframe);
    void audioPacketReady(const QByteArray &data, quint32 timestampUs);
    void latencySampleReceived(quint32 rttUs, quint32 transportLatencyUs);
    void connectionStateChanged(bool connected);
    void frameReceived();

private slots:
    void handlePendingDatagrams();

private:
    std::unique_ptr<QUdpSocket> m_udpSocket;
    std::atomic<bool> m_connected{false};
    std::atomic<quint64> m_receivedFrames{0};
    quint16 m_expectedSeq{0};
};

} // namespace airwave::desktop
