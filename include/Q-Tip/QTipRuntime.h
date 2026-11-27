//
// Created by Ayaan on 2026-11-04.
//

#ifndef QTIP_QTIPRUNTIME_H
#define QTIP_QTIPRUNTIME_H

#include <deque>
#include <optional>
#include <vector>

#include "Events/Event.h"

QTIP_CODE_BEGIN

class Window;

enum class ErrorMode {
    DialogAndExit,
    Exit,
    Dialog,
    Continue,
    Ignore
};

enum class ErrorLevel {
    Warning,
    Error,
    Fatal
};

class QTipRuntime {
public:

    static void fetchEvents();

    static std::vector<Window*> windows();

    static std::optional<Event> pollEvent();
    static const Event* peekEvent();

    static void setErrorMode(ErrorMode mode);
    static ErrorMode getErrorMode();

    static void reportError(const char* title, const char* message, ErrorLevel level);

    static void gamepadReceivesEventsInBackground(bool receives);

private:
    QTipRuntime();

    ~QTipRuntime();

    std::deque<Event> _events;

    std::vector<Window*> _windows;

    ErrorMode _mode = ErrorMode::DialogAndExit;

    static void registerWindow(Window* window);
    static void unregisterWindow(Window* window);

    static void replaceWindow(Window* oldWindow, Window* newWindow);

    static std::optional<Event> pollEvent(const Window* window);
    static const Event* peekEvent(const Window* window);

    static QTipRuntime instance;

    friend class Window;
};

QTIP_CODE_END

#endif //QTIP_QTIPRUNTIME_H
