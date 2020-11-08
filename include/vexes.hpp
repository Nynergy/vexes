#pragma once

#include <ncurses.h>
#include <string>
#include <map>

////////////////////////////////// MACROS ////////////////////////////////////

// These macros just define a simple way of finding the middle of the screen
#define MIDHEIGHT   (LINES/2)
#define MIDWIDTH    (COLS/2)

//////////////////////////////// CONSTANTS ///////////////////////////////////

// This map is used to make using attributes easier and more readable
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

///////////////////////////////// STRUCTS ////////////////////////////////////

/*
 * The Point is my way of creating easy to use coordinates, and it uses the
 * more typical Cartesian style of ordering (x first, y second), as opposed to
 * ncurses' y-before-x convention. While this convention is understandable
 * from the perspective of computer graphics, I simply prefer the x-before-y
 * style more.
 */
struct Point {

    int x, y;

    Point(int xIn, int yIn) : x(xIn), y(yIn) {}

    static bool pointsHaveEqualX(Point a, Point b) {
        return a.x == b.x;
    }

    static bool pointsHaveUnequalX(Point a, Point b) {
        return !pointsHaveEqualX(a, b);
    }

    static bool pointsHaveEqualY(Point a, Point b) {
        return a.y == b.y;
    }

    static bool pointsHaveUnequalY(Point a, Point b) {
        return !pointsHaveEqualY(a, b);
    }

};

/*
 * The Box is my way of creating boxes/borders by specifying a set of four
 * corners. This struct does not contain any information on how the border
 * looks, only where it appears.
 */
struct Box {

    Point ul, ur, ll, lr; // Upper left, upper right, lower left, lower right

    Box(Point ulIn, Point urIn, Point llIn, Point lrIn) :
        ul(ulIn), ur(urIn), ll(llIn), lr(lrIn) {}

};

/////////////////////////////// BASE CLASSES /////////////////////////////////

/*
 * The Engine class is a basic wrapper for initializing and running an ncurses
 * application. The user creates a subclass of the Engine and defines an
 * implementation for the init() and run() methods. The default behavior of
 * the Engine base class takes care of all the low level ncurses stuff like
 * setting up the environment and colors, as well as destroying stdscr when
 * it is deleted.
 */
class Engine {

private:
    // Create basic color pairs using transparent background
    void initializeColorPairs() {
        int backgroundColor = -1; // Transparency
        use_default_colors();

        init_pair(0, COLOR_BLACK, backgroundColor);
        init_pair(1, COLOR_RED, backgroundColor);
        init_pair(2, COLOR_GREEN, backgroundColor);
        init_pair(3, COLOR_YELLOW, backgroundColor);
        init_pair(4, COLOR_BLUE, backgroundColor);
        init_pair(5, COLOR_MAGENTA, backgroundColor);
        init_pair(6, COLOR_CYAN, backgroundColor);
        init_pair(7, COLOR_WHITE, backgroundColor);
    }
    // Set various environment variables to reasonable defaults
    void initializeScreenVariables() {
        initscr();		        // Begin curses mode
        cbreak();		        // Disable line buffering
        keypad(stdscr, TRUE);	// Enable extra keys
        noecho();		        // Disable echoing keys to console
        start_color();		    // Enable color mode
        curs_set(0);		    // Set cursor to be invisible
        timeout(50);		    // Make getch a non-blocking call
    }
    // Initialize curses environment
    void setupCursesEnvironment() {
        initializeScreenVariables();
        initializeColorPairs();
    }
    // Make sure to clean up after ourselves
    void teardownCursesEnvironment() {
        endwin(); // Destroy stdscr
    }

public:
    // Setup curses when the Engine is created
    Engine() {
        setupCursesEnvironment();
    }
    // Teardown curses when the Engine is destroyed
    virtual ~Engine() {
        teardownCursesEnvironment();
    }

    // The user must implement the following two methods in their subclass:
    /*
     * init() is where you should initialize any members of your Engine
     * subclass, and prepare to start running the application.
     */
    virtual void init() = 0;
    /*
     * run() is where you start accepting user input and handling events.
     */
    virtual void run() = 0;

};

/////////////////////////////// DRAWING UTILS ////////////////////////////////

// Drawing functions can take an optional WINDOW *, otherwise use stdscr

// Get attributes by a friendly, human-readable name
int getAttribute(std::string name) {
    auto iter = attributes.find(name);
    // Check if name exists, otherwise return A_NORMAL
    if(iter == attributes.end()) {
        return A_NORMAL;
    } else {
        return iter->second;
    }
}

// Combine an arbitrary amount of attributes into one attribute
// The first parameter tells the function how many attributes to expect
int combineAttributes(int num, ...) {
    va_list argList;
    int attr = A_NORMAL;
    va_start(argList, num);

    for(int i = 0; i < num; i++) {
        attr |= va_arg(argList, int);
    }

    va_end(argList);
    return attr;
}

// Draw character at a given point.
void drawCharAtPoint(char ch, Point p, WINDOW * win = NULL) {
    if(win != NULL) {
        wmove(win, p.y, p.x);
        waddch(win, ch);
    } else {
        move(p.y, p.x);
        addch(ch);
    }
}

// Draw string at a given point
void drawStringAtPoint(std::string text, Point p, WINDOW * win = NULL) {
    if(win != NULL) {
        wmove(win, p.y, p.x);
        waddstr(win, text.c_str());
    } else {
        move(p.y, p.x);
        addstr(text.c_str());
    }
}

// Draw a string centered on a given point
void drawCenteredStringAtPoint(std::string text, Point p, WINDOW * win = NULL) {
    // Compute new point offset by half of string's length
    size_t offset = text.size() / 2;
    Point newPoint(p.x - offset, p.y);
    
    // Delegate to drawStringAtPoint with new point
    drawStringAtPoint(text, newPoint, win);
}

// Set attributes for the given WINDOW (or default to stdscr)
void setAttributes(int attr, WINDOW * win = NULL) {
    if(win != NULL) {
        wattron(win, attr);
    } else {
        attron(attr);
    }
}

// Unset attributes for the given WINDOW (or default to stdscr)
void unsetAttributes(int attr, WINDOW * win = NULL) {
    if(win != NULL) {
        wattroff(win, attr);
    } else {
        attroff(attr);
    }
}

// Draw horizontal line between two points, using ALTCHARSET
void drawHLineBetweenPoints(Point a, Point b, WINDOW * win = NULL) {
    // Only draw line if points are on the same Y level
    if(Point::pointsHaveUnequalY(a, b)) { return; }

    setAttributes(getAttribute("alternate"), win);
    // To avoid issues of order, we check which x is larger
    if(a.x < b.x) {
        for(int i = a.x; i < b.x + 1; i++) {
            drawCharAtPoint(ACS_HLINE, Point(i, a.y), win);
        }
    } else if(a.x > b.x) {
        for(int i = b.x; i < a.x + 1; i++) {
            drawCharAtPoint(ACS_HLINE, Point(i, b.y), win);
        }
    } else {
        // x is equal, so we only draw a single character
        drawCharAtPoint(ACS_HLINE, a, win);
    }
    unsetAttributes(getAttribute("alternate"), win);
}

// Draw vertical line between two points, using ALTCHARSET
void drawVLineBetweenPoints(Point a, Point b, WINDOW * win = NULL) {
    // Only draw line if points are on the same X level
    if(Point::pointsHaveUnequalX(a, b)) { return; }

    setAttributes(getAttribute("alternate"), win);
    // To avoid issues of order, we check which y is larger
    if(a.y < b.y) {
        for(int i = a.y; i < b.y + 1; i++) {
            drawCharAtPoint(ACS_VLINE, Point(a.x, i), win);
        }
    } else if(a.y > b.y) {
        for(int i = b.y; i < a.y + 1; i++) {
            drawCharAtPoint(ACS_VLINE, Point(b.x, i), win);
        }
    } else {
        // y is equal, so we only draw a single character
        drawCharAtPoint(ACS_VLINE, a, win);
    }
    unsetAttributes(getAttribute("alternate"), win);
}

// Draw a default box
void drawBox(Box b, WINDOW * win = NULL) {
    // Draw top and bottom of box
    drawHLineBetweenPoints(b.ul, b.ur, win);
    drawHLineBetweenPoints(b.ll, b.lr, win);

    // Draw left and right of box
    drawVLineBetweenPoints(b.ul, b.ll, win);
    drawVLineBetweenPoints(b.ur, b.lr, win);

    // Draw corners of box
    setAttributes(getAttribute("alternate"), win);
    drawCharAtPoint(ACS_ULCORNER, b.ul, win);
    drawCharAtPoint(ACS_URCORNER, b.ur, win);
    drawCharAtPoint(ACS_LLCORNER, b.ll, win);
    drawCharAtPoint(ACS_LRCORNER, b.lr, win);
    unsetAttributes(getAttribute("alternate"), win);
}

// Fill a box with the given character
void fillBoxWithChar(Box b, char ch, WINDOW * win = NULL) {
    for(int y = b.ul.y; y < b.lr.y + 1; y++) {
        // Draw one line at a time to take advantage of addch()
        if(win != NULL) {
            wmove(win, y, b.ul.x);
        } else {
            move(y, b.ul.x);
        }

        for(int x = b.ul.x; x < b.lr.x + 1; x++) {
            if(win != NULL) {
                waddch(win, ch);
            } else {
                addch(ch);
            }
        }
    }
}

// Clear screen in given box by drawing spaces
// I do this instead of using clear() to avoid latency issues
void clearBox(Box b, WINDOW * win = NULL) {
    fillBoxWithChar(b, ' ', win);
}
