//
// Created by Ayaan on 2026-11-26.
//

#ifndef QTIP_GAMEPADMANAGER_H
#define QTIP_GAMEPADMANAGER_H

#include "Gamepad.h"
#include "Q-Tip/Config.h"
#include <vector>

QTIP_CODE_BEGIN

class GamepadManager {
public:
    GamepadManager();

    void beginEvents() const;

    void handleEvent(const Event& event);

    [[nodiscard]] Gamepad* primaryGamepad() const;

    [[nodiscard]] Gamepad* gamepad(GamepadID id) const;
    [[nodiscard]] Gamepad* gamepad(std::size_t index) const;

private:
    [[nodiscard]] bool exists(GamepadID id) const;

    std::vector<std::unique_ptr<Gamepad>> _gamepads;
};

QTIP_CODE_END

#endif //QTIP_GAMEPADMANAGER_H
