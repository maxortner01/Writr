#include <iostream>

#include <ncurses/ncurses.h>
#include "Window.h"

int main()
{
    writr::Window window;

    int current_index = 0;
    std::vector<std::string> lines;
    std::string center = "";
    while (true)
    {
        window.wipe();

        int mid = window.getSize().height / 2 - 1;
        int count = lines.size() - 1 - (lines.size() - current_index);
        int second_count = count + 2;
        for (int i = mid; i >= 0; i--)
        {
            if (count >= lines.size() || count < 0) break;
            if (i == window.getSize().height / 2) { count--; continue;}

            window.draw.text(lines[count], 0, i, window.getSize().width, writr::TextAlign::Left);
            count--;
        }

        int it = 2;
        while (second_count < lines.size())
        {
            window.draw.text(lines[second_count], 0, mid + it, window.getSize().width, writr::TextAlign::Left);
            second_count++;
            it++;
        }

        window.draw.text(std::to_string(current_index), 0, 0);
        window.draw.text("> " + center + " <", 0, window.getSize().height / 2, window.getSize().width, writr::TextAlign::Center);
        window.render();

        int c = getch();

        /**/ if (c == 127) 
        {
            if (center.length() > 0)
                center.erase(center.end() - 1, center.end());
            
        }
        else if (c == '\t')
        {
            center += "    ";
        }
        else if (c == '\n')
        {
            if (current_index == lines.size())
                lines.push_back(center);
            else
                lines[current_index] = center;

            center = "";
            current_index++;

            if (current_index < lines.size())
                center = lines[current_index];
        } 
        else if (c == KEY_UP)
        {
            current_index--;

            if (current_index > 0 && current_index < lines.size() - 1)
            {   
                lines[current_index + 1] = center;
                center = lines[current_index];
            }
            else if (current_index < 0)
            {
                current_index++;
            }
            else
            {
                center = lines[current_index];
            }
        }
        else if (c == KEY_DOWN)
        {
            current_index++;

            if (current_index < lines.size())
            {   
                lines[current_index - 1] = center;
                center = lines[current_index];
            }
            else if (current_index > lines.size())
            {
                center = "";
                current_index--;
            }
            else
            {
                lines[current_index - 1] = center;
                center = "";
            }
            
        }
        else
            center += c;

        if (c == 'x') break;        
    }

    std::cout << "Howdy " << window.getSize().width << std::endl;
}