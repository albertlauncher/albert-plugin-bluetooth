// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
#include <Foundation/Foundation.h>
#include <IOBluetooth/IOBluetooth.h>
#include <QTimer>
#include <albert/logging.h>
#include <albert/notification.h>
using enum BluetoothDevice::State;
using namespace albert;
using namespace std;
#if  ! __has_feature(objc_arc)
#error This file must be compiled with ARC.
#endif

@interface BluetoothConnectionHandler : NSObject

- (instancetype)initWith:(function<void(IOReturn)>)callback;

- (void)connectionComplete:(IOBluetoothDevice *)device
                    status:(IOReturn)status;
@end


BluetoothDevicePrivate::BluetoothDevicePrivate(BluetoothController &controller,
                                               IOBluetoothDevice *_device) :
    BluetoothDevicePrivateBase(controller,
                               QString::fromNSString(_device.addressString),
                               QString::fromNSString(_device.name),
                               _device.isConnected ? BluetoothDevice::State::Connected
                                                   : BluetoothDevice::State::Disconnected),
    device(_device),
    connection_handler(nullptr)  // lazy
{}


BluetoothDevice::BluetoothDevice(std::unique_ptr<BluetoothDevicePrivateBase> &&_d):
    d(::move(_d))
{
}

BluetoothDevice::~BluetoothDevice()
{
}

std::optional<QString> BluetoothDevice::toggleConnected()
{
    auto &p = *static_cast<BluetoothDevicePrivate*>(d.get());

    if (state() == Connecting)
        return tr("Device is currently connecting.");

    else if (state() == Disconnecting)
        return tr("Device is currently disconnecting.");

    else if (state() == Connected)
    {
        setState(Disconnecting);
        if (auto status = [p.device closeConnection];
            status == kIOReturnSuccess)
        {
            setState(Disconnected);
            INFO << QString("Successfully disconnected '%1'.").arg(d->name_);
        }
        else
        {
            setState(Connected);
            WARN << QString("Failed disconnecting '%1': Status '%2'").arg(d->name_, status);
        }
    }

    else  // Disconnected
    {
        if (!controller().poweredOn())
            powerOnAndConnect();
        else
        {
            // Lazy handler creation
            if (p.connection_handler == nil)
            {
                auto callback = [this](IOReturn status)
                {
                    if (status == kIOReturnSuccess)
                        connectionCallback({});
                    else
                        connectionCallback(QString::number(status));
                };

                p.connection_handler = [[BluetoothConnectionHandler alloc] initWith:callback];

                [p.device addObserver:p.connection_handler
                           forKeyPath:@"connected"
                              options:NSKeyValueObservingOptionNew
                              context:nil];
            }

            if (auto status = [p.device openConnection:p.connection_handler];
                status == kIOReturnSuccess)
            {
                DEBG << QString("Successfully issued CREATE_CONNECTION command for '%1'.")
                            .arg(d->name_);
                setState(Connecting);
            }
            else
            {
                WARN << QString("Failed issuing CREATE_CONNECTION command for '%1': Status '%2'")
                            .arg(d->name_, status);
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

