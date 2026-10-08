#pragma once
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>

namespace airwave::desktop {

class MdnsBroadcaster : public QObject {
    Q_OBJECT

public:
    explicit MdnsBroadcaster(QObject *parent = nullptr);
    ~MdnsBroadcaster() override;

    bool start(quint16 servicePort = 49152, const QString &serviceName = "Airwave-PC");
    void stop();

private slots:
    void sendAnnouncement();

private:
    QUdpSocket m_multicastSocket;
    QTimer m_broadcastTimer;
    quint16 m_port{49152};
    QString m_name;
};

} // namespace airwave::desktop
