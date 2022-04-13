#pragma once

#include <ncurses.h>

namespace vex {

    // STRUCTS
    template <typename T>
    struct Vec2 {
        T x, y;

        Vec2(T x_, T y_): x(x_), y(y_) {}

        bool operator == (const Vec2& other) const {
            return (x == other.x && y == other.y);
        }

        Vec2 operator + (const Vec2& other) const {
            return Vec2<T>(x + other.x, y + other.y);
        }

        Vec2 operator - (const Vec2& other) const {
            return Vec2<T>(x - other.x, y - other.y);
        }
    };

    typedef Vec2<int> Vec2i;

}

namespace vex {

    // BASE CLASSES
    class Engine {

    protected:
        void initializeScreenVariables() {
            initscr();		        // Begin curses mode
            cbreak();		        // Disable line buffering
            keypad(stdscr, TRUE);	// Enable extra keys
            noecho();		        // Disable echoing keys to console
            start_color();		    // Enable color mode
            curs_set(0);		    // Set cursor to be invisible
            timeout(50);		    // Make getch a non-blocking call
        }
        void initializeColorPairs() {
            int backgroundColor = -1; // Transparency
            use_default_colors();

            init_pair(0, COLOR_BLACK,   backgroundColor);
            init_pair(1, COLOR_RED,     backgroundColor);
            init_pair(2, COLOR_GREEN,   backgroundColor);
            init_pair(3, COLOR_YELLOW,  backgroundColor);
            init_pair(4, COLOR_BLUE,    backgroundColor);
            init_pair(5, COLOR_MAGENTA, backgroundColor);
            init_pair(6, COLOR_CYAN,    backgroundColor);
            init_pair(7, COLOR_WHITE,   backgroundColor);
        }
        void setupCursesEnvironment() {
            initializeScreenVariables();
            initializeColorPairs();
        }
        void teardownCursesEnvironment() {
            endwin();
        }

    public:
        Engine() { setupCursesEnvironment(); }
        virtual ~Engine() { teardownCursesEnvironment(); }

        virtual void init() = 0;
        virtual void run() = 0;

    public: // UTILITY FUNCTIONS
        Vec2i getMidpoint() {
            return {COLS / 2, LINES / 2};
        }

    public: // DRAWING FUNCTIONS
        void drawCharAtPoint(char ch, Vec2i p, WINDOW * win = NULL) {
            if(win != NULL) {
                wmove(win, p.y, p.x);
                waddch(win, ch);
            } else {
                move(p.y, p.x);
                addch(ch);
            }
        }

    };

}
