#include "Draw.h"

#include "Window.h"

namespace writr
{
    Draw::Draw(Window* window)
        : w(window)
    {

    }

    void Draw::text(std::string txt, unsigned int x, unsigned int y) const
    {
        for (int i = 0; i < txt.size(); i++)
            if (y * w->getSize().width + x < w->getSize().width * w->getSize().height)
                (*w)(x + i, y) = txt[i];
    }

    void Draw::text(std::string txt, unsigned int x, unsigned int y, unsigned int width, TextAlign alignment) const
    {
        switch (alignment)
        {
            case TextAlign::Left:
                text(txt, x, y);
                return;
            
            case TextAlign::Right:
                text(txt, x + width - txt.length(), y);
                return;

            case TextAlign::Center:
                text(txt, x + width / 2 - txt.length() / 2, y);
                return;
        };
    }
}