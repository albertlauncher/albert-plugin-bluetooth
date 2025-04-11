// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QObject>
#include <QString>
#include <memory>
#include <optional>
class BluetoothController;
class BluetoothDevicePrivateBase;

class BluetoothDevice : public QObject
{
    Q_OBJECT

public:

    enum class State { Disconnected, Connecting, Connected, Disconnecting };

    BluetoothDevice(std::unique_ptr<BluetoothDevicePrivateBase>&&);
    ~BluetoothDevice();

    BluetoothController &controller() const;
    const QString &address() const;
    const QString &name() const;
    State state() const;
    QString stateString() const;

    std::optional<QString> toggleConnected();
    void powerOnAndConnect();

protected:

    void setName(const QString &name);
    void setState(State s);
    void connectionCallback(std::optional<QString> error);

signals:

    void stateChanged(State);
    void nameChanged(const QString& name);

private:

    std::unique_ptr<BluetoothDevicePrivateBase> d;
    friend class BluetoothController;

};
