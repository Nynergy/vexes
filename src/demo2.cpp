/*
 * In this example, we show how to use Panels and the automatic layouts
 * provided by vexes.
 */

// First, be sure to include the header file where appropriate
#include "vexes.hpp"

// Next, we define our Engine subclass
class MyEngine : public Engine {

private:
    // Our engine class is going to hold onto three Panels. We can create our
    // own subclass, but I'll just use the default Panel because it has nice
    // built in rendering.
    std::vector<Panel *> panels;

public:
    // Here, we override the init() function.
    void init() override {
        // We want to create three Panels, and we'll use a custom layout.
        // To do this, we can use one of the custom layout functions in vexes.
        // NOTE: It's always good practice to catch InvalidRatioExceptions!
        try {
            // This is how to define a custom layout using a ratio string. You
            // could also use any of the default layouts in the library. Check
            // the source code to see which layouts are available.
            // NOTE: The 'H' in the function name is for "horizontal". There
            // are also vertical counterparts for each layout with 'V'.
            std::vector<Box> dimensions = Layouts::customHLayout("1:1:2");

            // The layout functions will always return us a vector of Boxes. These
            // Boxes tell us where our Panels should go in our desired layout. All
            // we have to do is create our Panels using the appropriate Box.
            for(Box dim : dimensions) {
                Panel * panel = new Panel(dim);
                panels.push_back(panel);
            }

            // You can specify a title at Panel creation, or set one after the
            // fact, like I do here.
            panels[0]->setTitle("Small Panel");
            panels[2]->setTitle("Large Panel");

            // We can also use Panel dimensions to make sub-layouts! I'll turn
            // the middle Panel into a bunch of small Panels. When specifying
            // dimensions in layouts, be sure to pass in a reference to the
            // Box, not a copy of the Box.
            Box panelDim = panels[1]->getGlobalDimensions();
            // VThirds will split the given area into three even thirds vertically.
            std::vector<Box> innerDimensions = Layouts::VThirds(&panelDim);
            
            // Since we aren't using the middle Panel anymore, I remove it.
            delete panels[1];
            panels.erase((panels.begin() + 1));

            // Now we add the inner Panels to our Panel vector.
            for(Box dim : innerDimensions) {
                Panel * panel = new Panel(dim);
                panels.push_back(panel);
            }

            panels[2]->setTitle("Inner Panel 1");
            panels[3]->setTitle("Inner Panel 2");
            panels[4]->setTitle("Inner Panel 3");
        } catch(InvalidRatioException& e) {
            drawStringAtPoint(e.what(), Point(0, 0));
        }
    }

    // We override the run() function to start accepting input. In this case,
    // 'q' is the only key that will exit the loop. When not encountering any
    // input, we want to render our Panels to the screen.
    void run() override {
        int key;
        while((key = getch()) != 'q') {
            // Now we want to render our panels
            for(Panel * p : panels) {
                p->drawPanel();
            }
        }
    }

    // We also want to make sure to override the destructor this time, since
    // we need to delete the Panels we created in the init() function!
    ~MyEngine() {
        for(Panel * p : panels) {
            delete p;
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
