// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
#include <IOBluetooth/IOBluetooth.h>
#include <QString>
#include <albert/logging.h>
#include <memory>
class BluetoothController;
using enum BluetoothDevice::State;
using namespace Qt::StringLiterals;
using namespace std;
#if  ! __has_feature(objc_arc)
#error This file must be compiled with ARC.
#endif

unique_ptr<BluetoothDevice> deviceFromNative(BluetoothController *controller, IOBluetoothDevice *device)
{ return make_unique<BluetoothDevice>(controller, make_unique<BluetoothDevicePrivate>(device)); }

// ---------------------------------------------------------------------------------------------------------------------

@interface BluetoothConnectionHandler : NSObject

- (instancetype)initWith:(function<void(IOReturn)>)callback;

- (void)connectionComplete:(IOBluetoothDevice *)device
                    status:(IOReturn)status;
@end

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


// ---------------------------------------------------------------------------------------------------------------------

BluetoothDevice::BluetoothDevice(BluetoothController *controller,
                                 unique_ptr<BluetoothDevicePrivate> &&p)
    : controller_(*controller)
    , address_(QString::fromNSString(p->device.addressString))
    , name_(QString::fromNSString(p->device.name))
    , state_(p->device.isConnected ? Connected : Disconnected)
    , d(::move(p))
{
}

BluetoothDevice::~BluetoothDevice() {}

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
            INFO << u"Successfully disconnected '%1'."_s.arg(name_);
        }
        else
        {
            setState(Connected);
            WARN << u"Failed disconnecting '%1': Status '%2'"_s.arg(name_, status);
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
                DEBG << u"Successfully issued CREATE_CONNECTION command for '%1'."_s.arg(name_);
                setState(Connecting);
            }
            else
            {
                WARN << u"Failed issuing CREATE_CONNECTION command for '%1': Status '%2'"_s.arg(name_, status);
            }
        }
    }
    return {};
}
