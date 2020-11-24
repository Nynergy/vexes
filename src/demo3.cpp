/*
 * In this example, we show off how users can create their own Panel subclass
 * in order to utilize a custom drawPanel method. We also show off some custom
 * drawing utilities in the process.
 */

// First, be sure to include the header file
#include "vexes.hpp"

// Next, we want to create our own Panel subclass and override the rendering
// method to something more custom.
class MyPanel : public Panel {

private:
    std::string contents = "This is my panel content!";

    void drawCustomBorder() {
        // First we define the characters of our custom border
        // The characters are always defined in the following order:
        // Top, Bottom, Left, Right, Upper Left, Upper Right, Lower Left, Lower Right
        char border[] = {
                          '%', '%', '|', '|', '#', '#', '#', '#'
                        };
        // Then we pass those characters to the custom box function
        drawCustomBox(localDimensions, border, win);
    }

    void drawCustomTitle() {
        // We want our title to really stick out, so let's add attributes
        Point titlePoint(columns / 2, 0);
        int attr = combineAttributes(3, getAttribute("yellow"),
                                        getAttribute("reverse"),
                                        getAttribute("bold"));
        setAttributes(attr, win);
        drawCenteredStringAtPoint(title, titlePoint, win);
        unsetAttributes(attr, win);
    }

    void drawContents() {
        // Let's draw our content directly in the center
        // Keep in mind that MIDWIDTH and MIDHEIGHT are for stdscr
        // To use our inner window, we have a macro
        // To use it, pass it an x and y, and it will place the centered
        // coordinates into those values for you. Check the source code to see
        // exactly how the macro works!
        int midX, midY;
        CENTER(win, midX, midY);
        Point contentPoint(midX, midY);

        // Let's also make it standout
        int attr = getAttribute("standout");
        setAttributes(attr, win);
        drawCenteredStringAtPoint(contents, contentPoint, win);
        unsetAttributes(attr, win);
    }

public:
    // Our constructor just passes everything along to the Panel constructor
    MyPanel(Box globalDimensionsIn, std::string titleIn = "") :
        Panel(globalDimensionsIn, titleIn) {}

    void drawPanel() override {
        // For our custom panel, let's create a custom border and title
        drawCustomBorder();
        drawCustomTitle();

        // We can also draw our panel contents
        drawContents();

        refreshWindow();
    }

};

// Next we define our Engine subclass, which will create one of our Panels
// and render it to the screen
class MyEngine : public Engine {

private:
    MyPanel * myPanel;

public:
    // Override init to create our panel and store it in the Engine
    void init() override {
        // First we create our dimensions and title, then the Panel
        // Note how we only define the upper left and lower right Points
        // These points are used in the two point constructor for Boxes
        // You don't have to specify all four corners
        Point ul(MIDWIDTH / 2, MIDHEIGHT / 2);
        Point lr(MIDWIDTH + (MIDWIDTH / 2), MIDHEIGHT + (MIDHEIGHT / 2));
        Box panelDimensions(ul, lr);
        std::string panelTitle = "My Custom Panel";
        myPanel = new MyPanel(panelDimensions, panelTitle);
    }

    // Override run to handle rendering and user input
    void run() override {
        int key;
        while((key = getch()) != 'q') {
            // Render our custom Panel
            myPanel->drawPanel();
        }
    }

    // Don't forget to override the destructor so we can safely delete our
    // MyPanel object!
    ~MyEngine() {
        delete myPanel;
    }

};

// Our main will, as always just create the Engine and run it
int main() {

    MyEngine * myEngine = new MyEngine();

    myEngine->init();
    myEngine->run();

    delete myEngine;

    return 0;

}
