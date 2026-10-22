//
// Created by Ayaan on 2026-08-23.
//

#include <Q-Tip/Graphics/Renderer.h>
#include <Q-Tip/Graphics/Texture.h>
#include <SDL3/SDL.h>
#include "../include/Helpers.h"
#include <Q-Tip/Logger/Log.h>
#include "earcut.hpp"
#include "Q-Tip/Graphics/RenderTarget.h"
#include "Q-Tip/Mods/ModLoader/ModLoader.h"
#include "Q-Tip/Window/Window.h"


namespace mapbox::util {

    // Register Index 0 (X)
    template <>
    struct nth<0, QTip::Point> {
        static double get(const QTip::Point& p) { return p.x; };
    };

    // Register Index 1 (Y)
    template <>
    struct nth<1, QTip::Point> {
        static double get(const QTip::Point& p) { return p.y; };
    };

}


QTIP_CODE_BEGIN

Renderer::Renderer(SDL_Window* window) {
    if (!window) return;
    _renderer = SDL_CreateGPURenderer(nullptr, window);
    if (!_renderer) {
        QTipLog(fmt("SDL_CreateGPURenderer failed: %s", SDL_GetError()), LOG_FATAL);
        exit(1);
    }
    SDL_SetDefaultTextureScaleMode(
        _renderer,
        SDL_SCALEMODE_NEAREST
    );
}

Renderer::~Renderer() {
    destroy();
}

Renderer::Renderer(Renderer&& other) noexcept
    : _renderer(other._renderer), _window(other._window) {
    other._renderer = nullptr;
    other._window = nullptr;
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    destroy();

    _renderer = other._renderer;
    _window = other._window;
    other._renderer = nullptr;
    other._window = nullptr;

    return *this;
}

void Renderer::destroy() {
    if (!_renderer) return;
    SDL_DestroyRenderer(_renderer);
    _renderer = nullptr;
    _window = nullptr;
}

Renderer::operator SDL_Renderer*() const {
    return _renderer;
}

void Renderer::clear() {
    SDL_RenderClear(_renderer);
}

void Renderer::present() {
    if (_window)
        ModLoader::beforePresent(*_window);

    SDL_RenderPresent(_renderer);
}

void Renderer::setRenderColor(Color color) {
    SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
}

void Renderer::renderPoint(Point p) {
    SDL_RenderPoint(_renderer, p.x, p.y);
}

void Renderer::renderLine(Line line) {
    SDL_RenderLine(_renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

void Renderer::renderThickLine(Line line, float thickness, bool rounded_edges) {
    DrawThickLine(_renderer, line.start.x, line.start.y, line.end.x, line.end.y, thickness);
    if (rounded_edges) {
        renderCircle({line.start, thickness / 2.0f - 1.0f});
        renderCircle({line.end  , thickness / 2.0f - 1.0f});
    }
}

void Renderer::renderRect(Rect rect, bool filled) {
    SDL_FRect sdlRect = rect;

    if (filled)
        SDL_RenderFillRect(_renderer, &sdlRect);
    else
        SDL_RenderRect(_renderer, &sdlRect);
}

void Renderer::renderRoundedRect(Rect rect, int radius) {
    SDL_FRect sdlRect = rect;
    DrawRoundedRect(_renderer, &sdlRect, radius);
}

void Renderer::renderCircle(Circle circle, bool filled) {
    DrawCircle(_renderer, circle.center.x, circle.center.y, circle.radius);
}

void Renderer::renderTriangle(Triangle triangle, bool filled) {
    SDL_Vertex verts[3];
    float r, g, b, a;
    SDL_GetRenderDrawColorFloat(_renderer, &r, &g, &b, &a);
    verts[0] = {{triangle.a.x, triangle.a.y}, {r, g, b, a}, {0, 0}};
    verts[1] = {{triangle.b.x, triangle.b.y}, {r, g, b, a}, {0, 0}};
    verts[2] = {{triangle.c.x, triangle.c.y}, {r, g, b, a}, {0, 0}};
    SDL_RenderGeometry(_renderer, nullptr, verts, 3, nullptr, 0);
}

void Renderer::renderPolygon(Polygon polygon, bool filled)
{
    if (polygon.points.size() < 3)
        return;

    if (!filled) {
        for (size_t i = 0; i < polygon.points.size(); ++i) {
            const Point& a = polygon.points[i];
            const Point& b = polygon.points[(i + 1) % polygon.points.size()];

            renderLine({a, b});
        }

        return;
    }

    std::vector<std::vector<Point>> rings = {
        polygon.points
    };

    const auto indices = mapbox::earcut<uint32_t>(rings);

    for (size_t i = 0; i < indices.size(); i += 3) {
        const Point& a = polygon.points[indices[i]];
        const Point& b = polygon.points[indices[i + 1]];
        const Point& c = polygon.points[indices[i + 2]];

        renderTriangle({a, b, c});
    }
}

void Renderer::renderText(const Font& font, const char* text, float x, float y, Color color) {
    setBlendMode(RenderBlendMode::None);
    ::renderText(font, _renderer, text, color, x, y);
    resetBlendMode();
}

void Renderer::renderTextCentered(const Font& font, const char* text, float x, float y, Color color) {
    setBlendMode(RenderBlendMode::None);
    ::renderTextCentered(font, _renderer, text, color, x, y);
    resetBlendMode();
}

void Renderer::renderTexture(Texture& texture, Rect srcRect, Rect dstRect) {
    SDL_FRect src;
    SDL_FRect dst;
    SDL_FRect* srcPtr = nullptr;
    SDL_FRect* dstPtr = nullptr;
    if (srcRect != Rect::zero) {
        src = srcRect;
        srcPtr = &src;
    }
    if (dstRect != Rect::zero) {
        dst = dstRect;
        dstPtr = &dst;
    }
    SDL_RenderTexture(
        _renderer,
        texture.nativeHandle(),
        srcPtr,
        dstPtr
    );
}

void Renderer::setTarget(RenderTarget& renderTarget) {
    _renderTargetStack.push_back(SDL_GetRenderTarget(_renderer));
    SDL_SetRenderTarget(_renderer, renderTarget.nativeHandle());
}

void Renderer::resetTarget() {
    if (_renderTargetStack.empty()) {
        return;
    }

    SDL_SetRenderTarget(_renderer, _renderTargetStack.back());
    _renderTargetStack.pop_back();
}

void Renderer::setClip(const Rect& clipRect) {
    _clipStack.push_back(_currentClip);

    if (_currentClip.enabled) {
        _currentClip.rect = _currentClip.rect.intersection(clipRect);
    } else {
        _currentClip.rect = clipRect;
        _currentClip.enabled = true;
    }

    SDL_Rect rect{
        static_cast<int>(_currentClip.rect.origin.x),
        static_cast<int>(_currentClip.rect.origin.y),
        static_cast<int>(_currentClip.rect.size.x),
        static_cast<int>(_currentClip.rect.size.y)
    };

    SDL_SetRenderClipRect(_renderer, &rect);
}

void Renderer::resetClip() {
    if (_clipStack.empty()) {
        return;
    }

    _currentClip = _clipStack.back();
    _clipStack.pop_back();

    if (_currentClip.enabled) {
        SDL_Rect rect{
            static_cast<int>(_currentClip.rect.origin.x),
            static_cast<int>(_currentClip.rect.origin.y),
            static_cast<int>(_currentClip.rect.size.x),
            static_cast<int>(_currentClip.rect.size.y)
        };

        SDL_SetRenderClipRect(_renderer, &rect);
    } else {
        SDL_SetRenderClipRect(_renderer, nullptr);
    }
}

void Renderer::setBlendMode(RenderBlendMode mode) {
    _blendStack.push_back(_mode);
    _mode = mode;
    SDL_SetRenderDrawBlendMode(_renderer, static_cast<SDL_BlendMode>(mode));
}

void Renderer::resetBlendMode() {
    if (_blendStack.empty()) {
        return;
    }

    SDL_SetRenderDrawBlendMode(_renderer, static_cast<SDL_BlendMode>(_blendStack.back()));
    _blendStack.pop_back();
}

QTIP_CODE_END
