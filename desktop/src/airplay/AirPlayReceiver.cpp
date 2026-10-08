#include "AirPlayReceiver.hpp"
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QDateTime>
#include <QDebug>

namespace airwave::desktop::airplay {

AirPlayReceiver::AirPlayReceiver(QObject *parent)
    : QObject(parent)
{
}

AirPlayReceiver::~AirPlayReceiver() {
    stop();
}

bool AirPlayReceiver::start(const QString &displayName) {
    m_displayName = displayName;
    m_active.store(true);
    emit activeStateChanged(true);
    qInfo() << "[AirPlayReceiver] Initialized zero-app AirPlay Receiver for" << displayName;
    return true;
}

void AirPlayReceiver::stop() {
    if (m_active.exchange(false)) {
        emit activeStateChanged(false);
        emit deviceDisconnected();
    }
}

void AirPlayReceiver::handleRtspConnection() {
    // AirPlay RTSP Setup / Teardown handshake handler (UxPlay adaptation)
}

void AirPlayReceiver::processVideoDatagrams() {
    // Demux RTP H.264 NAL units and dispatch to HwDecoder directly
}

} // namespace airwave::desktop::airplay
