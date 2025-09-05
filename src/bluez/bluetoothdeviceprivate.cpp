// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
#include "bluez.h"
#include <QTimer>
#include <albert/logging.h>
#include <albert/notification.h>
class BluetoothController;
using enum BluetoothDevice::State;
using namespace albert;
using namespace std;

unique_ptr<BluetoothDevice> deviceFromNative(BluetoothController *controller, const QString &object_path)
{ return make_unique<BluetoothDevice>(controller, make_unique<BluetoothDevicePrivate>(object_path)); }

BluetoothDevice::BluetoothDevice(BluetoothController *controller,
                                 unique_ptr<BluetoothDevicePrivate> &&p) :
    controller_(*controller),
    address_(p->device.address()),
    name_(p->device.alias()),
    state_(p->device.connected() ? Connected : Disconnected),
    d(::move(p))
{
}

BluetoothDevice::~BluetoothDevice()
{

}

std::optional<QString> BluetoothDevice::toggleConnected()
{
    if (d->device.connected())
        d->device.Disconnect();
    else
        d->device.Connect();

    return {};
}
