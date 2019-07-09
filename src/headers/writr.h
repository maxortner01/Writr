#pragma once

#include "Window.h"

namespace writr
{
    enum class WritrStatus
    {
        Info,
        Error
    };

    class Writr
    {
        struct Line
        {
            std::string text;
            /**/ bool   paragraph = false;
            /**/ bool   justified = true;
        };

        struct { std::string text; WritrStatus state; } status;
        
        const unsigned int width = 80;

        // Current line to edit
        Line* current_line;

        // Console window associated with this
        // renderer
        Window* window;

        // Position of current cursor as well as the line
        // buffer
        struct pos { int x; int y; } cursor_pos;
        std::vector<Line> lines;

        void loadFile(std::string filename);
        void saveFile(std::string filename);

        bool pollEvents();
        void addChar(char c);

        void moveDown();
        void moveUp();
        void moveLeft();
        void moveRight();
        
        void renderLines() const;

    public:
        Writr(std::string filename = "");

        void start();
    };
}