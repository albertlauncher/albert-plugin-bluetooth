// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "albert/logging.h"
#include "bluetoothcontrollerprivate.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
#include <Foundation/Foundation.h>
#include <IOBluetooth/IOBluetooth.h>
#include <ranges>
#include <shared_mutex>
using enum BluetoothDevice::State;
// using namespace albert;
using namespace std;
#if  ! __has_feature(objc_arc)
#error This file must be compiled with ARC.
#endif

extern "C" {
int IOBluetoothPreferencesAvailable();
int IOBluetoothPreferenceGetControllerPowerState();
void IOBluetoothPreferenceSetControllerPowerState(int state);
int IOBluetoothPreferenceGetDiscoverableState();
void IOBluetoothPreferenceSetDiscoverableState(int state);
}


@interface BluetoothPowerDelegate : NSObject
- (instancetype)initWith:(function<void(bool)>)callback;
@end


@interface BluetoothConnectionObserver : NSObject
- (instancetype)initWith:(function<void(IOBluetoothDevice*, bool)>)cb;
@end


BluetoothControllerPrivate::BluetoothControllerPrivate(IOBluetoothHostController *c) :
    BluetoothControllerPrivateBase(
          QString::fromNSString(c.addressAsString),
          QString::fromNSString(c.nameAsString),
          c.powerState
    ),
    controller(c)
{}

BluetoothController::BluetoothController():
    d(make_unique<BluetoothControllerPrivate>([IOBluetoothHostController defaultController]))
{
    auto &p = *static_cast<BluetoothControllerPrivate*>(d.get());

    auto power_callback = [this](bool v){ setPoweredOn(v); };
    p.power_delegate = [[BluetoothPowerDelegate alloc] initWith:power_callback];

    auto connection_callback = [this](IOBluetoothDevice *device, bool v)
    {
        auto &p = *static_cast<BluetoothControllerPrivate*>(d.get());
        if (auto it = p.devices.find(device); it == p.devices.end())
        {
            unique_lock lock(p.lock);
            p.devices.emplace(device, make_shared<BluetoothDevice>(
                                          make_unique<BluetoothDevicePrivate>(*this, device)));
        }
        else
        {
            auto state = v ? Connected : Disconnected;
            it->second->setState(state);
        }
    };
    p.connection_observer = [[BluetoothConnectionObserver alloc] initWith:connection_callback];

    // Initially create devices for all paired devices
    for (IOBluetoothDevice *device : [IOBluetoothDevice pairedDevices])
        p.devices.emplace(device, make_shared<BluetoothDevice>(
                                      make_unique<BluetoothDevicePrivate>(*this, device)));
}

BluetoothController::~BluetoothController() {}

void BluetoothController::toggle()
{
    IOBluetoothPreferenceSetControllerPowerState(poweredOn() ? 0 : 1);
}

vector<shared_ptr<BluetoothDevice>> BluetoothController::devices()
{
    auto &p = *static_cast<BluetoothControllerPrivate*>(d.get());
    shared_lock lock(p.lock);
    auto v = views::values(p.devices);
    return vector<shared_ptr<BluetoothDevice>>{v.begin(), v.end()};
}


// ------------------------------------------------------------------------------------------------
// Do _not_ move upwards. Interface implementation confuses lupdate.
// ------------------------------------------------------------------------------------------------


@implementation BluetoothPowerDelegate {
    function<void(bool)> _callback;
}

- (instancetype)initWith:(function<void(bool)>)callback
{
    self = [super init];
    if (self) {
        _callback = callback;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(bluetoothPowerOn:)
                                                     name:IOBluetoothHostControllerPoweredOnNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(bluetoothPowerOff:)
                                                     name:IOBluetoothHostControllerPoweredOffNotification
                                                   object:nil];
    }
    return self;
}

- (void)bluetoothPowerOn:(NSNotification *)notification { _callback(true); }
- (void)bluetoothPowerOff:(NSNotification *)notification { _callback(false); }
- (void)dealloc{ [[NSNotificationCenter defaultCenter] removeObserver:self]; }

@end


@implementation BluetoothConnectionObserver {
    function<void(IOBluetoothDevice *device, bool)> callback;
    IOBluetoothUserNotification *connect_notification;
    map<IOBluetoothDevice*, IOBluetoothUserNotification*> disconnect_notifications;
}

- (instancetype)initWith:(function<void(IOBluetoothDevice *device, bool)>)_callback
{
    self = [super init];
    if (self) {
        callback = _callback;

        // Sends connect notificatinos for all connected devices
        connect_notification =
            [IOBluetoothDevice registerForConnectNotifications:self
                                                      selector:@selector(deviceConnected:device:)];

        if (!connect_notification)
            CRIT << "Failed to register connect notifications.";
    }
    return self;
}

- (void)dealloc
{
    [connect_notification unregister];
    for (const auto&[d, n] : disconnect_notifications)
        [n unregister];
}

- (void)deviceConnected:(IOBluetoothUserNotification *)notification
                 device:(IOBluetoothDevice *)device
{
    if (auto it = disconnect_notifications.find(device);
        it != disconnect_notifications.end())
    {
        WARN << "Connect notification received multiple times"
             << QString::fromNSString(device.name)
             << QString::fromNSString(device.addressString);
    }
    else
    {
        DEBG << "Connect notification for"
             << QString::fromNSString(device.name)
             << QString::fromNSString(device.addressString);

        auto disconnect_notification =
            [device registerForDisconnectNotification:self
                                             selector:@selector(deviceDisconnected:device:)];

        if (disconnect_notification)
            // will succeed since we checked above that it does not exist
            disconnect_notifications.emplace(device, notification);
        else
            WARN << "Failed to register disconnect notification for"
                 << QString::fromNSString(device.name)
                 << QString::fromNSString(device.addressString);

        callback(device, true);
    }
}

- (void)deviceDisconnected:(IOBluetoothUserNotification *)notification
                    device:(IOBluetoothDevice *)device
{
    DEBG << "Disconnect notification for"
         << QString::fromNSString(device.name)
         << QString::fromNSString(device.addressString);

    if (auto it = disconnect_notifications.find(device);
        it == disconnect_notifications.end())
        WARN << "Disconnect notification received multiple times"
             << QString::fromNSString(device.name)
             << QString::fromNSString(device.addressString);
    else
    {
        [it->second unregister];
        disconnect_notifications.erase(it);
        callback(device, false);
    }
}

@end
