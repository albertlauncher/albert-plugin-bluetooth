// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothcontroller.h"
#include "bluetoothcontrollerprivate.h"
#include "bluetoothdevice.h"
#include "bluetoothdeviceprivate.h"
#include <IOBluetooth/IOBluetooth.h>
#include <albert/logging.h>
using enum BluetoothDevice::State;
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


// ---------------------------------------------------------------------------------------------------------------------

@interface BluetoothPowerDelegate : NSObject
- (instancetype)initWith:(function<void(bool)>)callback;
- (void)dealloc;
@end

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


// ---------------------------------------------------------------------------------------------------------------------

@interface BluetoothConnectionObserver : NSObject
- (instancetype)initWith:(function<void(IOBluetoothDevice*, bool)>)cb;
- (void)dealloc;
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


// ---------------------------------------------------------------------------------------------------------------------

unique_ptr<BluetoothController> defaultController()
{
    const auto c = [IOBluetoothHostController defaultController];
    return make_unique<BluetoothController>(make_unique<BluetoothControllerPrivate>(c));
}


// ---------------------------------------------------------------------------------------------------------------------

BluetoothController::BluetoothController(unique_ptr<BluetoothControllerPrivate> &&p)
    : address_(QString::fromNSString(p->controller.addressAsString))
    , name_(QString::fromNSString(p->controller.nameAsString))
    , powered_on_(p->controller.powerState == kBluetoothHCIPowerStateON)
    , d(::move(p))
{
    // auto &p = *static_cast<BluetoothControllerPrivate*>(d.get());

    auto power_callback = [this](bool v){ setPoweredOn(v); };
    d->power_delegate = [[BluetoothPowerDelegate alloc] initWith:power_callback];

    auto connection_callback = [this](IOBluetoothDevice *device, bool connected) {
        const auto address = QString::fromNSString(device.addressString);
        if (auto it = devices_.find(address); it == devices_.end())
            // New device, 'connected' should be always true here
            addDevice(deviceFromNative(this, device));
        else
            // Device already exists
            it->second->setState(connected ? Connected : Disconnected);
    };
    d->connection_observer = [[BluetoothConnectionObserver alloc] initWith:connection_callback];

    // Initially create devices for all paired devices
    for (IOBluetoothDevice *device : [IOBluetoothDevice pairedDevices])
        addDevice(deviceFromNative(this, device));
}

BluetoothController::~BluetoothController() {}

void BluetoothController::toggle() { IOBluetoothPreferenceSetControllerPowerState(poweredOn() ? 0 : 1); }

// ---------------------------------------------------------------------------------------------------------------------
// Do _not_ move upwards. Interface implementation confuses lupdate.
