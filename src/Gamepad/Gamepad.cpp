//
// Created by Ayaan on 2026-11-26.
//

#include "Q-Tip/Gamepad/Gamepad.h"

QTIP_CODE_BEGIN

Gamepad::Gamepad(GamepadID id)
    : _id(id),
      _connected(true) {
}

bool Gamepad::connected() const {
    return _connected;
}

GamepadID Gamepad::id() const {
    return _id.value();
}

bool Gamepad::buttonDown(GamepadButton button) const {
    return _buttons[static_cast<std::size_t>(button)];
}

bool Gamepad::buttonPressed(GamepadButton button) const {
    return _pressed[static_cast<std::size_t>(button)];
}

bool Gamepad::buttonReleased(GamepadButton button) const {
    return _released[static_cast<std::size_t>(button)];
}

float Gamepad::axis(GamepadAxis axis) const {
    return _axes[static_cast<std::size_t>(axis)];
}

void Gamepad::setDeadzone(float deadzone) {
    _deadzone = std::clamp(deadzone, 0.0f, 0.99f);
}

float Gamepad::deadzone() const {
    return _deadzone;
}

Point Gamepad::applyDeadzone(Point stick) const {
    const float magnitude = std::sqrt(
        stick.x * stick.x +
        stick.y * stick.y
    );

    if (magnitude <= _deadzone)
        return {0.0f, 0.0f};

    const float adjustedMagnitude =
        (magnitude - _deadzone) / (1.0f - _deadzone);

    const float scale = adjustedMagnitude / magnitude;

    return {
        stick.x * scale,
        stick.y * scale
    };
}

Point Gamepad::leftStick() const {
    return applyDeadzone({
        axis(GamepadAxis::LeftX),
        axis(GamepadAxis::LeftY)
    });
}

Point Gamepad::rightStick() const {
    return applyDeadzone({
        axis(GamepadAxis::RightX),
        axis(GamepadAxis::RightY)
    });
}

float Gamepad::leftTrigger() const {
    return axis(GamepadAxis::LeftTrigger);
}

float Gamepad::rightTrigger() const {
    return axis(GamepadAxis::RightTrigger);
}

void Gamepad::reset() {
    _buttons.fill(false);
    _pressed.fill(false);
    _released.fill(false);
    _axes.fill(0.0f);
}

void Gamepad::beginFrame() {
    _pressed.fill(false);
    _released.fill(false);
}

void Gamepad::handleEvent(const Event& event) {
    switch (event.type) {
        case EventType::GamepadConnected: {
            if (event.gamepadConnection.gamepadID == _id) {
                _connected = true;
                reset();
            }

            break;
        }

        case EventType::GamepadDisconnected: {
            if (event.gamepadConnection.gamepadID == _id) {
                _connected = false;
                reset();
            }

            break;
        }

        case EventType::GamepadButtonPressed: {
            if (event.gamepadButton.gamepadID != _id)
                break;

            const auto index =
                static_cast<std::size_t>(event.gamepadButton.button);

            if (!_buttons[index]) {
                _pressed[index] = true;
            }

            _buttons[index] = true;

            break;
        }

        case EventType::GamepadButtonReleased: {
            if (event.gamepadButton.gamepadID != _id)
                break;

            const auto index =
                static_cast<std::size_t>(event.gamepadButton.button);

            if (_buttons[index]) {
                _released[index] = true;
            }

            _buttons[index] = false;

            break;
        }

        case EventType::GamepadAxisMoved: {
            if (event.gamepadAxis.gamepadID != _id)
                break;

            const auto index =
                static_cast<std::size_t>(event.gamepadAxis.axis);

            _axes[index] = event.gamepadAxis.value;

            break;
        }

        default:
            break;
    }
}

QTIP_CODE_END
