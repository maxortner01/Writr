#pragma once

#include <inc.h>

namespace writr
{
    class Window;

    enum class TextAlign
    {
        Left,
        Center,
        Right,
        Justified
    };

    class Draw
    {
        Window* w;

    public:
        Draw(Window* window);

        void text(std::string txt, unsigned int x, unsigned int y, short attr = 0x000F) const;
        void text(std::string txt, unsigned int x, unsigned int y, unsigned int width, TextAlign alignment, int spaces = 0, short attr = 0x000F) const;
    };
}