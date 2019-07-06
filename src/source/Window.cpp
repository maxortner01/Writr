#include "Window.h"

#include <ncurses/ncurses.h>

#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

namespace writr
{
    void Window::updateSize()
    {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        size = { w.ws_row, w.ws_col };

        buffer.resize(size.width * size.height);
    }

    Window::Window() :
        draw(this)
    {
        // init ncurses
        initscr();
        raw();
        noecho();    
        keypad(stdscr, TRUE);

        // update the size and wipe the screen
        updateSize();
        wipe();
    }

    Window::~Window()
    {
        endwin();
    }

    char &Window::operator () (size_t x, size_t y)
    {
        return buffer[y * size.width + x];
    }

    void Window::render()
    {
        updateSize();

        for (int y = 0; y < getSize().height; y++)
        {
            for (int x = 0; x < getSize().width; x++)
                printw(&(*this)(x, y));

            printw("\n");
        }

        refresh();
    }

    void Window::wipe()
    {
        for (int i = 0; i < buffer.size(); i++)
            buffer[i] = ' ';

        clear();
    }

    char Window::getChar() const
    {
        return getch();
    }
}