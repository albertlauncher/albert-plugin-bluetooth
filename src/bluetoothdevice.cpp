// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothdevice.h"
#include "bluetoothcontroller.h"


void BluetoothDevice::powerOnAndConnect()
{
    controller().setPoweredOn(true);
    connect(&controller(), &BluetoothController::poweredOnChanged,
            this, &BluetoothDevice::toggleConnected,
            Qt::SingleShotConnection);
}
