
//
// Created by Ayaan on 2026-11-02.
//

#include "Q-Tip/Events/Event.h"

#include <SDL3/SDL.h>

#include <algorithm>

QTIP_CODE_BEGIN

namespace {

    template<std::size_t N>
    void copyText(
        std::array<char, N>& destination,
        std::uint32_t& length,
        const char* source
    )
    {
        destination.fill('\0');
        length = 0;

        if (source == nullptr)
            return;

        const std::size_t sourceLength = std::strlen(source);
        const std::size_t copyLength = std::min(sourceLength, N - 1);

        std::memcpy(destination.data(), source, copyLength);

        length = static_cast<std::uint32_t>(copyLength);
    }

    std::uint32_t translateMouseButtons(SDL_MouseButtonFlags state)
    {
        std::uint32_t buttons = 0;

        if (state & SDL_BUTTON_LMASK)
            buttons |= 1u << 0;

        if (state & SDL_BUTTON_MMASK)
            buttons |= 1u << 1;

        if (state & SDL_BUTTON_RMASK)
            buttons |= 1u << 2;

        if (state & SDL_BUTTON_X1MASK)
            buttons |= 1u << 3;

        if (state & SDL_BUTTON_X2MASK)
            buttons |= 1u << 4;

        return buttons;
    }

    std::optional<std::uint8_t> translateMouseButton(std::uint8_t button)
    {
        switch (button) {
            case SDL_BUTTON_LEFT:
                return 0;

            case SDL_BUTTON_MIDDLE:
                return 1;

            case SDL_BUTTON_RIGHT:
                return 2;

            case SDL_BUTTON_X1:
                return 3;

            case SDL_BUTTON_X2:
                return 4;

            default:
                return std::nullopt;
        }
    }

    DisplayOrientation translateOrientation(SDL_DisplayOrientation orientation)
    {
        switch (orientation) {
            case SDL_ORIENTATION_LANDSCAPE:
                return DisplayOrientation::Landscape;

            case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
                return DisplayOrientation::LandscapeFlipped;

            case SDL_ORIENTATION_PORTRAIT:
                return DisplayOrientation::Portrait;

            case SDL_ORIENTATION_PORTRAIT_FLIPPED:
                return DisplayOrientation::PortraitFlipped;

            default:
                return DisplayOrientation::Unknown;
        }
    }

    Event makeEvent(
        EventType type,
        std::uint32_t windowID,
        std::uint64_t timestamp
    )
    {
        Event result{};

        result.type = type;
        result.windowID = windowID;
        result.timestamp = timestamp;

        return result;
    }

    WindowEvent translateWindowEvent(const SDL_WindowEvent& event)
    {
        WindowEvent result{};

        if (SDL_Window* window = SDL_GetWindowFromID(event.windowID)) {
            int x = 0;
            int y = 0;
            int width = 0;
            int height = 0;

            SDL_GetWindowPosition(window, &x, &y);
            SDL_GetWindowSize(window, &width, &height);

            result.position = {
                static_cast<float>(x),
                static_cast<float>(y)
            };

            result.width = width;
            result.height = height;
        }

        // Use the values directly supplied by the event.
        switch (event.type) {
            case SDL_EVENT_WINDOW_MOVED:
                result.position = {
                    static_cast<float>(event.data1),
                    static_cast<float>(event.data2)
                };
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                result.width = event.data1;
                result.height = event.data2;
                break;

            default:
                break;
        }

        return result;
    }

    Event translateDisplayEvent(const SDL_DisplayEvent& event)
    {
        Event result = makeEvent(
            EventType::DisplayConnected,
            0,
            event.timestamp
        );

        result.display = {};

        result.display.displayID = event.displayID;

        switch (event.type) {
            case SDL_EVENT_DISPLAY_ADDED:
                result.type = EventType::DisplayConnected;
                break;

            case SDL_EVENT_DISPLAY_REMOVED:
                result.type = EventType::DisplayDisconnected;
                break;

            case SDL_EVENT_DISPLAY_ORIENTATION:
                result.type = EventType::DisplayOrientationChanged;
                result.display.orientation =
                    translateOrientation(
                        static_cast<SDL_DisplayOrientation>(event.data1)
                    );
                break;

            default:
                break;
        }

        SDL_Rect bounds{};

        if (SDL_GetDisplayBounds(event.displayID, &bounds)) {
            result.display.width = bounds.w;
            result.display.height = bounds.h;
        }

        if (const SDL_DisplayMode* mode =
                SDL_GetCurrentDisplayMode(event.displayID)) {
            result.display.refreshRate = mode->refresh_rate;
        }

        return result;
    }

}

std::optional<Event> translateEvent(const SDL_Event& event)
{
    switch (event.type) {

        // Display

        case SDL_EVENT_DISPLAY_ADDED:
        case SDL_EVENT_DISPLAY_REMOVED:
        case SDL_EVENT_DISPLAY_ORIENTATION:
            return translateDisplayEvent(event.display);


        // Window

        case SDL_EVENT_WINDOW_MOVED:
        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_MINIMIZED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        {
            EventType type;

            switch (event.type) {
                case SDL_EVENT_WINDOW_MOVED:
                    type = EventType::WindowMoved;
                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                    type = EventType::WindowResized;
                    break;

                case SDL_EVENT_WINDOW_MINIMIZED:
                    type = EventType::WindowMinimized;
                    break;

                case SDL_EVENT_WINDOW_MAXIMIZED:
                    type = EventType::WindowMaximized;
                    break;

                case SDL_EVENT_WINDOW_RESTORED:
                    type = EventType::WindowRestored;
                    break;

                case SDL_EVENT_WINDOW_FOCUS_GAINED:
                    type = EventType::WindowFocused;
                    break;

                case SDL_EVENT_WINDOW_FOCUS_LOST:
                    type = EventType::WindowUnfocused;
                    break;

                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    type = EventType::WindowClosed;
                    break;

                default:
                    return std::nullopt;
            }

            Event result = makeEvent(
                type,
                event.window.windowID,
                event.window.timestamp
            );

            result.window = translateWindowEvent(event.window);

            return result;
        }


        // Keyboard

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        {
            Event result = makeEvent(
                event.type == SDL_EVENT_KEY_DOWN
                    ? EventType::KeyPressed
                    : EventType::KeyReleased,
                event.key.windowID,
                event.key.timestamp
            );

            result.key = {
                static_cast<std::int32_t>(event.key.key),
                static_cast<std::int32_t>(event.key.scancode),
                static_cast<std::uint16_t>(event.key.mod),
                event.key.repeat
            };

            return result;
        }

        case SDL_EVENT_TEXT_INPUT:
        {
            Event result = makeEvent(
                EventType::TextEntered,
                event.text.windowID,
                event.text.timestamp
            );

            result.textInput = {};

            copyText(
                result.textInput.text,
                result.textInput.length,
                event.text.text
            );

            return result;
        }


        // Mouse

        case SDL_EVENT_MOUSE_MOTION:
        {
            Event result = makeEvent(
                EventType::MouseMoved,
                event.motion.windowID,
                event.motion.timestamp
            );

            result.mouseMove = {
                {event.motion.x, event.motion.y},
                {event.motion.xrel, event.motion.yrel},
                translateMouseButtons(event.motion.state)
            };

            return result;
        }

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            const auto button =
                translateMouseButton(event.button.button);

            if (!button)
                return std::nullopt;

            Event result = makeEvent(
                event.type == SDL_EVENT_MOUSE_BUTTON_DOWN
                    ? EventType::MouseButtonPressed
                    : EventType::MouseButtonReleased,
                event.button.windowID,
                event.button.timestamp
            );

            result.mouseButton = {
                {event.button.x, event.button.y},
                *button,
                event.button.clicks
            };

            return result;
        }

        case SDL_EVENT_MOUSE_WHEEL:
        {
            float x = event.wheel.x;
            float y = event.wheel.y;

            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
                x = -x;
                y = -y;
            }

            Event result = makeEvent(
                EventType::MouseScrolled,
                event.wheel.windowID,
                event.wheel.timestamp
            );

            result.mouseScroll = {{x, y}};

            return result;
        }


        // Application

        case SDL_EVENT_QUIT:
        {
            return makeEvent(
                EventType::ApplicationQuit,
                0,
                event.quit.timestamp
            );
        }


        // Drag and drop

        case SDL_EVENT_DROP_FILE:
        {
            Event result = makeEvent(
                EventType::FileDropped,
                event.drop.windowID,
                event.drop.timestamp
            );

            result.fileDrop = {};

            copyText(
                result.fileDrop.path,
                result.fileDrop.length,
                event.drop.data
            );

            return result;
        }


        // Clipboard

        case SDL_EVENT_CLIPBOARD_UPDATE:
        {
            Event result = makeEvent(
                EventType::ClipboardChanged,
                0,
                event.clipboard.timestamp
            );

            bool containsText = false;

            for (int i = 0; i < event.clipboard.num_mime_types; ++i) {
                const char* mimeType = event.clipboard.mime_types[i];

                if (mimeType != nullptr &&
                    std::strncmp(mimeType, "text/", 5) == 0) {
                    containsText = true;
                    break;
                }
            }

            result.clipboard = {containsText};

            return result;
        }

        default:
            return std::nullopt;
    }
}

QTIP_CODE_END
