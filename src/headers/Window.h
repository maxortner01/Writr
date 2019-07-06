#pragma once

#include <vector>
#include "Draw.h"

namespace writr
{
    class Window
    {
        std::vector<char> buffer;
        struct windowsize { size_t height; size_t width; } size;

        void updateSize();

    public: 
        Draw draw;

        Window();
        ~Window();

        char &operator () (size_t x, size_t y);

        void render();
        void wipe();

        char getChar() const;

        windowsize getSize() const { return size; }
    };
}