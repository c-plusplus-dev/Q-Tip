//
// Created by Ayaan on 2026-11-02.
//

#ifndef QTIP_EVENTS_H
#define QTIP_EVENTS_H

#include <array>
#include <cstdint>

#include "Q-Tip/Config.h"
#include "Q-Tip/Math/Point.h"

#include "EventType.h"
#include "Q-Tip/Window/Keycode.h"
#include "Q-Tip/Window/Scancode.h"

QTIP_CODE_BEGIN
    enum class DisplayOrientation {
    Unknown,
    Landscape,
    LandscapeFlipped,
    Portrait,
    PortraitFlipped
};

struct DisplayEvent {
    std::uint32_t displayID;

    std::int32_t width;
    std::int32_t height;

    float refreshRate;

    DisplayOrientation orientation;
};

struct WindowEvent {
    Point position;

    std::int32_t width;
    std::int32_t height;
};

struct KeyEvent {
    Keycode key;
    Scancode scancode;

    std::uint16_t modifiers;

    bool repeat;
};

struct TextInputEvent {
    std::array<char, 256> text;
    std::uint32_t length;
};

struct MouseMoveEvent {
    Point position;
    Point delta;

    std::uint32_t buttons;
};

typedef enum MouseButton {
    Left = 1,
    Middle,
    Right,
    X1,
    X2
} MouseButton;

struct MouseButtonEvent {
    Point position;

    MouseButton button;
    std::uint8_t clicks;
};

struct MouseScrollEvent {
    Point position;

    Point offset;
};

struct FileDropEvent {
    std::array<char, 1024> path;
    std::uint32_t length;

    Point position;
};

struct ClipboardEvent {
    bool containsText;
};

enum class GamepadButton : std::uint8_t {
    South,
    East,
    West,
    North,

    Back,
    Guide,
    Start,

    LeftStick,
    RightStick,

    LeftShoulder,
    RightShoulder,

    DPadUp,
    DPadDown,
    DPadLeft,
    DPadRight,

    Miscellaneous1,
    RightPaddle1,
    LeftPaddle1,
    RightPaddle2,
    LeftPaddle2,

    Touchpad,

    Miscellaneous2,
    Miscellaneous3,
    Miscellaneous4,
    Miscellaneous5,
    Miscellaneous6,

    Count
};

enum class GamepadAxis : std::uint8_t {
    LeftX,
    LeftY,
    RightX,
    RightY,

    LeftTrigger,
    RightTrigger,

    Count
};

typedef std::uint32_t GamepadID;

struct GamepadConnectionEvent {
    GamepadID gamepadID;
};

struct GamepadButtonEvent {
    GamepadID gamepadID;
    GamepadButton button;
};

struct GamepadAxisEvent {
    GamepadID gamepadID;
    GamepadAxis axis;

    float value;
};

//struct TouchEvent {
//    std::uint64_t touchID;
//    std::uint64_t fingerID;

//    Point position;
//    Point delta;

//    float pressure;
//};

//struct PenEvent {
//    std::uint32_t penID;

//    Point position;
//    Point tilt;

//    float pressure;
//};

// Main event
struct Event {
    EventType type;

    std::uint32_t windowID;

    std::uint64_t timestamp;

    union {
        DisplayEvent display;
        WindowEvent window;

        KeyEvent key;
        TextInputEvent textInput;

        MouseMoveEvent mouseMove;
        MouseButtonEvent mouseButton;
        MouseScrollEvent mouseScroll;

        FileDropEvent fileDrop;
        ClipboardEvent clipboard;

        GamepadConnectionEvent gamepadConnection;
        GamepadButtonEvent gamepadButton;
        GamepadAxisEvent gamepadAxis;

        //TouchEvent touch;
        //PenEvent pen;
    };
};

QTIP_CODE_END

#endif // QTIP_EVENTS_H