// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivate.h"
#include "bluetoothmanager.h"
#include "bluetoothmanagerprivate.h"

BluetoothManagerPrivate::BluetoothManagerPrivate(BluetoothManager *q)
    : q_(q)
{
    // On macos there is only one active host controller
    q_->addController(defaultController());
}

BluetoothManagerPrivate::~BluetoothManagerPrivate() {}
