#include "Window.h"
#include "mem/Manager.h"

#ifndef WIN32
#include <ncurses/ncurses.h>

#include <sys/ioctl.h>
#include <unistd.h>
#endif

#define CTRL_KEYPRESS(k) ((k)  & 0x1f)
#include <stdlib.h>
#include <iostream>

namespace writr
{
    // Window Update Size Method

    void Window::updateSize()
    {
#ifndef WIN32
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        size = { w.ws_row, w.ws_col };
#else
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

        size.width  = info.srWindow.Right  - info.srWindow.Left + 1;
        size.height = info.srWindow.Bottom - info.srWindow.Top  + 1;
#endif
        printf(std::to_string(size.width).c_str());

        if (buffer) delete buffer;
        buffer = new CHAR_INFO[size.width * size.height];
        memset(buffer, 0, sizeof(CHAR_INFO) * size.width * size.height);
    }

    // Window constructor

    Window::Window(unsigned int width, unsigned int height) :
        draw(this), Status(WindowState::Ok)
    {
        getValues() = {
            "Ok",
            "Console Init Failure",
            "Console Resize Failure",
            "Console Buffer Assignment Failure",
            "Console Window Resize Failure"
        };
        
#ifndef WIN32
        setlocale(LC_ALL, "");
        
        // init ncurses
        initscr();
        raw();
        noecho();
        keypad(stdscr, TRUE);
#else
        console = GetStdHandle(STD_OUTPUT_HANDLE);

        if (console == INVALID_HANDLE_VALUE)
        {
            setStatus(WindowState::Console_Init_Failure);
            return;
        }

        // Make the screen buffer size minimum so the buffer can shrink
        // below the actual size
        SMALL_RECT rect = { 0, 0, 1, 1 };
        SetConsoleWindowInfo(console, TRUE, &rect);

        // Set the size of the screen buffer
        COORD dim = { (short)width, (short)height };
        if (!SetConsoleScreenBufferSize(console, dim))
        {
            setStatus(WindowState::Console_Resize_Failure);
            return;
        }

        // Set the buffer to the console
        if (!SetConsoleActiveScreenBuffer(console))
        {
            setStatus(WindowState::Console_Buffer_Assignment_Failure);
            return;
        }

        // Change the size of the window
        window_rect = { 0, 0, (short)width - 1, (short)height - 1 };
        if (!SetConsoleWindowInfo(console, TRUE, &window_rect))
        {
            setStatus(WindowState::Console_Window_Resize_Failure);
            return;
        }
#endif
        // update the size and wipe the screen
        updateSize();
        wipe();
    }
    
    char &Window::operator () (size_t x, size_t y)
    {
#ifndef WIN32
        return buffer[y * size.width + x];
#else
        return buffer[y * size.width + x].Char.AsciiChar;
#endif
    }

    // Window Destructor

    Window::~Window()
    {
#ifndef WIN32
        endwin();
#endif
    }

    void Window::set(size_t x, size_t y, char c, short attr)
    {
        if (x >= 0 && x < size.width)
            if (y >= 0 && y < size.height)
            {
                (*this)(x, y) = c;
#ifdef WIN32
                buffer[y * size.width + x].Attributes = attr;
#endif
            }
    }

    // Window Create Method

    Window &Window::Create(unsigned int width, unsigned int height)
    {
        Window* window = new Window(width, height);
        Manager<Window>::get().push(window);

        return *window;
    }

    void Window::render()
    {
#ifndef WIN32
        updateSize();

        for (int y = 0; y < getSize().height; y++)
        {
            for (int x = 0; x < getSize().width; x++)
                printw(&(*this)(x, y));

            printw("\n");
        }

        refresh();
#else
        WriteConsoleOutput(console, buffer, { (short)size.width, (short)size.height }, { 0, 0 }, &window_rect);
#endif
    }

    // Window Wipe Method

    void Window::wipe()
    {
        for (int i = 0; i < size.width * size.height; i++)
        {
            buffer[i].Char.AsciiChar = ' ';
            buffer[i].Attributes = 0x000F;
        }

#ifndef WIN32
        clear();
#else

#endif
    }

    // Window get char method

    int Window::getChar() const
    {
#ifndef WIN32
        return getch();
#else
        COORD pos = { size.width - 1, size.height - 1 };
        SetConsoleCursorPosition(console, pos);

        char c = _getch();

        if (c == 0)
        {
            c = _getch();
            switch(c)
            {
                case 72:
                    return KeyCode::Up;
                case 80:
                    return KeyCode::Down;
                case 75:
                    return KeyCode::Left;
                case 77:
                    return KeyCode::Right;
            };
        }

        switch (c)
        {
            case CTRL_KEYPRESS('s'):
                return KeyCode::ControlS;

            default:
                return c;
        }
#endif
    }
}