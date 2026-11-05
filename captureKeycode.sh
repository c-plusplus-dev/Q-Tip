#!/bin/bash

SDL_HEADER="/opt/homebrew/include/SDL3/SDL_keycode.h"
OUTPUT="include/Q-Tip/Window/Keycode.h"

{
    echo "#ifndef QTIP_KEYCODE_H"
    echo "#define QTIP_KEYCODE_H"
    echo
    echo "namespace QTip {"
    echo
    echo "enum class Keycode {"

    awk '
        /SDLK_/ {
            if ($0 ~ /^#define[[:space:]]+SDLK_[A-Za-z0-9_]+[[:space:]]+/) {
                name = $2
                sub(/^SDLK_/, "", name)

                if (name ~ /^EXTENDED_MASK/)
                    next

                if (name ~ /^SCANCODE_MASK/)
                    next

                if (name ~ /^[0-9]/)
                    name = "Num" name

                value = $3
                sub(/,$/, "", value)

                printf "    %s = %s,\n", name, value
            }
        }
    ' "$SDL_HEADER"

    echo "};"
    echo
    echo "}"
    echo
    echo "#endif // QTIP_KEYCODE_H"
} > "$OUTPUT"
