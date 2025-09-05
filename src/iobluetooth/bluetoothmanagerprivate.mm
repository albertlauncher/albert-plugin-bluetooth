// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivate.h"
#include "bluetoothmanager.h"
#include "bluetoothmanagerprivate.h"


BluetoothManager::BluetoothManager()
    : d(std::make_unique<BluetoothManagerPrivate>(this))
{
    // On macos there is only one active host controller
    addController(defaultController());
}

BluetoothManager::~BluetoothManager()
{

}
