// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
class BluetoothManager;

class BluetoothManagerPrivate
{
public:
    BluetoothManagerPrivate(BluetoothManager *q);
    ~BluetoothManagerPrivate();

private:
    BluetoothManager *q_;
};
