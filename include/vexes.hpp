#pragma once

#include <ncurses.h>
#include <string>
#include <sstream>
#include <map>
#include <vector>

////////////////////////////////// MACROS ////////////////////////////////////

// These macros just define a simple way of finding the middle of the screen
#define MIDHEIGHT           (LINES/2)
#define MIDWIDTH            (COLS/2)

// This macro is for finding the center of a user-created WINDOW
#define CENTER(w, x, y)     {                          \
                              getmaxyx((w), (y), (x)); \
                              x = ((x) / 2) - 1;       \
                              y = ((y) / 2) - 1;       \
                            }

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

    Point() : x(0), y(0) {}
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

    Box() : ul(Point(0, 0)), ur(Point(COLS - 1, 0)),
            ll(Point(0, LINES - 1)), lr(Point(COLS - 1, LINES - 1)) {}
    Box(Point ulIn, Point urIn, Point llIn, Point lrIn) :
        ul(ulIn), ur(urIn), ll(llIn), lr(lrIn) {}

    // This function is a handy way to make a Box with only two points
    static Box create(Point upperLeft, Point lowerRight) {
        Point upperRight(lowerRight.x, upperLeft.y);
        Point lowerLeft(upperLeft.x, lowerRight.y);
        Box newBox(upperLeft, upperRight, lowerLeft, lowerRight);

        return newBox;
    }

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

// Draw horizontal line between two points, using given character
void drawCustomHLineBetweenPoints(char ch, Point a, Point b, WINDOW * win = NULL) {
    // Only draw line if points are on the same Y level
    if(Point::pointsHaveUnequalY(a, b)) { return; }

    // To avoid issues of order, we check which x is larger
    if(a.x < b.x) {
        for(int i = a.x; i < b.x + 1; i++) {
            drawCharAtPoint(ch, Point(i, a.y), win);
        }
    } else if(a.x > b.x) {
        for(int i = b.x; i < a.x + 1; i++) {
            drawCharAtPoint(ch, Point(i, b.y), win);
        }
    } else {
        // x is equal, so we only draw a single character
        drawCharAtPoint(ch, a, win);
    }
}

// Draw vertical line between two points, using given character
void drawCustomVLineBetweenPoints(char ch, Point a, Point b, WINDOW * win = NULL) {
    // Only draw line if points are on the same X level
    if(Point::pointsHaveUnequalX(a, b)) { return; }

    // To avoid issues of order, we check which y is larger
    if(a.y < b.y) {
        for(int i = a.y; i < b.y + 1; i++) {
            drawCharAtPoint(ch, Point(a.x, i), win);
        }
    } else if(a.y > b.y) {
        for(int i = b.y; i < a.y + 1; i++) {
            drawCharAtPoint(ch, Point(b.x, i), win);
        }
    } else {
        // y is equal, so we only draw a single character
        drawCharAtPoint(ch, a, win);
    }
}

// Draw horizontal line between two points, using ALTCHARSET
void drawHLineBetweenPoints(Point a, Point b, WINDOW * win = NULL) {
    // Delegate to the custom line with the HLINE character
    setAttributes(getAttribute("alternate"), win);
    drawCustomHLineBetweenPoints(ACS_HLINE, a, b, win);
    unsetAttributes(getAttribute("alternate"), win);
}

// Draw vertical line between two points, using ALTCHARSET
void drawVLineBetweenPoints(Point a, Point b, WINDOW * win = NULL) {
    // Delegate to the custom line with the VLINE character
    setAttributes(getAttribute("alternate"), win);
    drawCustomVLineBetweenPoints(ACS_VLINE, a, b, win);
    unsetAttributes(getAttribute("alternate"), win);
}

// Draw a box with user-defined characters
// Characters should always be in the following order:
// Top, Bottom, Left, Right, Upper Left, Upper Right, Lower Left, Lower Right
void drawCustomBox(Box b, char * chars, WINDOW * win = NULL) {
    // Draw top and bottom of box
    drawCustomHLineBetweenPoints(chars[0], b.ul, b.ur, win);
    drawCustomHLineBetweenPoints(chars[1], b.ll, b.lr, win);

    // Draw left and right of box
    drawCustomVLineBetweenPoints(chars[2], b.ul, b.ll, win);
    drawCustomVLineBetweenPoints(chars[3], b.ur, b.lr, win);

    // Draw corners of the box
    drawCharAtPoint(chars[4], b.ul, win);
    drawCharAtPoint(chars[5], b.ur, win);
    drawCharAtPoint(chars[6], b.ll, win);
    drawCharAtPoint(chars[7], b.lr, win);
}

// Draw a default box
void drawBox(Box b, WINDOW * win = NULL) {
    // Define alternate characters to use and set attribute on
    char alts[] = {
                    ACS_HLINE, ACS_HLINE, ACS_VLINE, ACS_VLINE,
                    ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER
                  };
    setAttributes(getAttribute("alternate"), win);
    drawCustomBox(b, alts, win);
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

protected:
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

/*
 * The Panel class is a handy abstraction for making panels in your app that
 * use separate screens. Supply it with a Box for its corners in relation to
 * the terminal screen as a whole and it will handle sizing and resizing for
 * you! When users subclass this base class, they have the option of making
 * their own drawPanel() function, but the default is provided out of the box.
 */
class Panel {

protected:
    WINDOW * win;
    std::string title;
    Box globalDimensions;   // globalDimensions is in relation to stdscr
    Box localDimensions;    // localDimensions is in relation to win
    int lines, columns;

    // Create new internal window based on global position
    void setupWindow() {
        win = newwin(lines + 1, columns + 1, globalDimensions.ul.y, globalDimensions.ul.x);
    }
    // Safely destroy internal window
    void teardownWindow() {
        delwin(win);
    }
    // Default draws a box around the window
    void drawBorder() {
        drawBox(localDimensions, win);
    }
    // Default finds middle of top line, draws title string
    void drawTitle() {
        Point titlePoint(columns / 2, 0);
        drawCenteredStringAtPoint(title, titlePoint, win);
    }
    // Refresh internal window
    void refreshWindow() {
        wrefresh(win);
    }
    // Destroy old window, make a new one
    void replaceWindow() {
        teardownWindow();
        setupWindow();
    }
    // Clear the space of the internal window
    void clearScreen() {
        clearBox(localDimensions, win);
    }

public:
    // Title string is optional
    Panel(Box globalDimensionsIn, std::string titleIn = "") {
        title = titleIn;
        globalDimensions = globalDimensionsIn;

        // Calculate sizes based on global dimensions
        lines = globalDimensions.ll.y - globalDimensions.ul.y;
        columns = globalDimensions.ur.x - globalDimensions.ul.x;

        // Use sizes in creating local dimensions
        Point ul(0, 0); Point ur(columns, 0);
        Point ll(0, lines); Point lr(columns, lines);
        localDimensions = Box(ul, ur, ll, lr);

        // Finally, create the internal window
        setupWindow();
    }
    // Clean up after ourselves
    virtual ~Panel() {
        teardownWindow();
    }

    // The user has the choice of leaving this, or overriding it. The default
    // just draws the border and title, then refreshes.
    virtual void drawPanel() {
        drawBorder();
        drawTitle();
        refreshWindow();
    }
    // Given a new Box of dimensions, reset the internal sizes and window
    void resizePanel(Box newGlobalDimensions) {
        globalDimensions = newGlobalDimensions;
        lines = newGlobalDimensions.ll.y - newGlobalDimensions.ul.y;
        columns = newGlobalDimensions.ur.x - newGlobalDimensions.ul.x;

        Point ul(0, 0); Point ur(columns, 0);
        Point ll(0, lines); Point lr(columns, lines);
        localDimensions = Box(ul, ur, ll, lr);

        replaceWindow();
    }

    WINDOW * getWin() { return win; }
    Box getGlobalDimensions() { return globalDimensions; }

    void setTitle(std::string newTitle) { title = newTitle; }

};

///////////////////////////// LAYOUT UTILITIES ///////////////////////////////

/*
 * Custom exception for denoting invalid ratio strings
 */
struct InvalidRatioException : public std::exception {

private:
    const char * message;

public:
    InvalidRatioException(const char * messageIn) : message(messageIn) {}
    const char * what() throw () {
        return message;
    }

};

/*
 * The Layouts class is a utility class designed so that users can either ask
 * for a custom layout by passing it a valid ratio string (e.g., "1:1:2"), or
 * they can use one of the default basic layouts. All the class does is return
 * a vector of Boxes, so the user can use them when making their Panels.
 */
class Layouts {

private:
    // This function is used to see if the numbers in ratio strings are ints
    static bool isInteger(const std::string & s) {
       if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) {
           return false;
       }

       char * p;
       strtol(s.c_str(), &p, 10);

       return (*p == 0);
    }

    /*
     * A ratio string is valid if and only if it contains an arbitrary series
     * of integer values delimited by a colon (:) character, e.g. "1:1:2".
     * Colons cannot appear more than once in a row (so "1::1:2" is not valid)
     * and cannot appear on either end of the string (":1:1:2" isn't valid).
     */
    static void validateRatio(std::string ratio) {
        // Check for at least one colon
        if(ratio.find(':') == std::string::npos) {
            const char * message = "Ratios must contain at least one colon.";
            throw InvalidRatioException(message);
        }

        // Check for colons on either end
        char first = *(ratio.begin());
        char last = *(--(ratio.end()));
        if(first == ':' || last == ':') {
            const char * message = "Ratios cannot begin or end with a colon.";
            throw InvalidRatioException(message);
        }

        // Check for double colons
        int length = ratio.length();
        for(int i = 1; i < length; i++) {
            if(ratio[i] == ':' && ratio[i - 1] == ':') {
                const char * message = "Ratios can only be delimited by single colons.";
                throw InvalidRatioException(message);
            }
        }

        // Split string on colons and check for integers
        std::stringstream ss(ratio);
        std::string token;
        while(std::getline(ss, token, ':')) {
            // Check if token is integer
            if(!isInteger(token)) {
                const char * message = "Ratio can only contain valid integers.";
                throw InvalidRatioException(message);
            }
            // Integer cannot be 0
            int num = std::stoi(token);
            if(num == 0) {
                const char * message = "Ratio cannot contain 0 as an integer.";
                throw InvalidRatioException(message);
            }
        }

        // If all the above checks out, ratio is valid
        return;
    }

    // Split the ratio string on colons and cast numbers to ints.
    static std::vector<int> extractNumsFromString(std::string ratio) {
        std::vector<int> nums;
        std::stringstream ss(ratio);
        std::string token;
        while(std::getline(ss, token, ':')) {
            int num = std::stoi(token);
            nums.push_back(num);
        }

        return nums;
    }

    /*
     * Create Box dimensions and track the position on the screen so far,
     * resulting in a set of distinct Boxes that take up the proper space
     * and positions desired.
     */
    static std::vector<Box> calculateHBoxes(std::vector<int> nums, Box * dimensions) {
        // Get an overall ratio base
        int base = 0;
        for(int num : nums) {
            base = base + num;
        }

        std::vector<Box> boxes;

        // Initialize dimensions based on dimensions (stdscr by default)
        int fullWidth, fullHeight;
        int startX, startY;
        if(dimensions != NULL) {
            // Base width and height on passed window
            fullWidth = (dimensions->ur.x - dimensions->ul.x);
            fullHeight = (dimensions->lr.y - dimensions->ur.y);
            startX = dimensions->ul.x;
            startY = dimensions->ul.y;
        } else {
            // Base width and height on stdscr
            fullWidth = COLS - 1;
            fullHeight = LINES - 1;
            startX = 0;
            startY = 0;
        }

        int lastX = startX - 1;
        for(int num : nums) {
            // Calculate how many rows and columns this box takes
            int rows = fullHeight;
            float frac = (float) num / base;
            int columns = (int) (frac * fullWidth);

            // Truncate columns if they would go offscreen
            if(lastX + 1 + columns >= fullWidth) {
                columns = fullWidth - (lastX + 1);
            }

            // Turn those rows and columns into Box dimensions
            Point ul(lastX + 1, startY);
            Point ur(lastX + 1 + columns, startY);
            Point ll(lastX + 1, startY + rows);
            Point lr(lastX + 1 + columns, startY + rows);
            Box corners(ul, ur, ll, lr);

            boxes.push_back(corners);

            // Update lastX to move new Box over by old Box width
            lastX = ur.x;
        }

        return boxes;
    }

    // Similar to calculateHBoxes, but for vertical layouts.
    static std::vector<Box> calculateVBoxes(std::vector<int> nums, Box * dimensions) {
        // Get an overall ratio base
        int base = 0;
        for(int num : nums) {
            base = base + num;
        }

        std::vector<Box> boxes;

        // Initialize dimensions based on dimensions (stdscr by default)
        int fullWidth, fullHeight;
        int startX, startY;
        if(dimensions != NULL) {
            // Base width and height on passed window
            fullWidth = (dimensions->ur.x - dimensions->ul.x);
            fullHeight = (dimensions->lr.y - dimensions->ur.y);
            startX = dimensions->ul.x;
            startY = dimensions->ul.y;
        } else {
            // Base width and height on stdscr
            fullWidth = COLS - 1;
            fullHeight = LINES - 1;
            startX = 0;
            startY = 0;
        }

        int lastY = startY - 1;
        for(int num : nums) {
            // Calculate how many rows and columns this box takes
            float frac = (float) num / base;
            int rows = (int) (frac * fullHeight);
            int columns = fullWidth;

            // Truncate rows if they would go offscreen
            if(lastY + 1 + rows >= fullHeight) {
                rows = fullHeight - (lastY + 1);
            }

            // Turn those rows and columns into Box dimensions
            Point ul(startX, lastY + 1);
            Point ur(startX + columns, lastY + 1);
            Point ll(startX, lastY + 1 + rows);
            Point lr(startX + columns, lastY + 1 + rows);
            Box corners(ul, ur, ll, lr);

            boxes.push_back(corners);

            // Update lastX to move new Box over by old Box width
            lastY = lr.y;
        }

        return boxes;
    }

public:
    /*
     * The user must pass a proper ratio string, and it will return a vector
     * of Boxes, each one representing the position of a Panel in the given
     * layout. An optional Box can be passed, which can be used to make
     * "sub-layouts" within a user-defined area. A good use case for this is
     * to make layouts within a Panel's globalDimensions.
     *
     * NOTE: The user is responsible for catching any InvalidRatioExceptions
     * that are thrown, otherwise the Engine class' automatic curses cleanup
     * will not run, and the terminal may become stuck in curses mode upon
     * a runtime error.
     */
    static std::vector<Box> customHLayout(std::string ratio, Box * dimensions = NULL) {
        std::vector<Box> boxes;
        // Check for proper ratio string
        try {
            validateRatio(ratio);
        } catch(InvalidRatioException& e) {
            throw InvalidRatioException(e.what());
        }

        // Extract numbers from ratio
        std::vector<int> nums = extractNumsFromString(ratio);

        // Populate vector with boxes based on ratio nums
        boxes = calculateHBoxes(nums, dimensions);

        return boxes;
    }

    /* 
     * This is similar to customHLayout(), but for vertical layouts. The user
     * is still responsible for catching InvalidRatioExceptions.
     */
    static std::vector<Box> customVLayout(std::string ratio, Box * dimensions = NULL) {
        std::vector<Box> boxes;
        // Check for proper ratio string
        try {
            validateRatio(ratio);
        } catch(InvalidRatioException& e) {
            throw InvalidRatioException(e.what());
        }

        // Extract numbers from ratio
        std::vector<int> nums = extractNumsFromString(ratio);

        // Populate vector with boxes based on ratio nums
        boxes = calculateVBoxes(nums, dimensions);

        return boxes;
    }

    /*
     * What follows are a bunch of default layouts in both orientations. Since
     * these functions will use valid ratio strings, there is no need to catch
     * any InvalidRatioExceptions.
     */
    static std::vector<Box> HSplit(Box * dimensions = NULL) {
        return customHLayout("1:1", dimensions);
    }
    static std::vector<Box> HTwoThirdsLeft(Box * dimensions = NULL) {
        return customHLayout("2:1", dimensions);
    }
    static std::vector<Box> HTwoThirdsRight(Box * dimensions = NULL) {
        return customHLayout("1:2", dimensions);
    }
    static std::vector<Box> HThirds(Box * dimensions = NULL) {
        return customHLayout("1:1:1", dimensions);
    }
    static std::vector<Box> VSplit(Box * dimensions = NULL) {
        return customVLayout("1:1", dimensions);
    }
    static std::vector<Box> VTwoThirdsAbove(Box * dimensions = NULL) {
        return customVLayout("2:1", dimensions);
    }
    static std::vector<Box> VTwoThirdsBelow(Box * dimensions = NULL) {
        return customVLayout("1:2", dimensions);
    }
    static std::vector<Box> VThirds(Box * dimensions = NULL) {
        return customVLayout("1:1:1", dimensions);
    }

};
