#pragma once
#include <QObject>
#include <QByteArray>
#include <memory>
#include <atomic>

namespace airwave::desktop::airplay {

// AirPlay standard protocol port numbers
constexpr uint16_t AIRPLAY_RTSP_PORT = 7000;
constexpr uint16_t AIRPLAY_VIDEO_PORT = 7001;
constexpr uint16_t AIRPLAY_AUDIO_PORT = 7002;

class AirPlayReceiver : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isActive READ isActive NOTIFY activeStateChanged)
    Q_PROPERTY(QString connectedDevice READ connectedDevice NOTIFY deviceConnected)

public:
    explicit AirPlayReceiver(QObject *parent = nullptr);
    ~AirPlayReceiver() override;

    bool start(const QString &displayName = "Airwave [PC]");
    void stop();

    bool isActive() const { return m_active.load(); }
    QString connectedDevice() const { return m_deviceName; }

signals:
    void videoFrameReady(const QByteArray &h264Data, quint32 ptsUs);
    void audioFrameReady(const QByteArray &pcmOrAacData, quint32 ptsUs);
    void activeStateChanged(bool active);
    void deviceConnected(const QString &name);
    void deviceDisconnected();

private slots:
    void handleRtspConnection();
    void processVideoDatagrams();

private:
    std::atomic<bool> m_active{false};
    QString m_displayName;
    QString m_deviceName;
};

} // namespace airwave::desktop::airplay
