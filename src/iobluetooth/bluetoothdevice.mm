// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "bluetoothdevice_p.h"
#include <Foundation/Foundation.h>
#include <QTimer>
#include <albert/logging.h>
#include <albert/notification.h>
using namespace std;
using namespace albert;
#if  ! __has_feature(objc_arc)
#error This file must be compiled with ARC.
#endif

@interface BluetoothConnectionHandler : NSObject

- (instancetype)initWith:(function<void(IOReturn)>)callback;

- (void)connectionComplete:(IOBluetoothDevice *)device
                    status:(IOReturn)status;
@end


BluetoothDevice::BluetoothDevice(std::unique_ptr<BluetoothDevicePrivate> &&_d):
    d(::move(_d))
{
    d->q = this;
}

BluetoothDevice::~BluetoothDevice()
{

}

BluetoothController &BluetoothDevice::controller() { return d->controller; }

const QString &BluetoothDevice::name() const { return d->name; }

BluetoothDevice::State BluetoothDevice::state() const { return d->state; }

std::optional<QString> BluetoothDevice::toggleConnected()
{
    using enum BluetoothDevice::State;

    if (d->state == Connecting)
        return tr("Device is currently connecting.");

    else if (d->state == Disconnecting)
        return tr("Device is currently disconnecting.");

    else if (d->state == Connected)
    {
        d->setStateAndNotify(Disconnecting);
        if (auto status = [d->device closeConnection];
            status == kIOReturnSuccess)
        {
            d->setStateAndNotify(Disconnected);
            INFO << QString("Successfully disconnected '%1'.").arg(d->name);
        }
        else
        {
            d->setStateAndNotify(Connected);
            WARN << QString("Failed disconnecting '%1': Status '%2'").arg(d->name, status);
        }
    }

    else  // Disconnected
    {
        if (!d->controller.poweredOn())
            powerOnAndConnect();
        else
        {
            // Lazy handler creation
            if (d->connection_handler == nil)
            {
                auto callback = [this](IOReturn status)
                {
                    if (status == kIOReturnSuccess)
                    {
                        INFO << QString("Successfully connected to device: '%1'").arg(d->name);
                        d->setStateAndNotify(Connected);
                        auto *n = new Notification(d->name, this->tr("Connected."));
                        n->send();
                        QTimer::singleShot(5000, n, [n]{ n->deleteLater(); });
                    }
                    else
                    {
                        WARN << QString("Failed to connect to device: '%1', status: '%2'")
                                    .arg(d->name).arg(status);
                        d->setStateAndNotify(Disconnected);
                        auto *n = new Notification(d->name, BluetoothDevice::tr("Connection failed"));
                        n->send();
                        QTimer::singleShot(5000, n, [n]{ n->deleteLater(); });
                    }
                };

                d->connection_handler = [[BluetoothConnectionHandler alloc] initWith:callback];
                [d->device addObserver:d->connection_handler
                            forKeyPath:@"connected"
                               options:NSKeyValueObservingOptionNew
                               context:nil];
            }

            if (auto status = [d->device openConnection:d->connection_handler
                                        withPageTimeout:(BluetoothHCIPageTimeout)BluetoothGetSlotsFromSeconds(3)
                                 authenticationRequired:false];
                status == kIOReturnSuccess)
            {
                DEBG << QString("Successfully issued CREATE_CONNECTION command for '%1'.").arg(d->name);
                d->setStateAndNotify(Connecting);
            }
            else
            {
                WARN << QString("Failed issuing CREATE_CONNECTION command for '%1': Status '%2'")
                            .arg(d->name, status);
            }
        }
    }
    return {};
}

// ------------------------------------------------------------------------------------------------
// Do _not_ move upwards. Interface implementation confuses lupdate.
// ------------------------------------------------------------------------------------------------

@implementation BluetoothConnectionHandler{
    function<void(IOReturn)> _callback;
}

- (instancetype)initWith:(function<void(IOReturn)>)callback
{
    self = [super init];
    if (self) {
        _callback = callback;
    }
    return self;
}

- (void)connectionComplete:(IOBluetoothDevice *)device
                    status:(IOReturn)status
{
    _callback(status);
}

@end

