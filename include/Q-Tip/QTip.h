//
// Created by Ayaan on 2026-08-23.
//

#ifndef QTIP_QTIP_H
#define QTIP_QTIP_H

#include <Q-Tip/Graphics/Renderer.h>
#include <Q-Tip/Graphics/Color.h>
#include <Q-Tip/Graphics/Shapes/Shapes.h>
#include <Q-Tip/Graphics/Font.h>
#include <Q-Tip/Graphics/Texture.h>
#include <Q-Tip/Clock.h>

#include <Q-Tip/Window/Window.h>

#include "Events/Event.h"

QTIP_CODE_BEGIN

class QTipRuntime {
public:

    static void pollEvents();

    static std::vector<Window*> windows();

private:
    QTipRuntime();

    ~QTipRuntime();

    std::deque<Event> _events;

    std::vector<Window*> _windows;

    static void registerWindow(Window* window);
    static void unregisterWindow(Window* window);

    static void replaceWindow(Window* oldWindow, Window* newWindow);

    static QTipRuntime instance;

    friend class Window;
};

QTIP_CODE_END

#endif //QTIP_QTIP_H
