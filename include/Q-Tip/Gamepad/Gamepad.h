//
// Created by Ayaan on 2026-11-26.
//

#ifndef QTIP_GAMEPAD_H
#define QTIP_GAMEPAD_H

#include "Q-Tip/Config.h"
#include "Q-Tip/Events/Event.h"
#include <array>
#include <optional>

QTIP_CODE_BEGIN

class Gamepad {
public:
    Gamepad() = default;

    explicit Gamepad(GamepadID id);

    [[nodiscard]] bool connected() const;

    [[nodiscard]] GamepadID id() const;

    [[nodiscard]] bool buttonDown(GamepadButton button) const;
    [[nodiscard]] bool buttonPressed(GamepadButton button) const;
    [[nodiscard]] bool buttonReleased(GamepadButton button) const;

    [[nodiscard]] float axis(GamepadAxis axis) const;

    void setDeadzone(float deadzone);
    [[nodiscard]] float deadzone() const;

    [[nodiscard]] Point leftStick() const;
    [[nodiscard]] Point rightStick() const;

    [[nodiscard]] float leftTrigger() const;
    [[nodiscard]] float rightTrigger() const;

    void handleEvent(const Event& event);

    void beginFrame();

    void reset();

private:
    static constexpr std::size_t ButtonCount =
        static_cast<std::size_t>(GamepadButton::Count);

    static constexpr std::size_t AxisCount =
        static_cast<std::size_t>(GamepadAxis::Count);

    std::optional<GamepadID> _id;

    std::array<bool, ButtonCount> _buttons{};
    std::array<bool, ButtonCount> _pressed{};
    std::array<bool, ButtonCount> _released{};

    std::array<float, AxisCount> _axes{};

    bool _connected = false;

    float _deadzone = 0.15f;

    [[nodiscard]] Point applyDeadzone(Point stick) const;
};

QTIP_CODE_END

#endif //QTIP_GAMEPAD_H
