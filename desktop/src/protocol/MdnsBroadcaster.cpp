#include "MdnsBroadcaster.hpp"
#include <QNetworkInterface>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace airwave::desktop {

MdnsBroadcaster::MdnsBroadcaster(QObject *parent)
    : QObject(parent)
{
    connect(&m_broadcastTimer, &QTimer::timeout, this, &MdnsBroadcaster::sendAnnouncement);
}

MdnsBroadcaster::~MdnsBroadcaster() {
    stop();
}

bool MdnsBroadcaster::start(quint16 servicePort, const QString &serviceName) {
    m_port = servicePort;
    m_name = serviceName;

    // Standard mDNS Multicast Group 224.0.0.251 : 5353
    bool ok = m_multicastSocket.bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress);
    if (!ok) {
        qWarning() << "[MdnsBroadcaster] Failed to bind local socket";
        return false;
    }

    sendAnnouncement();
    m_broadcastTimer.start(3000); // Pulse every 3s
    qInfo() << "[MdnsBroadcaster] Broadcasting Airwave discovery presence for" << serviceName;
    return true;
}

void MdnsBroadcaster::stop() {
    m_broadcastTimer.stop();
    m_multicastSocket.close();
}

void MdnsBroadcaster::sendAnnouncement() {
    QJsonObject info;
    info["service"] = "_airwave._tcp";
    info["name"] = m_name;
    info["port"] = m_port;
    info["version"] = 1;

    QByteArray datagram = QJsonDocument(info).toJson(QJsonDocument::Compact);
    // Broadcast on subnet and mDNS multicast
    m_multicastSocket.writeDatagram(datagram, QHostAddress("224.0.0.251"), 5353);
    m_multicastSocket.writeDatagram(datagram, QHostAddress::Broadcast, 49154);
}

} // namespace airwave::desktop
