// Copyright (c) 2024-2025 Manuel Schneider

#include <albert/logging.h>
#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "items.h"
#include <albert/albert.h>
using namespace albert;
using namespace std;

const QString BluetoothControllerItem::tr_bluetooth = QObject::tr("Bluetooth");

static QStringList icons(BluetoothDevice::State state)
{
    switch (state) {
    case BluetoothDevice::State::Disconnected:  return {QStringLiteral(":bt-inactive")};
    case BluetoothDevice::State::Connecting:    return {QStringLiteral(":bt-change")};
    case BluetoothDevice::State::Connected:     return {QStringLiteral(":bt-active")};
    case BluetoothDevice::State::Disconnecting: return {QStringLiteral(":bt-change")};
    default: break;
    }
    return {};
}

// -------------------------------------------------------------------------------------

BluetoothControllerItem::BluetoothControllerItem(BluetoothController &c)
    : controller(c)
{
    connect(&controller, &BluetoothController::poweredOnChanged,
            this, &BluetoothControllerItem::notifyObservers);
}

QString BluetoothControllerItem::id() const { return tr_bluetooth; }

QString BluetoothControllerItem::text() const { return tr_bluetooth; }

QString BluetoothControllerItem::subtext() const
{ return controller.poweredOn() ? tr("Enabled") : tr("Disabled"); }

QStringList BluetoothControllerItem::iconUrls() const
{
    return icons(controller.poweredOn() ? BluetoothDevice::State::Connected
                                        : BluetoothDevice::State::Disconnected);
}

QString BluetoothControllerItem::inputActionText() const { return tr_bluetooth; }

vector<Action> BluetoothControllerItem::actions() const
{
    return {
        {
            QStringLiteral("pow"),
            controller.poweredOn() ? tr("Disable") : tr("Enable"),
            [this] { controller.setPoweredOn(!controller.poweredOn()); }
        }
#ifdef Q_OS_MACOS
        ,{
            QStringLiteral("bluetooth"),
            tr("Open Bluetooth settings"),
            [] { openUrl("x-apple.systempreferences:com.apple.Bluetooth"); }
        }
#endif
    };
}

void BluetoothControllerItem::addObserver(Observer *observer) { observers.insert(observer); }

void BluetoothControllerItem::removeObserver(Observer *observer) { observers.erase(observer); }

void BluetoothControllerItem::notifyObservers()
{ for (auto observer : observers) observer->notify(this); }


// -------------------------------------------------------------------------------------

BluetoothDeviceItem::BluetoothDeviceItem(std::shared_ptr<BluetoothDevice> c):
    device(c)
{
    connect(device.get(), &BluetoothDevice::stateChanged,
            this, &BluetoothDeviceItem::notifyObservers);
}

QString BluetoothDeviceItem::id() const { return device->name(); }

QString BluetoothDeviceItem::text() const { return device->name(); }

QString BluetoothDeviceItem::subtext() const
{
    switch (device->state()) {
    case BluetoothDevice::State::Disconnected: return tr("Disconnected");
    case BluetoothDevice::State::Connecting: return tr("Connecting…");
    case BluetoothDevice::State::Connected: return tr("Connected");
    case BluetoothDevice::State::Disconnecting: return tr("Disconnecting…");
    default: break;
    }
    return {};
}

QStringList BluetoothDeviceItem::iconUrls() const { return icons(device->state()); }

QString BluetoothDeviceItem::inputActionText() const { return device->name(); }

vector<Action> BluetoothDeviceItem::actions() const
{
    switch (device->state()) {
    case BluetoothDevice::State::Disconnected:
        return {{
            QStringLiteral("toogle"),
            tr("Connect"),
            [this] { device->toggleConnected(); }
        }};
    case BluetoothDevice::State::Connecting:
        return {};
    case BluetoothDevice::State::Connected:
        return {{
            QStringLiteral("toogle"),
            tr("Disconnect"),
            [this] { device->toggleConnected(); }
        }};
    case BluetoothDevice::State::Disconnecting:
        return {};
    }
    return {};
}

void BluetoothDeviceItem::addObserver(Observer *observer) { observers.insert(observer); }

void BluetoothDeviceItem::removeObserver(Observer *observer) { observers.erase(observer); }

void BluetoothDeviceItem::notifyObservers()
{ for (auto observer : observers) observer->notify(this); }

