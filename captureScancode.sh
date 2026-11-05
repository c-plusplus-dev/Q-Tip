#!/bin/bash

SDL_HEADER="/opt/homebrew/include/SDL3/SDL_scancode.h"
OUTPUT="include/Q-Tip/Window/Scancode.h"

{
    echo "#ifndef QTIP_SCANCODE_H"
    echo "#define QTIP_SCANCODE_H"
    echo
    echo "namespace QTip {"
    echo
    echo "enum class Scancode {"

    awk '
        /SDL_SCANCODE_/ {
            if ($0 ~ /^[[:space:]]*SDL_SCANCODE_[A-Za-z0-9_]+[[:space:]]*=/) {
                name = $1
                sub(/^SDL_SCANCODE_/, "", name)

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
    echo "#endif // QTIP_SCANCODE_H"
} > "$OUTPUT"
