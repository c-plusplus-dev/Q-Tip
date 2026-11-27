//
// Created by Ayaan on 2026-08-23.
//

#include "Q-Tip/QTipRuntime.h"

#include <SDL3_ttf/SDL_ttf.h>

#include "include/Helpers.h"
#include <Q-Tip/Logger/Log.h>

#include "include/Q-TipHelpers.h"
#include "include/stdHelpers.h"
#include "Q-Tip/Mods/ModLoader/ModLoader.h"
#include "Q-Tip/Window/Window.h"

namespace {
    std::unordered_map<GamepadID, SDL_Gamepad*> gamepads;
}

QTIP_CODE_BEGIN

QTipRuntime QTipRuntime::instance;

QTipRuntime::QTipRuntime() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        QTipLog(fmt("SDL_Init failed: %s", SDL_GetError()), LOG_FATAL);
        exit(1);
    }

    if (!TTF_Init()) {
        QTipLog(fmt("TTF_Init failed: %s", SDL_GetError()), LOG_FATAL);
        exit(1);
    }

    SDL_SetGamepadEventsEnabled(true);
}

QTipRuntime::~QTipRuntime() {
    TTF_Quit();

    for (auto& [id, gamepad] : gamepads)
        SDL_CloseGamepad(gamepad);

    gamepads.clear();

    SDL_Quit();
}

void QTipRuntime::fetchEvents() {
    for (Window* window : instance._windows)
        window->beginFrame();

    SDL_Event e;
    instance._events.clear();
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_GAMEPAD_ADDED) {
            const auto id = e.gdevice.which;

            if (!gamepads.contains(id)) {
                SDL_Gamepad* gamepad = SDL_OpenGamepad(id);

                if (!gamepad) {
                    QTipLog(
                        fmt("Failed to open gamepad: %s", SDL_GetError()),
                        LOG_WARNING
                    );
                } else {
                    gamepads.emplace(id, gamepad);
                }
            }
        }

        else if (e.type == SDL_EVENT_GAMEPAD_REMOVED) {
            const auto id = e.gdevice.which;

            if (auto it = gamepads.find(id); it != gamepads.end()) {
                SDL_CloseGamepad(it->second);
                gamepads.erase(it);
            }
        }

        if (auto event = translateEvent(e)) {
            instance._events.push_back(event.value());
        } else
            continue;
        auto& event = instance._events.back();

        ModLoader::handleEvent(event);

        if (e.type == SDL_EVENT_QUIT) {
            for (Window* window : instance._windows)
                window->handleEvent(event);

            continue;
        }

        std::uint32_t windowID = event.windowID;

        for (Window* window : instance._windows) {
            if (window->_id == windowID) {
                window->handleEvent(event);
                break;
            }
        }
    }
}

std::vector<Window*> QTipRuntime::windows() {
    return instance._windows;
}

std::optional<Event> QTipRuntime::pollEvent() {
    if (instance._events.empty())
        return std::nullopt;
    return Detail::extract_front(instance._events);
}

const Event* QTipRuntime::peekEvent() {
    if (instance._events.empty())
        return nullptr;
    return &instance._events.front();
}

void QTipRuntime::setErrorMode(ErrorMode mode) {
    instance._mode = mode;
}

ErrorMode QTipRuntime::getErrorMode() {
    return instance._mode;
}

void QTipRuntime::reportError(
    const char* title,
    const char* message,
    ErrorLevel level
) {
    if (level == ErrorLevel::Warning) {
        QTipLog(message, LOG_WARNING);
        return;
    }

    if (level == ErrorLevel::Error) {
        QTipLog(message, LOG_ERROR);
        return;
    }

    // Fatal error
    switch (instance._mode) {
    case ErrorMode::DialogAndExit:
        if (!SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            title,
            message,
            nullptr
        )) {
            QTipLog(
                "Could not display fatal error dialog.",
                LOG_ERROR
            );
            QTipLog(message, LOG_FATAL);
        }

        std::exit(EXIT_FAILURE);

    case ErrorMode::Exit:
        QTipLog(message, LOG_FATAL);
        std::exit(EXIT_FAILURE);

    case ErrorMode::Dialog:
        if (!SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            title,
            message,
            nullptr
        )) {
            QTipLog(message, LOG_FATAL);
        }
        return;

    case ErrorMode::Continue:
        QTipLog(message, LOG_FATAL);
        return;

    case ErrorMode::Ignore:
        return;
    }
}

void QTipRuntime::gamepadReceivesEventsInBackground(bool receives) {
    SDL_SetHint(
        SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,
        receives ? "1" : "0"
    );
}

void QTipRuntime::registerWindow(Window* window) {
    instance._windows.push_back(window);
}

void QTipRuntime::unregisterWindow(Window* window) {
    std::erase(instance._windows, window);
}

void QTipRuntime::replaceWindow(Window* oldWindow, Window* newWindow) {
    const auto it = std::ranges::find(instance._windows, oldWindow);

    if (it != instance._windows.end()) {
        *it = newWindow;
        return;
    }

    instance._windows.push_back(newWindow);
}

std::optional<Event> QTipRuntime::pollEvent(const Window* window) {
    for (auto it = instance._events.begin(); it != instance._events.end(); ++it) {
        if (it->windowID == window->_id)
            return Detail::extract(instance._events, it);
    }

    return std::nullopt;
}

const Event* QTipRuntime::peekEvent(const Window* window) {
    for (auto& _event : instance._events) {
        if (_event.windowID == window->_id)
            return &_event;
    }

    return nullptr;
}

QTIP_CODE_END
