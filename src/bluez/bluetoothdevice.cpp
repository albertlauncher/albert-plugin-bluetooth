// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "bluetoothdevice_p.h"
#include <QTimer>
#include <albert/logging.h>
#include <albert/notification.h>
using namespace albert;
using namespace std;

BluetoothDevice::BluetoothDevice(std::unique_ptr<BluetoothDevicePrivate> &&_d):
    d(::move(_d))
{
    d->q = this;
}

BluetoothDevice::~BluetoothDevice()
{

}

BluetoothController &BluetoothDevice::controller() { return d->controller; }

const QString &BluetoothDevice::name() const { return d->name; }

BluetoothDevice::State BluetoothDevice::state() const { return d->state; }

std::optional<QString> BluetoothDevice::toggleConnected()
{
    using enum BluetoothDevice::State;

    return {};
}
