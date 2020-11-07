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
        attron(COLOR_PAIR(2));
        mvprintw(0, 0, "Engine initialized successfully!");
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(1));
        mvprintw(1, 0, "Press Q to quit");
        attron(COLOR_PAIR(1));
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
