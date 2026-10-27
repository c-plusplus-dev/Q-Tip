#include <Q-Tip/QTip.h>

#include <cmath>
#include <string>
#include <cstdlib>
#include <iostream>

using namespace QTip;

namespace {
    float wrapSeconds(float seconds, float period) {
        return std::fmod(seconds, period);
    }

    float pingPong(float seconds, float period) {
        const float half = period * 0.5f;
        const float wrapped = wrapSeconds(seconds, period);
        return wrapped < half ? wrapped / half : 1.0f - (wrapped - half) / half;
    }

    Color lerpColor(Color a, Color b, float t) {
        return {
            static_cast<uint8_t>(static_cast<float>(a.r) + static_cast<float>(b.r - a.r) * t),
            static_cast<uint8_t>(static_cast<float>(a.g) + static_cast<float>(b.g - a.g) * t),
            static_cast<uint8_t>(static_cast<float>(a.b) + static_cast<float>(b.b - a.b) * t),
            static_cast<uint8_t>(static_cast<float>(a.a) + static_cast<float>(b.a - a.a) * t)
        };
    }

    Rect insetRect(float x, float y, float w, float h, float inset) {
        return {x + inset, y + inset, w - inset * 2.0f, h - inset * 2.0f};
    }

    void drawFeatureBadge(
        Window& window,
        const Font& font,
        const char* label,
        float x,
        float y,
        Color color
    ) {
        window->setRenderColor(color);
        window->renderRect({x, y, 190.0f, 32.0f});

        window->renderText(font, label, x + 10.0f, y + 8.0f, Color::black);
    }
}

int main() {
    std::cerr << "========== MAIN ==========\n";

    Window window("Q-Tip Feature Test - initializing", 800, 600);

    Font font("assets/JetBrainsMono-Regular.ttf", 16);
    Font largeFont("assets/JetBrainsMono-Regular.ttf", 28);

    Texture texture(window, "assets/weirdstockphoto5-1.jpg");

    const bool textureOk = texture.isValid();

    Clock clock;

    bool hidWindow = false;
    bool reshapedWindow = false;
    bool restoredWindow = false;

    while (!window.shouldClose()) {
        QTipRuntime::pollEvents();

        const uint64_t elapsed = clock.now();
        const float t = static_cast<float>(elapsed) / 1000.0f;

        if (!hidWindow && t >= 2.0f) {
            window.setTitle("Q-Tip Feature Test - hide()/show()");
            window.hide();
            hidWindow = true;
        }

        if (hidWindow && t >= 2.35f) {
            window.show();
        }

        if (!reshapedWindow && t >= 5.0f) {
            window.setTitle("Q-Tip Feature Test - setSize(960, 640)");
            window.setSize(960.0f, 640.0f);
            reshapedWindow = true;
        }

        if (!restoredWindow && t >= 9.0f) {
            window.setTitle("Q-Tip Feature Test - all features running");
            window.setSize(800.0f, 600.0f);
            restoredWindow = true;
        }

        const float width = window.width();
        const float height = window.height();

        const float pulse = pingPong(t, 2.0f);
        const float slowPulse = pingPong(t, 5.0f);
        const float orbitX = width * 0.5f + std::cos(t * 1.5f) * 210.0f;
        const float orbitY = height * 0.5f + std::sin(t * 1.5f) * 135.0f;

        const Color background = lerpColor(Color::black, {18, 18, 32, 255}, slowPulse);

        window->setRenderColor(background);
        window->clear();

        window->setRenderColor(Color::blue);
        window->renderRect(insetRect(20.0f, 20.0f, width - 40.0f, height - 40.0f, 0.0f), false);

        window->setRenderColor({32, 32, 48, 255});
        window->renderRect({36.0f, 70.0f, width - 72.0f, height - 150.0f});

        window->setRenderColor(Color::white);
        window->renderRect({42.0f, 76.0f, width - 84.0f, height - 162.0f}, false);

        window->setRenderColor(Color::red);
        window->renderRect({60.0f, 95.0f, 150.0f + pulse * 60.0f, 80.0f});

        window->setRenderColor(Color::green);
        window->renderRect({240.0f, 95.0f, 170.0f, 80.0f}, false);

        window->setRenderColor(Color::blue);
        window->renderRect({440.0f, 95.0f, 170.0f, 80.0f}, true);

        window->setRenderColor(Color::green);
        window->renderThickLine({60.0f, 205.0f, width - 60.0f, 205.0f}, 5 + pulse * 5, true);
        window->renderThickLine({60.0f, 225.0f, width - 60.0f, height - 145.0f}, 10, true);
        window->renderThickLine({width - 60.0f, 225.0f, 60.0f, height - 145.0f}, 10, true);

        window->setRenderColor(Color::red);
        for (int i = 0; i < static_cast<int>((window.width() - 140.0f) / 9.0f); ++i) {
            const float x = 70.0f + static_cast<float>(i) * 9.0f;
            const float y = 265.0f + std::sin(t * 4.0f + static_cast<float>(i) * 0.28f) * 38.0f;
            window->renderPoint({x, y});
        }

        window->setRenderColor(Color::green);
        for (int i = 0; i < 48; ++i) {
            const float angle = t * 2.0f + static_cast<float>(i) * 0.35f;
            const float radius = 20.0f + static_cast<float>(i % 8) * 5.0f;
            window->renderPoint(
                {
                    orbitX + std::cos(angle) * radius,
                    orbitY + std::sin(angle) * radius
                }
            );
        }

        if (textureOk) {
            const Rect animatedSrc = Rect::zero;
            const Rect animatedDst = {
                width - 245.0f - pulse * 30.0f,
                235.0f - pulse * 20.0f,
                180.0f + pulse * 60.0f,
                135.0f + pulse * 45.0f
            };

            window->renderTexture(texture, animatedSrc, animatedDst);

            window->setRenderColor(Color::white);
            window->renderRect(animatedDst, false);
        }

        drawFeatureBadge(window, font, "Color constants", 60.0f, height - 195.0f, Color::red);
        drawFeatureBadge(window, font, "Rect + outlines", 270.0f, height - 195.0f, Color::green);
        drawFeatureBadge(window, font, "Texture render", 480.0f, height - 195.0f, Color::blue);

        window->renderText(
            font,
            "Renderer: clear(), present(), setRenderColor(), renderPoint(), renderLine(), renderRect(), renderTexture()",
            60.0f,
            height - 145.0f,
            Color::white
        );

        window->renderText(
            font,
            "Text: renderText() and renderTextCentered()",
            60.0f,
            height - 120.0f,
            Color::green
        );

        const std::string status =
            std::string("Window: pollEvents(), shouldClose(), width()=") +
            std::to_string(static_cast<int>(width)) +
            ", height()=" +
            std::to_string(static_cast<int>(height)) +
            ", nativeWindow=ok" +
            ", nativeRenderer=ok" +
            ", texture=" +
            (textureOk ? "ok" : "failed");

        window->renderText(
            font,
            status.c_str(),
            60.0f,
            height - 95.0f,
            Color::blue
        );

        const std::string timer =
            std::string("Timed actions: 2s hide/show, 5s resize, 9s restore | elapsed ") +
            std::to_string(static_cast<int>(t)) +
            "s";

        window->renderText(
            font,
            timer.c_str(),
            60.0f,
            height - 70.0f,
            Color::white
        );

        window->renderTextCentered(
            largeFont,
            "Q-Tip Timed Feature Test",
            width / 2.0f,
            42.0f + std::sin(t * 2.5f) * 5.0f,
            Color::white
        );

        window->renderTextCentered(
            font,
            "Close the window to exit",
            width / 2.0f,
            height - 30.0f,
            Color::red
        );

        window->present();
    }

    return 0;
}