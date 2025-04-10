// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QString>
#include <QObject>
class BluetoothDevicePrivate;
class BluetoothController;

class BluetoothDevice : public QObject
{
    Q_OBJECT

public:

    BluetoothDevice(std::unique_ptr<BluetoothDevicePrivate>&&);
    ~BluetoothDevice();

    BluetoothController &controller();

    const QString &name() const;

    enum class State { Disconnected, Connecting, Connected, Disconnecting };
    State state() const;

    std::optional<QString> toggleConnected();

    void powerOnAndConnect();

signals:

    void stateChanged(State);

private:

    std::unique_ptr<BluetoothDevicePrivate> d;
    friend class BluetoothController;

};
