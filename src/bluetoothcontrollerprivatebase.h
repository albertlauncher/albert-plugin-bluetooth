// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QString>
#include <map>
#include <memory>
#include <shared_mutex>
class BluetoothDevice;

class BluetoothControllerPrivateBase
{
public:

    BluetoothControllerPrivateBase(const QString &address, const QString &name, bool power):
        address_(address),
        name_(name),
        powered_on_(power)
    {}
    virtual ~BluetoothControllerPrivateBase() = default;


private:

    QString address_;
    QString name_;
    bool powered_on_;
    std::shared_mutex lock;
    std::map<QString, std::shared_ptr<BluetoothDevice>> devices_;
    friend class BluetoothController;
};
