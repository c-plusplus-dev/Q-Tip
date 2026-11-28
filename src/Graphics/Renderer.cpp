//
// Created by Ayaan on 2026-08-23.
//

#include <Q-Tip/Graphics/Renderer.h>
#include <Q-Tip/Graphics/Texture.h>
#include <SDL3/SDL.h>
#include "../include/Helpers.h"
#include <Q-Tip/Logger/Log.h>
#include "earcut.hpp"
#include "Q-Tip/QTipRuntime.h"
#include "Q-Tip/Graphics/RenderTexture.h"
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

namespace {
    std::vector<int> triangulatePolygon(
        const std::vector<Vertex>& vertices
    ) {
        std::vector<std::vector<Point>> rings(1);

        rings[0].reserve(vertices.size());

        for (const Vertex& vertex : vertices) {
            rings[0].push_back(vertex.position);
        }

        const auto earcutIndices = mapbox::earcut<uint32_t>(rings);

        return {
            earcutIndices.begin(),
            earcutIndices.end()
        };
    }
}

Renderer::Renderer(SDL_Window* window) {
    if (!window) return;

    _renderer = SDL_CreateGPURenderer(nullptr, window);

    if (!_renderer) {
        const std::string gpuError = SDL_GetError();

        QTipLog(
            fmt(
                "SDL GPU renderer initialization failed: %s. "
                "Falling back to SDL renderer.",
                gpuError.c_str()
            ),
            LOG_WARNING
        );

        _renderer = SDL_CreateRenderer(window, nullptr);

        if (!_renderer) {
            QTipRuntime::reportError(
                "Oh no!",
                "The application has encountered an error that may leave the application in an unusable state. "
                "Please contact the creator of the application.",
                ErrorLevel::Fatal
            );
            return;
        }
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

void Renderer::renderGeometry(
    const std::vector<Vertex>& vertices,
    const Texture* texture
) {
    if (vertices.size() < 3)
        return;

    const auto indices = triangulatePolygon(vertices);

    renderGeometry(vertices, indices, texture);
}

void Renderer::renderGeometry(const std::vector<Vertex>& vertices, const std::vector<int>& indices, const Texture* texture) {
    std::vector<SDL_Vertex> sdlVertices;
    sdlVertices.reserve(vertices.size());

    for (const Vertex& vertex : vertices) {
        sdlVertices.push_back({
            {vertex.position.x, vertex.position.y},
            {
                static_cast<float>(vertex.color.r) / 255.0f,
                static_cast<float>(vertex.color.g) / 255.0f,
                static_cast<float>(vertex.color.b) / 255.0f,
                static_cast<float>(vertex.color.a) / 255.0f
            },
            {vertex.uv.x, vertex.uv.y}
        });
    }

    SDL_RenderGeometry(
        _renderer,
        texture ? texture->nativeHandle() : nullptr,
        sdlVertices.data(),
        static_cast<int>(sdlVertices.size()),
        indices.data(),
        static_cast<int>(indices.size())
    );
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

void Renderer::setTarget(RenderTexture& renderTarget) {
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
