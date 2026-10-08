#include "AwtpReceiver.hpp"
#include <QNetworkDatagram>
#include <QDateTime>
#include <QDebug>

namespace airwave::desktop {

AwtpReceiver::AwtpReceiver(QObject *parent)
    : QObject(parent),
      m_udpSocket(std::make_unique<QUdpSocket>(this))
{
}

AwtpReceiver::~AwtpReceiver() {
    stop();
}

bool AwtpReceiver::start(quint16 port) {
    if (m_udpSocket->state() == QAbstractSocket::BoundState) {
        m_udpSocket->close();
    }

    // Set high-throughput socket options
    bool ok = m_udpSocket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
    if (!ok) {
        qWarning() << "[AwtpReceiver] Failed to bind to UDP port:" << port << m_udpSocket->errorString();
        return false;
    }

    // Allocate 4MB receive buffer for 1080p60/120 high bitrate packet bursts
    m_udpSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 4 * 1024 * 1024);

    connect(m_udpSocket.get(), &QUdpSocket::readyRead, this, &AwtpReceiver::handlePendingDatagrams, Qt::UniqueConnection);
    m_connected.store(true);
    emit connectionStateChanged(true);
    qInfo() << "[AwtpReceiver] Listening on 0.0.0.0:" << port;
    return true;
}

void AwtpReceiver::stop() {
    if (m_udpSocket && m_udpSocket->isOpen()) {
        m_udpSocket->close();
    }
    if (m_connected.exchange(false)) {
        emit connectionStateChanged(false);
    }
}

void AwtpReceiver::handlePendingDatagrams() {
    while (m_udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        const QByteArray payload = datagram.data();

        if (payload.size() < static_cast<int>(sizeof(protocol::AwtpHeader))) {
            continue; // Drop truncated datagram
        }

        const auto *header = reinterpret_cast<const protocol::AwtpHeader*>(payload.constData());
        if (header->magic != protocol::AWTP_MAGIC || header->version != protocol::AWTP_VERSION) {
            continue; // Invalid protocol magic
        }

        quint32 nowUs = static_cast<quint32>(QDateTime::currentMSecsSinceEpoch() * 1000);
        quint32 transportLatencyUs = (nowUs >= header->timestampUs) ? (nowUs - header->timestampUs) : 0;

        const char *dataPtr = payload.constData() + sizeof(protocol::AwtpHeader);
        int dataSize = payload.size() - sizeof(protocol::AwtpHeader);

        if (header->msgType == static_cast<uint8_t>(protocol::MsgType::VideoPacket)) {
            bool isKeyframe = (header->flags & protocol::FLAG_KEYFRAME) != 0;
            QByteArray nalu(dataPtr, dataSize);
            m_receivedFrames.fetch_add(1);
            emit videoPacketReady(nalu, header->timestampUs, isKeyframe);
            emit frameReceived();
            emit latencySampleReceived(0, transportLatencyUs);
        } else if (header->msgType == static_cast<uint8_t>(protocol::MsgType::AudioPacket)) {
            QByteArray audioData(dataPtr, dataSize);
            emit audioPacketReady(audioData, header->timestampUs);
        } else if (header->msgType == static_cast<uint8_t>(protocol::MsgType::Ping)) {
            // Echo pong back to sender for glass-to-glass and RTT calculation
            protocol::AwtpHeader pongHeader{};
            pongHeader.magic = protocol::AWTP_MAGIC;
            pongHeader.version = protocol::AWTP_VERSION;
            pongHeader.msgType = static_cast<uint8_t>(protocol::MsgType::Pong);
            pongHeader.timestampUs = header->timestampUs;
            m_udpSocket->writeDatagram(reinterpret_cast<const char*>(&pongHeader), sizeof(pongHeader), datagram.senderAddress(), datagram.senderPort());
        }
    }
}

} // namespace airwave::desktop
