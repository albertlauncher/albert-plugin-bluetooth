// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
class BluetoothController;

class BluetoothControllerPrivate
{
public:

    BluetoothControllerPrivate(BluetoothController *_q, bool on):
        q(_q), powered_on_(on) {}

    BluetoothController *q;

    bool poweredOn() const;
    void setPoweredOn(bool v);

private:

    bool powered_on_;
};
