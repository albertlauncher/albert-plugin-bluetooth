// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QCoreApplication>
#include <QStringList>
#include <albert/action.h>
#include <albert/item.h>
#include <vector>
class BluetoothDevice;
class BluetoothControllerPrivate;

class BluetoothController : public QObject
{
    Q_OBJECT
public:

    BluetoothController();
    ~BluetoothController();

    bool poweredOn() const;
    void setPoweredOn(bool);

    std::vector<std::shared_ptr<BluetoothDevice>> devices();

signals:

    void poweredOnChanged(bool);
    void devicesChanged();

private:

    std::unique_ptr<BluetoothControllerPrivate> d;

};
