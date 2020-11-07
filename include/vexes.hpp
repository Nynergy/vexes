#pragma once

#include <ncurses.h>

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
