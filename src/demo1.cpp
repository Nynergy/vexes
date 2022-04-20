/*
 * This demo shows off a basic subclass and main loop structure of a vexes
 * application, as well as some fundamental Renderable classes provided by the
 * header file.
 */

// First, we include the vexes header file. Very important step!
#include "vexes.hpp"

// Our application is built out of the vex::Engine base class, so we need to
// subclass it in order to define the app's behavior and internal data.
class DemoEngine : public vex::Engine {

private:
    // Store the current user input in an integer variable.
    int key;
    // Track if the user has quit the application.
    bool userQuit = false;
    // Keep a pointer to a Text object in order to update it later on.
    vex::Text * fps;
    // Collect all the Renderables together to be drawn all at once.
    std::vector<vex::Renderable *> renderables;

public:
    // Since we'll be allocating memory for all our Renderable objects, we
    // deallocate that memory here when the Engine is destroyed.
    ~DemoEngine() {
        for(auto& obj : renderables) {
            delete obj;
        }
    }

    // The init() method is one of two necessary overrides we must make. This
    // is where we initialize all the data our application will be using. In
    // the case of this demo, it's where we create and collect all our
    // Renderable objects.
    void init() override {
        // vex::Glyph
        // The Glyph represents a single character, and shows off the basics of
        // the Renderable subclasses. A Glyph takes a single character to draw,
        // and a Vec2i position (effectively XY coordinates) of where to draw
        // that character.
        vex::Vec2i midpoint = getMidpoint();
        vex::Glyph * mid_glyph = new vex::Glyph('#', midpoint);
        renderables.push_back(mid_glyph);

        // vex::Text
        // The Text object is the vexes representation of a string. These are
        // created much the same way as Glyphs, but take a full string instead
        // of a single character. As you'll see later in this demo, they also
        // have the ability to be centered, and drawn either horizontally or
        // vertically.
        vex::Text * reg_txt = new vex::Text("This string is not centered.", {midpoint.x, midpoint.y - 4});
        renderables.push_back(reg_txt);

        vex::Text * center_txt = new vex::Text("This string is centered!", {midpoint.x, midpoint.y - 2});
        // We center a Text object with the setCentered() method.
        center_txt->setCentered(true);
        renderables.push_back(center_txt);

        // vex::Line
        // The Line is just that: a line of a single character from point A to
        // point B. We have three line types: Horizontal, Vertical, and
        // Arbitrary. Horizontal and Vertical are just special versions of the
        // Arbitrary line, where the points are forced to have matching X/Y
        // values where appropriate, and they default to using the built-in
        // ncurses alternate line characters.
        vex::HLine * h_line = new vex::HLine({3, 3}, {9, 3});
        // We can set any Renderable's attributes with setAttributes().
        h_line->setAttributes(getAttribute("yellow"));
        renderables.push_back(h_line);
        vex::Text * h_text = new vex::Text("We have HLines", {11, 3});
        renderables.push_back(h_text);

        vex::VLine * v_line = new vex::VLine({3, 5}, {3, 16});
        v_line->setAttributes(getAttribute("cyan"));
        renderables.push_back(v_line);
        vex::Text * v_text = new vex::Text("And VLines", {5, 6});
        // We tell Text objects to draw vertically using setVertical().
        v_text->setVertical(true);
        renderables.push_back(v_text);

        // Arbitrary lines can be made of any one character, specified at
        // creation, as shown below. They use linear interpolation to get all
        // the terminal cells between the two points, allowing you to draw a
        // line at any angle, not just vertically or horizontally.
        vex::Line * a_line = new vex::Line('*', {9, 8}, {16, 14});
        a_line->setAttributes(getAttribute("magenta"));
        renderables.push_back(a_line);
        vex::Text * a_text = new vex::Text("And Arbitrary Lines!", {14, 10});
        renderables.push_back(a_text);

        // vex::Quad
        // The Quad is how we represent a filled rectangle. The default Quad
        // class takes an IntRect at creation, which describes the XY position
        // of its upper left corner and the width/height of its dimensions. It
        // draws a rectangle of reversed spaces in the area defined by that
        // IntRect, effectively filling the space with a psuedo-block character
        // in place of the traditional ncurses ACS_BLOCK character.
        vex::Quad * rect = new vex::Quad({{COLS - 32, 1}, {30, 9}});
        rect->setAttributes(getAttribute("blue"));
        renderables.push_back(rect);
        // Notice the call to rect->center() here. You can get a Vec2i of the
        // center of the Quad with that method. The same method exists for any
        // IntRect you create, as well (Quads just return the center() of their
        // own internal IntRect anyway).
        vex::Text * rect_text = new vex::Text("We have Quadrilaterals!", rect->center());
        rect_text->setCentered(true);
        // You can combine attributes like normal bit flags.
        rect_text->setAttributes(getAttribute("blue") | getAttribute("reverse") | getAttribute("bold"));
        renderables.push_back(rect_text);

        // CustomQuad is a customizable version of the Quad, which allows you
        // to specify what character the rectangle is filled with, rather than
        // defaulting to a filled block character.
        vex::CustomQuad * custom_rect = new vex::CustomQuad('%', {{COLS - 32, 11}, {30, 9}});
        custom_rect->setAttributes(getAttribute("red") | getAttribute("bold"));
        renderables.push_back(custom_rect);
        vex::Text * custom_rect_text = new vex::Text("And Custom Quads, too!", custom_rect->center());
        custom_rect_text->setCentered(true);
        custom_rect_text->setAttributes(getAttribute("red") | getAttribute("bold"));
        renderables.push_back(custom_rect_text);

        // vex::Border
        // The Border is effectively a box around the outer perimeter of an
        // IntRect. Like a Quad, it takes IntRect dimensions, but it does not
        // render anything inside the outer edge of the rectangle. This default
        // version draws a box using the ncurses alternate characters.
        vex::IntRect border_dim({COLS - 36, LINES - 27}, {30, 10});
        vex::Border * border = new vex::Border(border_dim);
        renderables.push_back(border);
        // Like Quads, Borders can tell you their center point with center().
        vex::Text * border_text = new vex::Text("Use Default Borders...", border->center());
        border_text->setCentered(true);
        renderables.push_back(border_text);

        // CustomBorder is, like CustomQuad, a way for you to specify what
        // characters it is made of instead of the ncurses defaults. You pass
        // it a std::vector of characters, in the following order:
        // 0: Top Line          1: Bottom Line
        // 2: Left Line         3: Right Line
        // 4: Upper Left Corner 5: Upper Right Corner
        // 6: Lower Left Corner 7: Lower Right Corner
        std::vector<char> border_glyphs = {'=', '=', '|', '|', '+', '+', '+', '+'};
        vex::IntRect custom_border_dim({COLS - 36, LINES - 16}, {30, 10});
        vex::CustomBorder * custom_border = new vex::CustomBorder(border_glyphs, custom_border_dim);
        custom_border->setAttributes(getAttribute("green"));
        renderables.push_back(custom_border);
        vex::Text * custom_border_text = new vex::Text("Or Create Custom Borders!", custom_border->center());
        custom_border_text->setCentered(true);
        custom_border_text->setAttributes(getAttribute("green"));
        renderables.push_back(custom_border_text);

        vex::Text * init_txt = new vex::Text("Engine initialized successfully!", {0, LINES - 2});
        init_txt->setAttributes(getAttribute("green"));
        renderables.push_back(init_txt);

        // You can also combine an arbitrary number of attributes using the
        // Engine method combineAttributes(), which takes a number of
        // attributes to combine, and then those attributes.
        int quit_attr = combineAttributes(3, getAttribute("red"),
                                             getAttribute("reverse"),
                                             getAttribute("bold"));
        vex::Text * quit_txt = new vex::Text("Press Q to quit.", {0, LINES - 1});
        quit_txt->setAttributes(quit_attr);
        renderables.push_back(quit_txt);

        // Here we finally initialize our fps Text object. The string here is
        // not important, since we will be updating it later on.
        fps = new vex::Text("", {0, 0});
        renderables.push_back(fps);
    }

    // The run() method is our other important override. Here, we define our
    // main application loop. I've chosen to break it out into three more
    // sub-functions, which are responsible for different things.
    void run() override {
        // We want to keep looping until the user quits.
        while(!userQuit) {
            handleInput();
            update();
            render();
        }
    }

    // Here, we call getch() to get user input, and include any logic involving
    // specific user input. For now, just set userQuit to true if the user hits
    // the 'q' key.
    void handleInput() {
        key = getch();

        if(key == 'q') {
            userQuit = true;
        }
    }

    // Here, we update any parts of our state that do not necessarily depend on
    // user input, or should be updated every loop.
    void update() {
        // The vexes library includes a Clock class, which is included in the
        // Engine by default. We can use it to get the elapsed time since the
        // last call to the clock. Using this value, we can calculate the FPS,
        // and interpolate animations to adjust for different FPS values on
        // different machines.
        elapsedTime = clock.getElapsedTime(true);
        // For now, let's just display the current FPS by setting it to the
        // text value of our fps Renderable object.
        fps->setText("FPS: " + std::to_string(1.0 / elapsedTime));
    }

    // Here, we simply draw all of our Renderable objects. This application is
    // simple enough that it does not require us to clear the screen between
    // renders, but other applications would clear the screen here before
    // drawing the objects.
    void render() {
        for(auto& obj : renderables) {
            draw(*obj);
        }
    }

};

// Our main function is incredibly simple, since all the logic is handled by
// our Engine subclass. We just create our Engine, initialize it, and tell it
// to start running.
int main() {

    DemoEngine * engine = new DemoEngine();

    engine->init();
    engine->run();

    delete engine;

    return 0;

}
