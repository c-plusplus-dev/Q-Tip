
#include <Q-Tip/Graphics/Texture.h>
#include <Q-Tip/Graphics/Renderer.h>
#include <SDL3_image/SDL_image.h>

#include "include/Helpers.h"
#include <Q-Tip/Logger/Log.h>

#include "Q-Tip/Window/Window.h"

QTIP_CODE_BEGIN

Texture::Texture(Window& window, const std::filesystem::path& path) {
    _texture = loadTexture(window.getRenderer(), path);

    if (!_texture) {
        QTipLog(help_err::error.error, LOG_ERROR);
    }
}

Texture::~Texture() {
    destroy();
}

Texture::Texture(Texture&& other) noexcept
    : _texture(other._texture) {
    other._texture = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    destroy();

    _texture = other._texture;
    other._texture = nullptr;

    return *this;
}

void Texture::destroy() {
    if (!_texture) {
        return;
    }

    SDL_DestroyTexture(_texture);
    _texture = nullptr;
}

bool Texture::isValid() const {
    return _texture != nullptr;
}

SDL_Texture* Texture::nativeHandle() const {
    return _texture;
}

Point Texture::size() const {
    if (!_texture)
        return {0, 0};
    return {static_cast<float>(_texture->w), static_cast<float>(_texture->h)};
}

QTIP_CODE_END
