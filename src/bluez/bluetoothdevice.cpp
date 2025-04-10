// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
#include <QTimer>
#include <albert/logging.h>
#include <albert/notification.h>
using namespace albert;
using namespace std;

BluetoothDevice::BluetoothDevice(std::unique_ptr<BluetoothDevicePrivateBase> &&_d):
    d(::move(_d))
{
}

BluetoothDevice::~BluetoothDevice()
{

}

std::optional<QString> BluetoothDevice::toggleConnected()
{
    using enum BluetoothDevice::State;

    return {};
}
