// Copyright (c) 2024-2025 Manuel Schneider

#include "bluetoothdevice.h"
#include <albert/logging.h>
#include <albert/networkutil.h>
#include <albert/notification.h>
using enum BluetoothDevice::State;
using namespace Qt::StringLiterals;
using namespace albert::util;
using namespace std;

BluetoothDevice::BluetoothDevice(BluetoothController *controller,
                                 const QString &id,
                                 const QString &name,
                                 State state) :
    controller_(*controller),
    id_(id),
    name_(name),
    state_(state)
{
}

BluetoothDevice::~BluetoothDevice() {}

void BluetoothDevice::setName(const QString &n)
{
    if (name_ != n)
    {
        name_ = n;
        DEBG << u"Bluetooth device name changed to '%1' (%2)"_s.arg(name(), address());
        emit nameChanged(name_);
    }
}

void BluetoothDevice::setState(State s)
{
    if (state_ != s)
    {
        notification_.reset();  // make sure notification does not dislay outdated information

        state_ = s;
        DEBG << u"Bluetooth device state changed to '%1' (%2 %3)"_s.arg(stateString(), name(), address());
        emit stateChanged(s);
    }
}

QString BluetoothDevice::stateString() const
{
    switch (state_) {
    case Connected: return tr("Connected");
    case Connecting: return tr("Connecting");
    case Disconnected: return tr("Disconnected");
    case Disconnecting: return tr("Disconnecting");
    }
    return {};
}

static inline QStringList buildIconUrlFromEmoji(const QString &emoji)
{ return {u"text:?text="_s + percentEncoded(emoji)}; }

QStringList BluetoothDevice::deviceIconUrls()
{
    // https://www.ampedrftech.com/datasheets/cod_definition.pdf
    const auto class_of_device = classOfDevice();

    switch (((class_of_device) & 0b0001111100000000) >> 8) {
    case 0x01: // Computer (desktop,notebook, PDA, organizers, .... )
        switch (((class_of_device) & 0b011111100) >> 2) {
        case 0x03:  // Laptop.
            return buildIconUrlFromEmoji(u"💻"_s);
        case 0x04:  // Handheld PC/PDA (clam shell).
        case 0x05:  // Palm-size PC/PDA.
            return buildIconUrlFromEmoji(u"📱"_s);
        case 0x06:  // Wearable computer (watch size).
            return buildIconUrlFromEmoji(u"⌚"_s);
        case 0x01:  // Desktop workstation.
        case 0x02:  // Server-class computer.
        default:
            return buildIconUrlFromEmoji(u"🖥️"_s);
        }

    case 0x02: // Phone (cellular, cordless, payphone, modem, ...)
        switch (((class_of_device) & 0b11111100) >> 2) {
        case 0x01:  // Cellular.
        case 0x03:  // Smart phone.
            return buildIconUrlFromEmoji(u"📱"_s);
        case 0x02:  // Cordless.
        case 0x04:  // Wired modem or voice gateway.
        case 0x05:  // Common ISDN access.
        default:
            return buildIconUrlFromEmoji(u"📞"_s);
        }

    case 0x03:  // LAN/Network Access point
        return buildIconUrlFromEmoji(u"🛜"_s);

    case 0x04:  // Audio/Video (headset,speaker,stereo, video display, vcr…
        switch (((class_of_device) & 0b11111100) >> 2) {
        case 0b00001:  // Wearable Headset Device.
            return buildIconUrlFromEmoji(u"🎧"_s);
        case 0b00010: // Hands-free Device.
            return buildIconUrlFromEmoji(u"🗣️"_s);
        //case 0b00011:  // (Reserved)
        case 0b00100:  // Microphone
            return buildIconUrlFromEmoji(u"🎤"_s);
        case 0b00101:  // Loudspeaker
            return buildIconUrlFromEmoji(u"🔊"_s);
        case 0b00110:  // Headphones
            return buildIconUrlFromEmoji(u"🎧"_s);
        case 0b00111:  // Portable Audio
            return buildIconUrlFromEmoji(u"🔊"_s);
        case 0b01000:  // Car audio
            return buildIconUrlFromEmoji(u"🚗🔊"_s);
        case 0b01001:  // Set-top box
            return buildIconUrlFromEmoji(u"📺"_s);
        case 0b01010:  // HiFi Audio Device
            return buildIconUrlFromEmoji(u"🎵"_s);
        case 0b01011:  // VCR
            return buildIconUrlFromEmoji(u"📼"_s);
        case 0b01100:  // Video Camera
            return buildIconUrlFromEmoji(u"📹"_s);
        case 0b01101:  // Camcorder
            return buildIconUrlFromEmoji(u"📷"_s);
        case 0b01110:  // Video Monitor
        case 0b01111:  // Video Display and Loudspeaker
        case 0b10000:  // Video Conferencing
            return buildIconUrlFromEmoji(u"📺"_s);
        // case 0b10001:  // (Reserved)
        case 0b10010:  // Gaming/Toy
            return buildIconUrlFromEmoji(u"🧸"_s);
        default:
            return buildIconUrlFromEmoji(u"🎵"_s);
        }

    case 0x05:  // Peripheral (mouse, joystick, keyboards, ...)
        switch ((class_of_device & 0b11000000) >> 6) {
        case 0x00:  // "Not a keyboard or pointing device."
            switch ((class_of_device & 0x00111100) >> 2) {
            case 0x01: // Joystick.
                return buildIconUrlFromEmoji(u"🕹️"_s);
            case 0x02: // Gamepad.
                return buildIconUrlFromEmoji(u"🎮"_s);
            default:
                return buildIconUrlFromEmoji(u"❔"_s);
            }
        case 0x01:  // Keyboard.
            return buildIconUrlFromEmoji(u"⌨️"_s);
        case 0x02:  // Pointing device.
            switch ((class_of_device & 0b00111100) >> 2) {
            case 0x05: // Digitizer tablet.
                return buildIconUrlFromEmoji(u"✍️"_s);
            default: // Mouse.
                return buildIconUrlFromEmoji(u"🖱️"_s);
            }
        case 0x03:  // Combo device.
            return buildIconUrlFromEmoji(u"⌨️🖱️"_s);
        default:
            return buildIconUrlFromEmoji(u"❔"_s);
        }

    case 0x06:  // Imaging (printer, scanner, camera, display, ...)
        return buildIconUrlFromEmoji(u"🖼️"_s);

    case 0x07:  // Wearable (watch, pager, ...)
        switch (((class_of_device) & 0b11111100) >> 2) {
        case 2:  // Pager.
            return buildIconUrlFromEmoji(u"📟"_s);
        case 3:  // Jacket.
            return buildIconUrlFromEmoji(u"🧥"_s);
        case 4:  // Helmet.
            return buildIconUrlFromEmoji(u"🪖"_s);
        case 5:  // Glasses.
            return buildIconUrlFromEmoji(u"🕶️"_s);
        case 1:  // Wrist watch.
        default:
            return buildIconUrlFromEmoji(u"⌚"_s);
        }

    case 0x08:  // Toy (robot, vehicle, ...)
        switch (((class_of_device) & 0b11111100) >> 2) {
        case 0x01:  // Robot
            return buildIconUrlFromEmoji(u"🤖"_s);
        case 0x02:  // Vehicle
            return buildIconUrlFromEmoji(u"🚗"_s);
        case 0x03:  // Doll / Action figure
            return buildIconUrlFromEmoji(u"🧸"_s);
        case 0x04:  // Controller
            return buildIconUrlFromEmoji(u"🎮"_s);
        case 0x05:  // Game
        default:
            return buildIconUrlFromEmoji(u"🪀"_s);
        }

    case 0x09:
        // Health (heart rate sensor, blood pressure, thermometers, ...)
        switch (((class_of_device) & 0b11111100) >> 2) {
        // case 1:  // Blood Pressure Monitor
        case 2:  // Thermometer
            return buildIconUrlFromEmoji(u"🌡️"_s);
        // case 3:  // Weighing Scale
        // case 4:  // Glucose Meter
        // case 5:  // Pulse Oximeter
        // case 6:  // Heart/Pulse Rate Monitor
        // case 7:  // Health Data Display
        default:
            return buildIconUrlFromEmoji(u"❤️"_s);
        }

    default:
        return buildIconUrlFromEmoji(u"❔"_s);
    }
}

void BluetoothDevice::connectDeviceCallback(optional<QString> error)
{
    if (error)
    {
        WARN << u"Failed to connect device: '%1'. Error: '%2'"_s.arg(name(), error.value());
        setState(Disconnected);
        sendStateNotification(tr("Connecting device failed"));
    }
    else
    {
        INFO << u"Successfully connected device: '%1'"_s.arg(name());
        setState(Connected);
        sendStateNotification(tr("Connected"));
    }
}

void BluetoothDevice::disconnectDeviceCallback(optional<QString> error)
{
    if (error)
    {
        WARN << u"Failed to disconnect device: '%1'. Error: '%2'"_s.arg(name(), error.value());
        setState(Connected);
        sendStateNotification(tr("Disconnecting device failed"));
    }
    else
    {
        INFO << u"Successfully disconnected device: '%1'"_s.arg(name());
        setState(Disconnected);
        sendStateNotification(tr("Disconnected"));
    }
}

void BluetoothDevice::sendStateNotification(const QString &msg)
{
    notification_ = make_unique<Notification>(name(), msg);
    auto delete_notification = [this]{ notification_.reset(); };
    connect(notification_.get(), &Notification::activated, this, delete_notification);
    notification_->send();
}
