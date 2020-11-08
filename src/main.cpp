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

        // We can draw HLines and VLines between two points
        // NOTE: Drawing a line between two equal points just draws one char
        Point a1(3, 3);
        Point b1(9, 3);
        drawHLineBetweenPoints(a1, b1);
        Point a2(3, 5);
        Point b2(3, 11);
        drawVLineBetweenPoints(a2, b2);
        Point p4(11, 3);
        Point p5(5, 8);
        drawStringAtPoint("We have HLines", p4);
        drawStringAtPoint("And VLines!", p5);

        // We can draw default boxes/borders by defining the corners of a box
        Point ul(10, 10);
        Point ur(20, 10);
        Point ll(10, 20);
        Point lr(20, 20);
        Box b(ul, ur, ll, lr);
        drawBox(b);
        Point p6(15, 14);
        Point p7(15, 15);
        drawCenteredStringAtPoint("Boxes", p6);
        drawCenteredStringAtPoint("too", p7);

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
