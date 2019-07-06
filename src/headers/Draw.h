#pragma once

#include <string>

namespace writr
{
    class Window;

    enum class TextAlign
    {
        Left,
        Center,
        Right
    };

    class Draw
    {
        Window* w;

    public:
        Draw(Window* window);

        void text(std::string txt, unsigned int x, unsigned int y) const;
        void text(std::string txt, unsigned int x, unsigned int y, unsigned int width, TextAlign alignment) const;
    };
}