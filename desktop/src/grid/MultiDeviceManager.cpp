#include "MultiDeviceManager.hpp"
#include <QDebug>

namespace airwave::desktop::grid {

MultiDeviceManager::MultiDeviceManager(QObject *parent)
    : QObject(parent)
{
}

MultiDeviceManager::~MultiDeviceManager() = default;

bool MultiDeviceManager::registerDevice(uint8_t streamId, const QString &name, const QString &ip, bool isWired) {
    if (m_devices.size() >= 8) {
        qWarning() << "[MultiDeviceManager] Maximum grid capacity (8 devices) reached";
        return false;
    }

    DeviceSlot slot;
    slot.streamId = streamId;
    slot.deviceName = name;
    slot.ipAddress = ip;
    slot.isWired = isWired;
    slot.isActive = true;

    m_devices.insert(streamId, slot);
    emit devicesChanged();
    qInfo() << "[MultiDeviceManager] Slot" << streamId << "registered:" << name << "@" << ip;
    return true;
}

void MultiDeviceManager::unregisterDevice(uint8_t streamId) {
    if (m_devices.remove(streamId) > 0) {
        emit devicesChanged();
        qInfo() << "[MultiDeviceManager] Slot" << streamId << "unregistered.";
    }
}

void MultiDeviceManager::triggerHybridFallback(uint8_t streamId, const QString &wifiIp) {
    if (!m_devices.contains(streamId)) return;

    DeviceSlot &slot = m_devices[streamId];
    qInfo() << "[MultiDeviceManager] Hybrid Failover: Device" << slot.deviceName << "lost USB link, switching to Wi-Fi @" << wifiIp;
    slot.ipAddress = wifiIp;
    slot.isWired = false;
    emit deviceSwitchedToFallback(streamId, wifiIp);
    emit devicesChanged();
}

} // namespace airwave::desktop::grid
