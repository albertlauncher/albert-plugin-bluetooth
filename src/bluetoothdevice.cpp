// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include <QTimer>
#include <albert/logging.h>
#include <albert/notification.h>
using enum BluetoothDevice::State;
using namespace Qt::StringLiterals;
using namespace albert::util;
using namespace albert;
using namespace std;

void BluetoothDevice::powerOnAndConnect()
{
    Q_ASSERT(controller().poweredOn() == false);
    if (controller().poweredOn())
    {
        if (state() == Disconnected)
            toggleConnected();
    }
    else
    {
        controller().toggle();
        connect(&controller(), &BluetoothController::poweredOnChanged,
                this, &BluetoothDevice::toggleConnected,
                Qt::SingleShotConnection);
    }
}

QString BluetoothDevice::stateString() const
{
    switch (state_) {
    case Disconnected: return tr("Disconnected");
    case Connecting: return tr("Connecting");
    case Connected: return tr("Connected");
    case Disconnecting: return tr("Disconnecting");
    }
    return {};
}

void BluetoothDevice::setName(const QString &name)
{
    if (name_ != name)
    {
        name_ = name;
        DEBG << u"Bluetooth device name changed to '%1' (%2)"_s.arg(name_, address_);
        emit nameChanged(name_);
    }
}

void BluetoothDevice::setState(State s)
{
    if (state_ != s)
    {
        state_ = s;
        DEBG << QStringLiteral("Bluetooth device state changed to '%1' (%2 %3)")
                    .arg(stateString(), name_, address_);
        emit stateChanged(s);
    }
}

void BluetoothDevice::connectionCallback(optional<QString> error)
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
