#include <iostream>

#include <fstream>
#include <ncurses/ncurses.h>
#include "Window.h"

#ifndef CTRL
#define CTRL(c) ((c) & 037)
#endif

class WritrScreen
{
    struct Line
    {
        std::string text;
        bool justify = false;
        bool par = false;
    };

    struct Status
    {
        enum Type
        {
            Trace,
            Error,
            Info
        } type;

        std::string text;
    } status;

    int current_index;
    int width = 70;

    Line current_line;
    std::vector<Line> lines;

    writr::Window window;

    void appendStringToLines(const std::string &str);
    void formatStringToLines(const std::string &str);

    bool saveToFile(std::string dir);

    void addChar(char c);
    void moveDown(bool justified = false);
    void moveUp();

    void renderLines();
    void renderStatus();

    void pollEvents();

public:
    WritrScreen();  
    WritrScreen(std::string filelocation);  

    void run();
};

void WritrScreen::appendStringToLines(const std::string &str)
{
    for (int i = 0; i < str.length(); i++)
        addChar(str[i]);

    current_index = lines.size();
}

void WritrScreen::formatStringToLines(const std::string &str)
{
    lines.clear();
    lines.shrink_to_fit();

    appendStringToLines(str);
}

bool WritrScreen::saveToFile(std::string dir)
{
    std::ofstream f;
    f.open(dir);

    for (int i = 0; i < lines.size(); i++)
    {
        if (lines[i].par && i > 0)
        {
            f << "\n\n";
        }

        f << lines[i].text;
    }

    return true;
}

void WritrScreen::addChar(char c)
{
    int spaces = 0;
    if (current_line.par) spaces += 4;

    // If the length is equal to width, grab the last word, save it
    // for the current text, move down and set text
    if (current_line.text.length() + spaces >= width)
    {

        int s = -1;
        for (int i = current_line.text.length(); i >= 0; i--)
            if (current_line.text[i] == ' ')
            {
                s = i;
                break;
            }

        std::string word = "";
        if (s > -1)
        {
            word = std::string(current_line.text.begin() + s, current_line.text.end());
            current_line.text.erase(current_line.text.begin() + s, current_line.text.end());
        }

        moveDown(true);

        if (!word.empty())
            current_line.text = word;
    }

    if (c == '\n')
    {
        current_line.justify = false;
        moveDown(false);
        current_line.par = true;
    }
    else
    {
        current_line.text += c;
    }
    
}

void WritrScreen::moveDown(bool justified)
{
    if (current_index == lines.size())
        lines.push_back({ current_line.text, justified, current_line.par });
    
    current_index++;

    if (current_index < lines.size())
    {   
        status.text = "Move down 0";
        lines[current_index - 1].text = current_line.text;
        current_line = lines[current_index];
    }
    else if (current_index > lines.size())
    {
        status.text = "Move down 1";
        current_index--;
    }
    else
    {
        status.text = "Move down 2";
        current_line = {"", false, false};
    }
}

void WritrScreen::moveUp()
{
    if (current_index == lines.size())
        lines.push_back(current_line);

    current_index--;
    
    if (current_index >= 0 && current_index < lines.size() - 1)
    {
        lines[current_index + 1].text = current_line.text;
        current_line = lines[current_index];
    }
    else if (current_index < 0)
    {
        current_index++;
    }
    else
    {
        current_line = lines[current_index];
    }
}

void WritrScreen::renderLines()
{
    saveToFile("file");

    int mid = window.getSize().height / 2 - 1;
    int count = lines.size() - 1 - (lines.size() - current_index);
    int second_count = count + 2;
    for (int i = mid; i >= 0; i--)
    {
        if (count >= lines.size() || count < 0) break;
        if (i == window.getSize().height / 2) { count--; continue;}

        writr::TextAlign alignment = writr::TextAlign::Left;
        if (lines[count].justify) alignment = writr::TextAlign::Justified;

        int spaces = 0;
        if (lines[count].par) spaces += 4;

        window.draw.text(lines[count].text, window.getSize().width / 2 - width / 2, i, width, alignment, spaces);
        count--;
    }

    int it = 2;
    while (second_count < lines.size())
    {
        writr::TextAlign alignment = writr::TextAlign::Left;
        if (lines[second_count].justify) alignment = writr::TextAlign::Justified;

        int spaces = 0;
        if (lines[second_count].par) spaces += 4;

        window.draw.text(lines[second_count].text, window.getSize().width / 2 - width / 2, mid + it, width, alignment, spaces);
        second_count++;
        it++;
    }
}

void WritrScreen::renderStatus()
{
    attron(COLOR_PAIR(1));
    window.draw.text(status.text, 0, window.getSize().height - 1, window.getSize().width, writr::TextAlign::Center);
    attroff(COLOR_PAIR(1));
}

void WritrScreen::pollEvents()
{
    int c = getch();

    /**/ if (c == 127) 
    {
        if (current_line.text.length() > 0)
            current_line.text.erase(current_line.text.end() - 1, current_line.text.end());
        else
            moveUp();
        
    }
    else if (c == CTRL('s'))
    {
        if (saveToFile("file"))
            status.text = "Successfully saved to file [ file ]!";
        else
            status.text = "Saving to file [ file ] failed!";
    }
    else if (c == '\t')
    {
        if (current_line.par) current_line.par = false;
        else current_line.par = true;
    }
    else if (c == '\n' || c == KEY_DOWN)
    {
        moveDown();
        lines.at(lines.size() - 1).justify = false;
        current_line.par = true;
    } 
    else if (c == KEY_UP)
    {
        moveUp();
    }
    else
        addChar(c);

    if (c == 27)
    {
        endwin();
        exit(0);
    }  
}

WritrScreen::WritrScreen() :
    current_index(0), window()
{
    current_line.par = true;
}

WritrScreen::WritrScreen(std::string filelocation) :
    WritrScreen()
{
    std::fstream f(filelocation);

    if (!f.good())
    {
        status.text = "Failed to load file '" + filelocation + "'";
        status.type = status.Error;
    }
    else
    {
        status.text = "Successfully loaded file '" + filelocation + "'";
        status.type = status.Info;
    }

    for (std::string line; std::getline(f, line);)
    {
        if (line.empty()) continue;
        appendStringToLines(line + "\n");
    }

    f.close();
}

void WritrScreen::run()
{
    while (true)
    {
        window.wipe();
        renderLines();
        renderStatus();

        window.draw.text(std::to_string(current_index), 0, 0);
        window.draw.text("> " + current_line.text + " <", 0, window.getSize().height / 2, window.getSize().width, writr::TextAlign::Center);
        window.render();

        pollEvents();
    }
}

int main()
{
    WritrScreen screen("file");

    screen.run();
}