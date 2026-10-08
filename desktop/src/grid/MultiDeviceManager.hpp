#pragma once
#include <QObject>
#include <QMap>
#include <QString>
#include <memory>
#include "../protocol/AwtpReceiver.hpp"

namespace airwave::desktop::grid {

struct DeviceSlot {
    uint8_t streamId{0};
    QString deviceName;
    QString ipAddress;
    bool isWired{true};
    qreal latencyMs{0.0};
    bool isActive{false};
};

class MultiDeviceManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(int activeDeviceCount READ activeDeviceCount NOTIFY devicesChanged)

public:
    explicit MultiDeviceManager(QObject *parent = nullptr);
    ~MultiDeviceManager() override;

    bool registerDevice(uint8_t streamId, const QString &name, const QString &ip, bool isWired);
    void unregisterDevice(uint8_t streamId);

    // Hybrid Failover: when USB unplugged, migrate to Wi-Fi without dropping stream
    void triggerHybridFallback(uint8_t streamId, const QString &wifiIp);

    int activeDeviceCount() const { return m_devices.size(); }

signals:
    void devicesChanged();
    void deviceSwitchedToFallback(uint8_t streamId, const QString &newIp);

private:
    QMap<uint8_t, DeviceSlot> m_devices;
};

} // namespace airwave::desktop::grid
