/*
 * In this example, we show off how users can utilize the Form class to make
 * creating fillable fields a breeze.
 */

// At this point, I won't bore you with the usual stuff.
#include "vexes.hpp"

class MyEngine : public Engine {

private:
    // We'll be using a single form where the user can enter a string
    Form * myForm;

public:
    void init() override {
        // Let's draw a box and put our form in there
        Point ul(MIDWIDTH / 2, MIDHEIGHT / 2);
        Point lr(MIDWIDTH + (MIDWIDTH / 2), MIDHEIGHT + (MIDHEIGHT / 2));
        Box borderBox(ul, lr);
        drawBox(borderBox);

        // We'll draw a title while we're at it
        Point titlePoint(MIDWIDTH, MIDHEIGHT / 2);
        setAttributes(getAttribute("red"));
        drawCenteredStringAtPoint("Hit Enter to Fill Form!", titlePoint);
        unsetAttributes(getAttribute("red"));

        // And instructions for exit
        Point exitPoint(MIDWIDTH, MIDHEIGHT + (MIDHEIGHT / 2));
        drawCenteredStringAtPoint("When not typing, hit 'q' to quit.", exitPoint);

        // We can define how long we want the Form to be. By default it's the
        // width of the terminal screen. We also specify where on stdscr the
        // leftmost point of the form will be, and our form's prompt.
        int formLength = MIDWIDTH - 4;
        Point formPoint(ul.x + 2, MIDHEIGHT);
        std::string formPrompt = "Type in me:";
        myForm = new Form(formPoint, formLength, formPrompt);
    }

    void run() override {
        int key;
        while((key = getch()) != 'q') {
            // Here we wait for the user to press ENTER, then edit our Form
            if(key == 10) { // Enter key
                std::string input = myForm->edit();
                std::string output = "You entered: " + input;
                drawStringAtPoint(output, Point(0, 0));
            }
        }
    }

    ~MyEngine() {
        delete myForm;
    }

};

int main() {

    MyEngine * myEngine = new MyEngine();

    myEngine->init();
    myEngine->run();

    delete myEngine;

    return 0;

}
