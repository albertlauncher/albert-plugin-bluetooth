// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QString>
#include "bluetoothdevice.h"
#include <Foundation/Foundation.h>
#include <IOBluetooth/IOBluetooth.h>
#if  ! __has_feature(objc_arc)
#error This file must be compiled with ARC.
#endif
class BluetoothController;
@class BluetoothConnectionHandler;

class BluetoothDevicePrivate
{
public:
    BluetoothDevice *q;
    BluetoothController &controller;
    QString name;
    __strong IOBluetoothDevice *device;
    __strong BluetoothConnectionHandler* connection_handler;
    BluetoothDevice::State state;

    void setStateAndNotify(BluetoothDevice::State s)
    {
        if (state == s)
            return;

        state = s;
        emit q->stateChanged(s);
    }
};
