#include "Writr.h"

#include <iostream>

namespace writr
{
    void Writr::loadFile(std::string filename)
    {
        std::fstream f;
        f.open(filename);

        if (!f.good())
        {
            status.state = WritrStatus::Error;
            status.text  = "Failed to load file [" + filename + "]!";
            return;
        }

        for (std::string line; std::getline(f, line);)
        {
            if (line.empty())
            {
                current_line->justified = false;
                moveDown();
                current_line->paragraph = true;
                continue;
            }

            for (int i = 0; i < line.length(); i++)
                addChar(line[i]);
        }
        
        status = { ("Successfully loaded file [" + filename + "]!").c_str(), WritrStatus::Info };
    }

    void Writr::saveFile(std::string filename)
    {
        std::ofstream f;
        f.open(filename);

        for (int i = 0; i < lines.size(); i++)
        {
            if (lines[i].paragraph) { f << '\n'; f << '\n'; }

            f << lines[i].text;
        }

        status = { ("Successfully written to file [" + filename + "]!").c_str(), WritrStatus::Info };
    }

    bool Writr::pollEvents()
    {
        char c = window->getChar();

        switch (c)
        {
            case KeyCode::ControlS:
                saveFile("file.txt");
                break;

            case KeyCode::Backspace:
                /**/ if (current_line->text.length() > 0 && cursor_pos.x > 0)
                {
                    current_line->text.erase(current_line->text.length() - 1);
                    cursor_pos.x--;
                }
                else if (cursor_pos.x == 0)
                {
                    moveUp();
                }
                break;

            case KeyCode::Escape:
                return false;

            case KeyCode::Enter:
                current_line->justified = false;
                moveDown();
                current_line->paragraph = true;
                break;

            case KeyCode::Left:
                //moveLeft();
                break;

            case KeyCode::Right:
                //moveRight();
                break;

            case KeyCode::Up:
                moveUp();
                break;

            case KeyCode::Down:
                moveDown();
                break;

            default:
                addChar((char)c);
                break;
        };

        return true;
    }

    void Writr::addChar(char c)
    {
        bool at_end = false;
        if (cursor_pos.x == current_line->text.length()) at_end = true;

        int spaces = 0;
        if (current_line->paragraph) spaces += 4;

        if (current_line->text.length() >= width - spaces)
        {
            int s = -1;
            for (int i = current_line->text.length(); i >= 0; i--)
                if (current_line->text[i] == ' ')
                {
                    s = i;
                    break;
                }


            std::string word = "";
            if (s > -1)
            {
                word = std::string(current_line->text.begin() + s, current_line->text.end());
                current_line->text.erase(current_line->text.begin() + s, current_line->text.end());
            }

            int index = cursor_pos.x;

            moveDown();

            current_line->text = word;
            if (!at_end) 
            {
                moveUp();
                cursor_pos.x = index;
            }
            else
                cursor_pos.x = word.length();
        }

        cursor_pos.x++;

        if (at_end)
            current_line->text += c;
        else
            current_line->text.insert(current_line->text.begin() + cursor_pos.x - 1, c);
    }

    void Writr::moveDown()
    {
        if (cursor_pos.y == lines.size() - 1) 
            lines.push_back({ "", false, true });

        cursor_pos.y++;
        current_line = &lines.at(cursor_pos.y);
        cursor_pos.x = current_line->text.length();
    }

    void Writr::moveUp()
    {
        if (cursor_pos.y == 0) return;

        cursor_pos.y--;
        current_line = &lines.at(cursor_pos.y);
        cursor_pos.x = current_line->text.length();
    }
    
    void Writr::moveLeft()
    {
        if (cursor_pos.x > 0)
            cursor_pos.x--;
    }

    void Writr::moveRight()
    {
        if (cursor_pos.x <= current_line->text.length())
            cursor_pos.x++;
    }

    void Writr::renderLines() const
    {
        int mid_line = window->getSize().height / 2;

        int drawn = 0;
        int c = cursor_pos.y - 1;
        for (int i = mid_line - 1; i >= 0; i--)
        {
            if (c < 0 || c >= lines.size()) break;

            TextAlign align = TextAlign::Left;
            if (lines[c].justified) align = TextAlign::Justified;

            int spaces = 0;
            if (lines[c].paragraph) spaces += 4;
            
            short color = 0x0007;
            if (mid_line - i > window->getSize().height / 4)
                color = 0x0008;

            window->draw.text(lines[c--].text, window->getSize().width / 2 - width / 2, i, width, align, spaces, color);
            drawn++;
        }

        drawn = 0;
        c = cursor_pos.y + 1;
        for (int i = mid_line + 1; i <= window->getSize().height && c < lines.size(); i++)
        {
            TextAlign align = TextAlign::Left;
            if (lines[c].justified) align = TextAlign::Justified;

            int spaces = 0;
            if (lines[c].paragraph) spaces += 4;
            
            short color = 0x0007;
            if (mid_line - i > window->getSize().height / 4)
                color = 0x0008;

            window->draw.text(lines[c++].text, window->getSize().width / 2 - width / 2, i, width, align, spaces, color);
            drawn++;
        }

    }

    Writr::Writr(std::string filename)
    {
        window = &Window::Create(300, 100);

        if ( window->getStatusNum() > 0 )
        {
            std::cout << "Error: " << window->getStatusValue() << std::endl;
            exit(1);
        }

        lines.push_back(Line());
        current_line = &lines.at(0);
        current_line->paragraph = true;
        cursor_pos = { 0, 0 };

        if (!filename.empty())
        {
            loadFile(filename);
        }
    }

    void Writr::start()
    {
        std::string spaces;
        for (int i = 0; i < width; i++) spaces += ' ';

        std::string tab_spaces;
        for (int i = 0; i < 4; i++) tab_spaces += ' ';

        std::string cursor;
        cursor += char(219);

        while (true)
        {
            std::string to_display;
            if (current_line->paragraph) 
                to_display += tab_spaces;
                
            to_display += current_line->text;

            if (cursor_pos.x < current_line->text.length())
                to_display[cursor_pos.x] = cursor[0];
            else
                to_display += cursor;

            int s = 0;
            if (current_line->paragraph) s += 4;

            window->wipe();

            renderLines();
            // Render the arrows
            window->draw.text(">  " + spaces + "  <", 0, window->getSize().height / 2, window->getSize().width, TextAlign::Center);

            // Render the debug cursor position
            //window->draw.text(std::to_string(cursor_pos.x) + ", " + std::to_string(cursor_pos.y), 0, 0);
            //window->draw.text(to_display, 0, window->getSize().height / 2, window->getSize().width, TextAlign::Center);

            // Render text in the middle of the screen
            window->draw.text(to_display, window->getSize().width / 2 - width / 2, window->getSize().height / 2);

            short color = 0x0008;
            if (status.state == WritrStatus::Error) color = 0x0004;

            // Render the status at the bottom
            window->draw.text(status.text, 0, window->getSize().height - 1, window->getSize().width, TextAlign::Center, 0, 0x0007);

            window->render();

            if (!pollEvents()) break;
        }
    }
}