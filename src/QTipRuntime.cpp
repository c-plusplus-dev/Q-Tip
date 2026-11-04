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

QTIP_CODE_BEGIN

QTipRuntime QTipRuntime::instance;

QTipRuntime::QTipRuntime() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        QTipLog(fmt("SDL_Init failed: %s", SDL_GetError()), LOG_FATAL);
        exit(1);
    }

    if (!TTF_Init()) {
        QTipLog(fmt("TTF_Init failed: %s", SDL_GetError()), LOG_FATAL);
        exit(1);
    }
}

QTipRuntime::~QTipRuntime() {
    TTF_Quit();
    SDL_Quit();
}

void QTipRuntime::fetchEvents() {
    for (Window* window : instance._windows)
        window->beginFrame();

    SDL_Event e;
    instance._events.clear();
    while (SDL_PollEvent(&e)) {
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
