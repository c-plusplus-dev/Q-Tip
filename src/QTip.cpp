//
// Created by Ayaan on 2026-08-23.
//

#include "Q-Tip/QTip.h"

#include <iostream>
#include <SDL3_ttf/SDL_ttf.h>

#include "include/Helpers.h"
#include <Q-Tip/Logger/Log.h>

#include "include/Q-TipHelpers.h"
#include "Q-Tip/Mods/ModLoader/ModLoader.h"

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

void QTipRuntime::pollEvents() {
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

QTIP_CODE_END
