//
// Created by Ayaan on 2026-09-07.
//

#include "Q-Tip/Graphics/RenderTarget.h"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

#include "Q-Tip/Window/Window.h"
#include "Q-Tip/Logger/Log.h"
#include "include/Helpers.h"

QTIP_CODE_BEGIN

RenderTarget::RenderTarget(Window& window, int width, int height) {
    auto& renderer = window.getRenderer();
    _texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    if (!_texture) {
        QTipLog(fmt("SDL_CreateTexture(..., SDL_TEXTUREACCESS_TARGET, ...) failed: %s", SDL_GetError()), LOG_ERROR);
        return;
    }
    SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);

    renderer.setTarget(*this);

    renderer.setRenderColor({0, 0, 0, 0});
    renderer.clear();

    renderer.resetTarget();
}

RenderTarget::RenderTarget(Window& window, Point size) {
    auto& renderer = window.getRenderer();
    _texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
    if (!_texture) {
        QTipLog(fmt("SDL_CreateTexture(..., SDL_TEXTUREACCESS_TARGET, ...) failed: %s", SDL_GetError()), LOG_ERROR);
        return;
    }
    SDL_SetTextureBlendMode(_texture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);

    renderer.setTarget(*this);

    renderer.setRenderColor({0, 0, 0, 0});
    renderer.clear();

    renderer.resetTarget();
}

QTIP_CODE_END
