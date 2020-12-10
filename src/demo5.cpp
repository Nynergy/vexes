/*
 * In this example, we show off how users can create and use custom Form
 * subclasses, as well as how user's can pre-fill Forms by injecting arbitrary
 * strings into the field buffer.
 */

// As with the last demo, I'll skip the boring stuff
#include "vexes.hpp"

// Our custom Form will act like the regular form, but be rendered differently
class MyForm : public Form {

private:
    // Our custom drawing functions will colorize our form a bit
    void drawCustomPrompt() {
        int attr = combineAttributes(2, getAttribute("bold"),
                                        getAttribute("magenta"));
        setAttributes(attr, win);

        // Then we delegate to the regular prompt rendering method
        drawPrompt();

        unsetAttributes(attr, win);
    }

    void drawCustomBuffer() {
        int attr = combineAttributes(3, getAttribute("bold"),
                                        getAttribute("reverse"),
                                        getAttribute("magenta"));
        setAttributes(attr, win);

        // Let's make the input field more visible
        Point a(promptLength + 1, 0); Point b(promptLength + columns, 0);
        drawCustomHLineBetweenPoints(' ', a, b, win);

        // For the rest, we let the base class draw it with our custom attributes
        drawBuffer();

        unsetAttributes(attr, win);
    }

public:
    // Constructor just passes everything to the base class
    MyForm(Point origin, int length, std::string prompt) : Form(origin, length, prompt) {}

    void drawForm() override {
        clearForm();
        drawCustomPrompt();
        drawCustomBuffer();
    }

};

class MyEngine : public Engine {

private:
    // We'll be using our own Form subclass for this demo
    MyForm * myForm;

public:
    void init() override {
        // Like last time, we'll draw a box and put our Form inside it
        Point ul(MIDWIDTH / 2, MIDHEIGHT / 2);
        Point lr(MIDWIDTH + (MIDWIDTH / 2), MIDHEIGHT + (MIDHEIGHT / 2));
        Box borderBox(ul, lr);
        drawBox(borderBox);

        // Draw our title
        Point titlePoint(MIDWIDTH, MIDHEIGHT / 2);
        setAttributes(getAttribute("yellow"));
        drawCenteredStringAtPoint("Hit Enter to Fill Form!", titlePoint);
        unsetAttributes(getAttribute("yellow"));

        // And instructions for exit
        Point exitPoint(MIDWIDTH, MIDHEIGHT + (MIDHEIGHT / 2));
        drawCenteredStringAtPoint("When not typing, hit 'q' to quit.", exitPoint);

        // And finally create our custom form and place it on the screen
        int formLength = MIDWIDTH - 4;
        Point formPoint(ul.x + 2, MIDHEIGHT);
        std::string formPrompt = "Type in me:";
        myForm = new MyForm(formPoint, formLength, formPrompt);

        // To demonstrate pre-filling forms, let's inject a string into it
        myForm->injectString("Pre-filled text");
    }

    void run() override {
        int key;
        while((key = getch()) != 'q') {
            // Wait for the user to press ENTER, then edit our Form
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
