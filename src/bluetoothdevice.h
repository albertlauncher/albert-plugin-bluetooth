// Copyright (c) 2024-2025 Manuel Schneider

#pragma once
#include <QObject>
#include <memory>
#include <optional>
class BluetoothController;
class BluetoothDevicePrivate;

class BluetoothDevice : public QObject
{
    Q_OBJECT
public:
    enum class State { Disconnected, Connecting, Connected, Disconnecting };

    BluetoothDevice(BluetoothController *controller, std::unique_ptr<BluetoothDevicePrivate> &&);
    ~BluetoothDevice();

    inline BluetoothController &controller() const { return controller_; }
    inline const QString &address() { return address_; }
    inline const QString &name() const { return name_; }
    inline State state() const { return state_; }
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

    BluetoothController &controller_;
    QString address_;
    QString name_;
    BluetoothDevice::State state_;
    std::unique_ptr<BluetoothDevicePrivate> d;

    friend class BluetoothController;
    friend class BluetoothDevicePrivate;

};
