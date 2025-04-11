// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QString>

class BluetoothControllerPrivateBase
{
public:

    BluetoothControllerPrivateBase(const QString &address,
                                   const QString &name,
                                   bool on):
        address_(address),
        name_(name),
        powered_on_(on)
    {}
    virtual ~BluetoothControllerPrivateBase() = default;


private:

    QString address_;
    QString name_;
    bool powered_on_;
    friend class BluetoothController;
};
