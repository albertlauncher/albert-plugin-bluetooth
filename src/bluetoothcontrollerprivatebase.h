// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QString>

class BluetoothControllerPrivateBase
{
public:

    BluetoothControllerPrivateBase(const QString &a, const QString &n, bool p):
        address(a), name(n), powered_on(p) {}
    virtual ~BluetoothControllerPrivateBase() = default;

private:

    QString address;
    QString name;
    bool powered_on;
    friend class BluetoothController;
};
