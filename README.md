# Q-Tip

>**WARNING:** I have used AI in this project. Sorry. I am good at coding, but I use AI for boring stuff. I am sincerely sorry.

>**WARNING:** I will push updates here and there and randomly update homebrew. I recommend to use -HEAD when installing Q-Tip.

Q-Tip is my little project, this was never thought to be used a lot by many people.
I just had an idea. And I made it. Hope you like it a little at least.

>**NOTE:** Versioning doesn't work the way you think. It's just a bunch of numbers spaced by dots.

>**NOTE:** This README is outdated, please wait until I want to make the docs when Q-Tip is big enough. Why I have not yet made documentation is due to me thinking that no one will notice Q-Tip at all, meaning no one will try to use Q-Tip. If you want to test or use Q-Tip, please read through the source code first.

>**NOTE:** Docs are at https://c-plusplus-dev.github.io/docs except I haven't written any documentation yet.

It is a small 2D graphics library written in modern C++ and based on SDL3.

Q-Tip is intended to simplify 2D rendering using SDL3 without completely concealing SDL; it offers lightweight C++ wrappers for windows, rendering, colours, rectangles, fonts, and textures, and at the same time allows access to the underlying SDL handles when required.

## Features

- Simple `Window` abstraction
- RAII-style resource management
- SDL3 renderer wrapper
- Points, lines, and rectangles
- Filled and outlined rectangles
- Text rendering through SDL3_ttf
- Texture loading through SDL3_image
- Centered text rendering
- Built-in common colors
- Lightweight `Rect` and `Color` types
- Move-only resource types to avoid accidental copies
- Native SDL handle access when you need lower-level control
- CMake package/install support
- C++20

## Example

```cpp
#include <Q-Tip/QTip.h>

using namespace QTip;

int main() {
    Window window("Q-Tip", 800, 600);
    Font font("assets/JetBrainsMono-Regular.ttf", 16);

    while (!window.shouldClose()) {
        QTRuntime.pollEvents();

        window->setRenderColor(Color::black);
        window->clear();

        window->setRenderColor(Color::blue);
        window->renderRect({50.0f, 50.0f, 200.0f, 100.0f});

        window->renderText(
            font,
            "Hello, Q-Tip!",
            70.0f,
            80.0f,
            Color::white
        );

        window->present();
    }
}
```

The `Window` class forwards renderer access through `operator->`, allowing you to write:

```cpp
window->clear();
window->renderPoint(100.0f, 100.0f);
window->renderLine(100.0f, 100.0f, 300.0f, 200.0f);
window->renderRect({50.0f, 50.0f, 200.0f, 100.0f});
window->renderText(font, "Hello!", 200.0f, 200.0f, Color::white);
window->present();
```

## Why Q-Tip?

Although SDL3 is powerful, it can require a good deal of setup and resource management if used directly.

Q-Tip offers a higher-level C++ interface for the various things you need when creating a small 2D application, at the same time keeping SDL close by so that you can still access it directly when required.

The aim is not to create a new game engine; Q-Tip should remain small, focused, and easy to use.

## Requirements

- C++20-compatible compiler
- CMake version 4.0 or newer
- SDL3
- SDL3_ttf
- SDL3_image

## Building

Clone the repository and configure it with CMake:

```bash
git clone https://github.com/ProPythonCoderAya/Q-Tip.git
cd Q-Tip

cmake -S . -B build
cmake --build build
```

This compiles the `QTip` library and the example program.

## Using Q-Tip With CMake

The CMake target 'QTip::QTip' is provided by Q-Tip.

Once Q-Tip is installed, you can link it to your own project with:

```cmake
find_package(QTip REQUIRED)

target_link_libraries(MyGame PRIVATE QTip::QTip)
```

## A Small Example

Creating a window is intentionally simple:

```cpp
Window window("My Game", 800, 600);
```

Then you can render through the window:

```cpp
window->setRenderColor(Color::white);
window->clear();

window->renderRect({100.0f, 100.0f, 200.0f, 100.0f});
window->renderLine(0.0f, 0.0f, 800.0f, 600.0f);

window->present();
```

For text, create a `Font` and render it:

```cpp
Font font("assets/JetBrainsMono-Regular.ttf", 16);

window->renderTextCentered(
    font,
    "Hello, world!",
    400.0f,
    300.0f,
    Color::white
);
```

## Project Structure

```text
Q-Tip/
├── include/Q-Tip/     # Public headers
├── src/               # Library implementation
├── examples/          # Example program
├── cmake/             # CMake package configuration
└── CMakeLists.txt
```

## Contributing

Q-Tip is still evolving.

We welcome bug reports, ideas, improvements, and pull requests.

## License

The terms governing the use and distribution of Q-Tip can be found in the repository's license file.

## Author

Created by **Ayaan** (`@ProPythonCoderAya`).
