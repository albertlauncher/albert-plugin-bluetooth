// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothdevice.h"
#include "bluetoothdeviceitem.h"
using enum BluetoothDevice::State;
using namespace albert;
using namespace std;

BluetoothDeviceItem::BluetoothDeviceItem(std::shared_ptr<BluetoothDevice> d):
    device(::move(d))
{
    connect(device.get(), &BluetoothDevice::stateChanged,
            this, &BluetoothDeviceItem::notifyObservers);
}

QString BluetoothDeviceItem::id() const { return device->name(); }

QString BluetoothDeviceItem::text() const { return device->name(); }

QString BluetoothDeviceItem::subtext() const
{
    switch (device->state()) {
    case Connecting:
    case Disconnecting:
        return device->stateString() + QStringLiteral("…");
    case Connected:
    case Disconnected:
        return device->stateString();
    default: break;
    }
    return {};
}

QStringList BluetoothDeviceItem::iconUrls() const
{
    switch (device->state()) {
    case Disconnected:  return {QStringLiteral(":bt-inactive")};
    case Connecting:    return {QStringLiteral(":bt-change")};
    case Connected:     return {QStringLiteral(":bt-active")};
    case Disconnecting: return {QStringLiteral(":bt-change")};
    default: break;
    }
    return {};
}

QString BluetoothDeviceItem::inputActionText() const { return device->name(); }

vector<Action> BluetoothDeviceItem::actions() const
{
    switch (device->state()) {
    case Disconnected:
        return {{
            QStringLiteral("toogle"),
            tr("Connect"),
            [this] { device->toggleConnected(); }
        }};
    case Connecting:
        return {};
    case Connected:
        return {{
            QStringLiteral("toogle"),
            tr("Disconnect"),
            [this] { device->toggleConnected(); }
        }};
    case Disconnecting:
        return {};
    }
    return {};
}

void BluetoothDeviceItem::addObserver(Observer *observer) { observers.insert(observer); }

void BluetoothDeviceItem::removeObserver(Observer *observer) { observers.erase(observer); }

void BluetoothDeviceItem::notifyObservers()
{ for (auto observer : observers) observer->notify(this); }

