// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivatebase.h"
#include <QTimer>
#include <albert/logging.h>
#include <albert/notification.h>
using namespace albert::util;
using namespace albert;
using enum BluetoothDevice::State;

BluetoothController &BluetoothDevice::controller() const { return d->controller_; }

const QString &BluetoothDevice::address() const { return d->address_; }

const QString &BluetoothDevice::name() const { return d->name_; }

BluetoothDevice::State BluetoothDevice::state() const { return d->state_; }

void BluetoothDevice::powerOnAndConnect()
{
    Q_ASSERT(controller().poweredOn() == false);

    controller().toggle();
    connect(&controller(), &BluetoothController::poweredOnChanged,
            this, &BluetoothDevice::toggleConnected,
            Qt::SingleShotConnection);
}

QString BluetoothDevice::stateString() const
{
    switch (d->state_) {
    case Disconnected: return tr("Disconnected");
    case Connecting: return tr("Connecting");
    case Connected: return tr("Connected");
    case Disconnecting: return tr("Disconnecting");
    }
    return {};
}

void BluetoothDevice::setName(const QString &name)
{
    if (d->name_ == name)
        return;

    DEBG << QStringLiteral("Bluetooth device name changed to '%1' (%2)")
                .arg(name, d->address_);

    d->name_ = name;
    emit nameChanged(name);
}

void BluetoothDevice::setState(State s)
{
    if (d->state_ == s)
        return;

    DEBG << QStringLiteral("Bluetooth device state changed to '%1' (%2 %3)")
                .arg(stateString(), d->name_, d->address_);

    d->state_ = s;
    emit stateChanged(s);
}

void BluetoothDevice::connectionCallback(std::optional<QString> error)
{
    if (error)
    {
        WARN << QStringLiteral("Failed to connect to device: '%1'. Error: '%2'")
                    .arg(name(), error.value());
        setState(Disconnected);
        auto *n = new Notification(name(), tr("Connection failed."));
        n->send();
        QTimer::singleShot(5000, n, [n]{ n->deleteLater(); });
    }
    else
    {
        INFO << QStringLiteral("Successfully connected to device: '%1'").arg(name());
        setState(Connected);
        auto *n = new Notification(name(), tr("Connected."));
        n->send();
        QTimer::singleShot(5000, n, [n]{ n->deleteLater(); });
    }
}
