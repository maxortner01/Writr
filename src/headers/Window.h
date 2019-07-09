#pragma once

#include <inc.h>
#include "Draw.h"
#include "Data/Status.h"

namespace writr
{
    enum class WindowState
    {
        Ok,
        Console_Init_Failure,
        Console_Resize_Failure,
        Console_Buffer_Assignment_Failure,
        Console_Window_Resize_Failure
    };

    enum KeyCode
    {
        Escape    = 27,
        Backspace = 8,
        Enter     = 13,
        Up        = -1,
        Down      = -2,
        Left      = -3,
        Right     = -4,
        ControlS  = -5
    };

    class Window : public Status<WindowState>
    {

#ifdef WIN32
        CHAR_INFO* buffer;
        HANDLE     console;
        SMALL_RECT window_rect;
#else
        std::vector<char> buffer;
#endif

        struct windowsize { size_t height; size_t width; } size;

        void updateSize();

        // Private constructor
        Window(unsigned int width, unsigned int height);

        char &operator () (size_t x, size_t y);
    public:
        Window() = delete;
        ~Window();

        // Create a window and store it in the managers
        static Window &Create(unsigned int width = 80, unsigned int height = 30);

        void set(size_t x, size_t y, char c, short attr = 0x000F);

        void render();
        void wipe();

        int getChar() const;

        windowsize getSize() const { return size; }

    public:
        Draw draw;
    };
}