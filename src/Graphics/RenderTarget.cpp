//
// Created by Ayaan on 2026-09-07.
//

#include "Q-Tip/Graphics/RenderTarget.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

#include "Q-Tip/Graphics/Renderer.h"
#include "Q-Tip/Logger/Log.h"
#include "include/Helpers.h"

QTIP_CODE_BEGIN
    RenderTarget::RenderTarget(Renderer& renderer, int width, int height) {
    _texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    if (!_texture) {
        QTipLog(fmt("SDL_CreateTexture(..., SDL_TEXTUREACCESS_TARGET, ...) failed: %s", SDL_GetError()), LOG_ERROR);
        return;
    }

    renderer.setTarget(*this);

    renderer.setRenderColor({0, 0, 0, 0});
    renderer.clear();

    renderer.resetTarget();
}

RenderTarget::RenderTarget(Renderer& renderer, Point size) {
    _texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
    if (!_texture) {
        QTipLog(fmt("SDL_CreateTexture(..., SDL_TEXTUREACCESS_TARGET, ...) failed: %s", SDL_GetError()), LOG_ERROR);
        return;
    }

    renderer.setTarget(*this);

    renderer.setRenderColor({0, 0, 0, 0});
    renderer.clear();

    renderer.resetTarget();
}

QTIP_CODE_END
