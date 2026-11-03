#include <Q-Tip/Window/Input.h>
#include <utility>

#include "Q-Tip/Events/Event.h"

QTIP_CODE_BEGIN

namespace {
    constexpr std::size_t mouseButtonIndex(MouseButton button)
    {
        return static_cast<std::size_t>(button);
    }

    constexpr std::size_t keyIndex(Key key)
    {
        return static_cast<std::size_t>(key);
    }
}

Input::Input()
{
    _keys.fill(false);
    _pressed.fill(false);
    _released.fill(false);

    _mouseButtons.fill(false);
    _mousePressed.fill(false);
    _mouseReleased.fill(false);
}

bool Input::keyDown(Key key) const
{
    const auto index = keyIndex(key);

    if (index >= KeyCount)
        return false;

    return _keys[index];
}

bool Input::keyWasPressed(Key key) const
{
    const auto index = keyIndex(key);

    if (index >= KeyCount)
        return false;

    return _pressed[index];
}

bool Input::keyWasReleased(Key key) const
{
    const auto index = keyIndex(key);

    if (index >= KeyCount)
        return false;

    return _released[index];
}

bool Input::mouseButtonDown(MouseButton button) const
{
    const auto index = mouseButtonIndex(button);

    if (index >= _mouseButtons.size())
        return false;

    return _mouseButtons[index];
}

bool Input::mouseButtonWasPressed(MouseButton button) const
{
    const auto index = mouseButtonIndex(button);

    if (index >= _mousePressed.size())
        return false;

    return _mousePressed[index];
}

bool Input::mouseButtonWasReleased(MouseButton button) const
{
    const auto index = mouseButtonIndex(button);

    if (index >= _mouseReleased.size())
        return false;

    return _mouseReleased[index];
}

float Input::mouseX() const
{
    return _mouseX;
}

float Input::mouseY() const
{
    return _mouseY;
}

float Input::mouseDeltaX() const
{
    return _mouseDeltaX;
}

float Input::mouseDeltaY() const
{
    return _mouseDeltaY;
}

std::string Input::textInput() {
    return std::exchange(_text, {});
}

void Input::beginFrame()
{
    _pressed.fill(false);
    _released.fill(false);

    _mousePressed.fill(false);
    _mouseReleased.fill(false);

    _mouseDeltaX = 0.0f;
    _mouseDeltaY = 0.0f;
}

void Input::processEvent(const Event& event)
{
    switch (event.type) {
        case EventType::KeyPressed:
        {
            const auto key = static_cast<std::size_t>(event.key.scancode);

            if (key >= KeyCount)
                break;

            // Ignore key-repeat events. A key is only "pressed"
            // on the initial KEY_DOWN event.
            if (!event.key.repeat) {
                _pressed[key] = true;
            }

            _keys[key] = true;
            break;
        }

        case EventType::KeyReleased:
        {
            const auto key = static_cast<std::size_t>(event.key.scancode);

            if (key >= KeyCount)
                break;

            _keys[key] = false;
            _released[key] = true;
            break;
        }

        case EventType::MouseMoved:
        {
            _mouseX = event.mouseMove.position.x;
            _mouseY = event.mouseMove.position.y;

            _mouseDeltaX += event.mouseMove.delta.x;
            _mouseDeltaY += event.mouseMove.delta.y;
            break;
        }

        case EventType::MouseButtonPressed:
        {
            std::size_t button;

            switch (event.mouseButton.button) {
                case Left:
                    button = mouseButtonIndex(Left);
                    break;

                case Middle:
                    button = mouseButtonIndex(Middle);
                    break;

                case Right:
                    button = mouseButtonIndex(Right);
                    break;

                case X1:
                    button = mouseButtonIndex(X1);
                    break;

                case X2:
                    button = mouseButtonIndex(X2);
                    break;

                default:
                    return;
            }

            _mouseButtons[button] = true;
            _mousePressed[button] = true;
            break;
        }

        case EventType::MouseButtonReleased:
        {
            std::size_t button;

            switch (event.mouseButton.button) {
                case Left:
                    button = mouseButtonIndex(MouseButton::Left);
                    break;

                case Middle:
                    button = mouseButtonIndex(MouseButton::Middle);
                    break;

                case Right:
                    button = mouseButtonIndex(MouseButton::Right);
                    break;

                case X1:
                    button = mouseButtonIndex(MouseButton::X1);
                    break;

                case X2:
                    button = mouseButtonIndex(MouseButton::X2);
                    break;

                default:
                    return;
            }

            _mouseButtons[button] = false;
            _mouseReleased[button] = true;
            break;
        }

        case EventType::TextEntered: {
            _text.append(
               event.textInput.text.data(),
               event.textInput.length
           );
            break;
        }

        default:
            break;
    }
}

QTIP_CODE_END
