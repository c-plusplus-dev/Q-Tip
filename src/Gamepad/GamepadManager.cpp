//
// Created by Ayaan on 2026-11-25.
//

#include "Q-Tip/Gamepad/GamepadManager.h"

QTIP_CODE_BEGIN

GamepadManager::GamepadManager() = default;

void GamepadManager::beginEvents() const {
    for (auto& gamepad : _gamepads) {
        gamepad->beginFrame();
    }
}

void GamepadManager::handleEvent(const Event& event) {
    if (event.type == EventType::GamepadConnected) {
        if (!exists(event.gamepadConnection.gamepadID)) {
            _gamepads.emplace_back(
                std::make_unique<Gamepad>(
                    event.gamepadConnection.gamepadID
                )
            );
        }
    }
    else if (event.type == EventType::GamepadDisconnected) {
        const auto id = event.gamepadConnection.gamepadID;

        std::erase_if(_gamepads, [id](const auto& gamepad) {
            return gamepad->id() == id;
        });
    }

    for (auto& gamepad : _gamepads) {
        gamepad->handleEvent(event);
    }
}

Gamepad* GamepadManager::primaryGamepad() const {
    if (_gamepads.empty())
        return nullptr;

    return _gamepads.front().get();
}

Gamepad* GamepadManager::gamepad(GamepadID id) const {
    for (auto& gamepad : _gamepads) {
        if (gamepad->id() == id)
            return gamepad.get();
    }
    return nullptr;
}

Gamepad* GamepadManager::gamepad(std::size_t index) const {
    if (index >= _gamepads.size())
        return nullptr;

    return _gamepads[index].get();
}

bool GamepadManager::exists(GamepadID id) const {
    return gamepad(id) != nullptr;
}

QTIP_CODE_END
