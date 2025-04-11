// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include "bluetoothcontrollerprivatebase.h"
#include <Foundation/Foundation.h>
#include <IOBluetooth/IOBluetooth.h>
#include <map>
#include <memory>
#include <shared_mutex>
@class BluetoothConnectionObserver;
@class BluetoothPowerDelegate;
class BluetoothController;
class BluetoothDevice;


class BluetoothControllerPrivate : public BluetoothControllerPrivateBase
{
public:
    BluetoothControllerPrivate(IOBluetoothHostController*);

    __strong IOBluetoothHostController *controller;
    __strong BluetoothPowerDelegate* power_delegate;
    __strong BluetoothConnectionObserver* connection_observer;
    std::shared_mutex lock;
    std::map<IOBluetoothDevice*,std::shared_ptr<BluetoothDevice>> devices;

};
