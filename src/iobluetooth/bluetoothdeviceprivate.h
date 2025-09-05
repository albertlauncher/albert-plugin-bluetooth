// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <memory>
@class BluetoothConnectionHandler;
@class IOBluetoothDevice;
class BluetoothController;
class BluetoothDevice;

std::unique_ptr<BluetoothDevice> deviceFromNative(BluetoothController *controller, IOBluetoothDevice *device);

class BluetoothDevicePrivate
{
public:
    __strong IOBluetoothDevice *device;
    __strong BluetoothConnectionHandler *connection_handler;
};
