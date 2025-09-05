// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <memory>
@class BluetoothConnectionObserver;
@class BluetoothPowerDelegate;
@class IOBluetoothHostController;
class BluetoothController;

std::unique_ptr<BluetoothController> defaultController();

class BluetoothControllerPrivate
{
public:
    __strong IOBluetoothHostController *controller;
    __strong BluetoothPowerDelegate* power_delegate;
    __strong BluetoothConnectionObserver* connection_observer;
};
