//
// Created by Ayaan on 2026-09-10.
//

#include "Q-Tip/Mods/ModLoader/ModLoader.h"

#include "Q-Tip/Events/Event.h"
#include "Q-Tip/Mods/Mod.h"

QTIP_CODE_BEGIN

ModLoader ModLoader::instance;

ModLoader::~ModLoader() {
    for (const auto& mod : mods) {
        mod->shutdown();
    }
}

void ModLoader::handleEvent(const Event& event) {
    for (const auto& mod : instance.mods) {
        mod->handleEvent(event);
    }
}

void ModLoader::beforePresent(Window& window) {
    for (const auto& mod : instance.mods) {
        mod->beforePresent(window);
    }
}

void ModLoader::windowCreated(Window& window) {
    for (const auto& mod : instance.mods) {
        mod->windowCreated(window);
    }
}

void ModLoader::windowDestroyed(Window& window) {
    for (const auto& mod : instance.mods) {
        mod->windowDestroyed(window);
    }
}

QTIP_CODE_END
