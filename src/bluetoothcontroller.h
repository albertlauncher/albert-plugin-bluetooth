// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QObject>
#include <vector>
#include <memory>
class BluetoothDevice;
class BluetoothControllerPrivateBase;

class BluetoothController : public QObject
{
    Q_OBJECT

public:

    BluetoothController();
    ~BluetoothController();

    const QString &address() const;
    const QString &name() const;

    void toggle();
    bool poweredOn() const;

    std::vector<std::shared_ptr<BluetoothDevice>> devices();

protected:

    void setName(const QString &);
    void setPoweredOn(bool);

signals:

    void nameChanged(const QString &);
    void poweredOnChanged(bool);
    void devicesChanged();

private:

    std::unique_ptr<BluetoothControllerPrivateBase> d;

};
