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

    void Draw::text(std::string txt, unsigned int x, unsigned int y, unsigned int width, TextAlign alignment, int space_count) const
    {
        switch (alignment)
        {
            case TextAlign::Left:
                // Erase the spaces at the beginning
                for (int i = 0; i < txt.length(); i++)
                    if (txt[i] != ' ')
                    {
                        txt.erase(txt.begin(), txt.begin() + i);
                        break;
                    }
                    
                for (int i = 0; i < space_count; i++)
                    txt.insert(txt.begin(), ' ');
                    
                text(txt, x, y);
                return;
            
            case TextAlign::Right:
                text(txt, x + width - txt.length(), y);
                return;

            case TextAlign::Center:
                text(txt, x + width / 2 - txt.length() / 2, y);
                return;

            case TextAlign::Justified:
            {
                unsigned int r_width = width - space_count;

                // Erase the spaces at the beginning
                for (int i = 0; i < txt.length(); i++)
                    if (txt[i] != ' ')
                    {
                        txt.erase(txt.begin(), txt.begin() + i);
                        break;
                    }

                // Erase the spaces at the end
                for (int i = txt.length() - 1; i >= 0; i--)
                    if (txt[i] != ' ')
                    {
                        if (i != txt.length() - 1)
                            txt.erase(txt.begin() + i, txt.end());
                        break;
                    }

                // index the spaces
                std::vector<int> spaces;
                for (int i = 0; i < txt.length(); i++)
                    if (txt[i] == ' ') spaces.push_back(i);

                spaces.shrink_to_fit();

                unsigned int left_count, right_count, mid;
                left_count  = 0;
                right_count = spaces.size() - 1;
                mid = spaces.size() / 2;

                while(txt.length() < r_width)
                {
                    for (int i = left_count + 1; i < spaces.size(); i++)
                        spaces[i]++;

                    txt.insert(txt.begin() + spaces[left_count++], ' ');
                    if (left_count > mid) left_count = 0;

                    if (txt.length() == r_width)
                        break;

                    for (int i = right_count + 1; i < spaces.size(); i++)
                        spaces[i]++;

                    txt.insert(txt.begin() + spaces[right_count--], ' ');
                    if (right_count < mid) right_count = spaces.size() - 1;

                    if (txt.length() == r_width)
                        break;
                }

                for (int i = 0; i < space_count; i++)
                    txt.insert(txt.begin(), ' ');

                text(txt, x, y);

                return;
            }
        };
    }
}