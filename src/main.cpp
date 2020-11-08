/*
 * In this example, we show how to use various parts of the vexes library.
 */

// First, be sure to include the header file where appropriate
#include "vexes.hpp"

// Next, define a subclass of the Engine class. This can (and probably should)
// be in a header file, but this is just a simple example.
class MyEngine : public Engine {

public:
    // Here, we override the init() function. This subclass doesn't have any
    // members to initialize, so instead we just draw some text to the screen.
    void init() override {
        // Here, we show off the simple Point and a basic drawing function.
        // Point takes x before y, which is opposite the normal ncurses way.
        // Notice the usage of our macros to find the middle of the screen.
        Point p1(MIDWIDTH, MIDHEIGHT);
        drawCharAtPoint('#', p1);

        // We can do the same with whole strings, even center them on a point!
        Point p2(MIDWIDTH, MIDHEIGHT - 4);
        Point p3(MIDWIDTH, MIDHEIGHT - 2);
        drawStringAtPoint("This string is not centered.", p2);
        drawCenteredStringAtPoint("This string is centered!", p3);

        // We have window-optional wrappers for toggling attributes
        setAttributes(COLOR_PAIR(2));
        Point initP(0, 0);
        drawStringAtPoint("Engine initialized successfully!", initP);
        unsetAttributes(COLOR_PAIR(2));

        setAttributes(COLOR_PAIR(1));
        Point quitP(0, 1);
        drawStringAtPoint("Press Q to quit", quitP);
        unsetAttributes(COLOR_PAIR(1));
    }

    // We override the run() function to start accepting input. In this case,
    // 'q' is the only key that will exit the loop.
    void run() override {
        int key;
        while((key = getch()) != 'q') {
            // Do nothing until user presses 'q'
        }
    }

};

// In our main function, all we do is create an engine, initialize it, and let
// it run. Once it exits the run() function (AKA the user has quit), it is
// deleted, which will automatically destroy stdscr and exit curses mode.
int main() {

    MyEngine * myEngine = new MyEngine();

    myEngine->init();
    myEngine->run();

    delete myEngine;

    return 0;

}
