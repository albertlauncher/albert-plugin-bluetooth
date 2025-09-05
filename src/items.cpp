// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "items.h"
#include <albert/logging.h>
#include <albert/systemutil.h>
using enum BluetoothDevice::State;
using namespace Qt::StringLiterals;
using namespace albert::util;
using namespace albert;
using namespace std;


// -------------------------------------------------------------------------------------------------

const QString BluetoothControllerItem::tr_bluetooth = QObject::tr("Bluetooth");

BluetoothControllerItem::BluetoothControllerItem(shared_ptr<BluetoothController> c) :
    controller(::move(c))
{
    connect(controller.get(), &BluetoothController::poweredOnChanged,
            this, [this]{ dataChanged(); });
    connect(controller.get(), &BluetoothController::nameChanged,
            this, [this]{ dataChanged(); });
}

QString BluetoothControllerItem::id() const { return tr_bluetooth; }

QString BluetoothControllerItem::text() const { return tr_bluetooth; }

QString BluetoothControllerItem::subtext() const
{
    if (controller->name().isEmpty())
        return controller->poweredOn() ? tr("Enabled") : tr("Disabled");
    else
        return u"%1  ·  %2"_s.arg(controller->poweredOn() ? tr("Enabled") : tr("Disabled"),
                                  controller->name());
}

QStringList BluetoothControllerItem::iconUrls() const
{ return {controller->poweredOn() ? u":bt-active"_s : u":bt-inactive"_s}; }

QString BluetoothControllerItem::inputActionText() const { return tr_bluetooth; }

vector<Action> BluetoothControllerItem::actions() const
{
    return {
        {
            u"pow"_s,
            controller->poweredOn() ? tr("Disable") : tr("Enable"),
            [this] { controller->toggle(); }
        }
#ifdef Q_OS_MACOS
        ,{
            u"bluetooth"_s,
            tr("Open Bluetooth settings"),
            [] { openUrl(u"x-apple.systempreferences:com.apple.Bluetooth"_s); }
        }
#endif
    };
}


// -------------------------------------------------------------------------------------------------

BluetoothDeviceItem::BluetoothDeviceItem(shared_ptr<BluetoothDevice> dev):
    device(::move(dev))
{
    connect(device.get(), &BluetoothDevice::stateChanged,
            this, [this]{ dataChanged(); });
}

QString BluetoothDeviceItem::id() const { return device->name(); }

QString BluetoothDeviceItem::text() const { return device->name(); }

QString BluetoothDeviceItem::subtext() const
{
    switch (device->state()) {
    case Connecting:
    case Disconnecting:
        return device->stateString() + u'…';
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
    case Disconnected:  return {u":bt-inactive"_s};
    case Connecting:    return {u":bt-change"_s};
    case Connected:     return {u":bt-active"_s};
    case Disconnecting: return {u":bt-change"_s};
    default: break;
    }
    return {};
}

QString BluetoothDeviceItem::inputActionText() const { return device->name(); }

vector<Action> BluetoothDeviceItem::actions() const
{
    switch (device->state()) {
    case Disconnected:
        return {{u"toogle"_s, tr("Connect"), [this] { device->toggleConnected(); }}};
    case Connecting:
        return {};
    case Connected:
        return {{u"toogle"_s, tr("Disconnect"), [this] { device->toggleConnected(); }}};
    case Disconnecting:
        return {};
    }
    return {};
}
