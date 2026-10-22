//
// Created by Ayaan on 2026-09-07.
//

#ifndef QTIP_RENDERTEXTURE_H
#define QTIP_RENDERTEXTURE_H
#include <Q-Tip/Config.h>

#include "Texture.h"
#include "Q-Tip/Math/Point.h"
#include "Q-Tip/Mods/Moddable.h"

QTIP_CODE_BEGIN
class Window;

class RenderTarget : public Texture {
MODDABLE_DERIVED(RenderTarget, Texture)

public:
    RenderTarget(Window& renderer, int width, int height);

    RenderTarget(Window& window, Point size);
};

QTIP_CODE_END

#endif //QTIP_RENDERTEXTURE_H
