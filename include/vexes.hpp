#pragma once

#include <ncurses.h>
#include <string>
#include <map>

namespace vex {

    // CONSTANTS
    static const std::map<std::string, int> attributes = {
        {"standout", A_STANDOUT},
        {"underline", A_UNDERLINE},
        {"reverse", A_REVERSE},
        {"blink", A_BLINK},
        {"dim", A_DIM},
        {"bold", A_BOLD},
        {"protected", A_PROTECT},
        {"invisible", A_INVIS},
        {"alternate", A_ALTCHARSET},
        {"extract", A_CHARTEXT},
        {"black", COLOR_PAIR(0)},
        {"red", COLOR_PAIR(1)},
        {"green", COLOR_PAIR(2)},
        {"yellow", COLOR_PAIR(3)},
        {"blue", COLOR_PAIR(4)},
        {"magenta", COLOR_PAIR(5)},
        {"cyan", COLOR_PAIR(6)},
        {"white", COLOR_PAIR(7)}
    };

}

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
        int getAttribute(std::string name) {
            auto iter = attributes.find(name);
            // Check if name exists, otherwise return A_NORMAL
            if(iter == attributes.end()) {
                return A_NORMAL;
            } else {
                return iter->second;
            }
        }
        void setAttributes(int attr, WINDOW * win = NULL) {
            if(win != NULL) {
                wattron(win, attr);
            } else {
                attron(attr);
            }
        }
        void unsetAttributes(int attr, WINDOW * win = NULL) {
            if(win != NULL) {
                wattroff(win, attr);
            } else {
                attroff(attr);
            }
        }
        void drawCharAtPoint(char ch, Vec2i p, WINDOW * win = NULL) {
            if(win != NULL) {
                wmove(win, p.y, p.x);
                waddch(win, ch);
            } else {
                move(p.y, p.x);
                addch(ch);
            }
        }
        void drawStringAtPoint(std::string text, Vec2i p, WINDOW * win = NULL) {
            if(win != NULL) {
                wmove(win, p.y, p.x);
                waddstr(win, text.c_str());
            } else {
                move(p.y, p.x);
                addstr(text.c_str());
            }
        }
        void drawCenteredStringAtPoint(std::string text, Vec2i p, WINDOW * win = NULL) {
            // Compute new point offset by half of string's length
            size_t offset = text.size() / 2;
            Vec2i newPoint(p.x - offset, p.y);
            
            // Delegate to drawStringAtPoint with new point
            drawStringAtPoint(text, newPoint, win);
        }
        void drawCustomHLineBetweenPoints(char ch, Vec2i a, Vec2i b, WINDOW * win = NULL) {
            // Only draw line if points are on the same Y level
            if(a.y != b.y) { return; }

            // To avoid issues of order, we check which x is larger
            if(a.x < b.x) {
                for(int i = a.x; i < b.x + 1; i++) {
                    drawCharAtPoint(ch, {i, a.y}, win);
                }
            } else if(a.x > b.x) {
                for(int i = b.x; i < a.x + 1; i++) {
                    drawCharAtPoint(ch, {i, b.y}, win);
                }
            } else {
                // x is equal, so we only draw a single character
                drawCharAtPoint(ch, a, win);
            }
        }
        void drawHLineBetweenPoints(Vec2i a, Vec2i b, WINDOW * win = NULL) {
            // Delegate to the custom line with the HLINE character
            setAttributes(getAttribute("alternate"), win);
            drawCustomHLineBetweenPoints(ACS_HLINE, a, b, win);
            unsetAttributes(getAttribute("alternate"), win);
        }
        void drawCustomVLineBetweenPoints(char ch, Vec2i a, Vec2i b, WINDOW * win = NULL) {
            // Only draw line if points are on the same X level
            if(a.x != b.x) { return; }

            // To avoid issues of order, we check which y is larger
            if(a.y < b.y) {
                for(int i = a.y; i < b.y + 1; i++) {
                    drawCharAtPoint(ch, {a.x, i}, win);
                }
            } else if(a.y > b.y) {
                for(int i = b.y; i < a.y + 1; i++) {
                    drawCharAtPoint(ch, {b.x, i}, win);
                }
            } else {
                // y is equal, so we only draw a single character
                drawCharAtPoint(ch, a, win);
            }
        }
        void drawVLineBetweenPoints(Vec2i a, Vec2i b, WINDOW * win = NULL) {
            // Delegate to the custom line with the VLINE character
            setAttributes(getAttribute("alternate"), win);
            drawCustomVLineBetweenPoints(ACS_VLINE, a, b, win);
            unsetAttributes(getAttribute("alternate"), win);
        }

    };

}
