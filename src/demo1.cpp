#include "vexes.hpp"

class DemoEngine : public vex::Engine {

public:
    // TODO: Push Renderables into an array, iterated over in the run() method
    void init() override {
        // vex::Text
        vex::Text init_txt("Engine initialized successfully!", {0, LINES - 2});
        init_txt.setAttributes(getAttribute("green"));
        draw(init_txt);

        int quit_attr = combineAttributes(3, getAttribute("red"),
                                             getAttribute("reverse"),
                                             getAttribute("bold"));
        vex::Text quit_txt("Press Q to quit.", {0, LINES - 1});
        quit_txt.setAttributes(quit_attr);
        draw(quit_txt);

        vex::Vec2i midpoint = getMidpoint();
        vex::Text reg_txt("This string is not centered.", {midpoint.x, midpoint.y - 4});
        vex::Text center_txt("This string is centered!", {midpoint.x, midpoint.y - 2});
        center_txt.setCentered(true);
        draw(reg_txt);
        draw(center_txt);

        // vex::Glyph
        vex::Glyph mid_glyph('#', midpoint);
        draw(mid_glyph);

        // vex::Line
        vex::HLine h_line({3, 3}, {9, 3});
        h_line.setAttributes(getAttribute("yellow"));
        draw(h_line);
        vex::Text h_text("We have HLines", {11, 3});
        draw(h_text);

        vex::VLine v_line({3, 5}, {3, 16});
        v_line.setAttributes(getAttribute("cyan"));
        draw(v_line);
        vex::Text v_text("And VLines", {5, 6});
        v_text.setVertical(true);
        draw(v_text);

        vex::Line a_line('*', {9, 8}, {16, 14});
        a_line.setAttributes(getAttribute("magenta"));
        draw(a_line);
        vex::Text a_text("And Arbitrary Lines!", {14, 10});
        draw(a_text);

        // TODO: vex::FillRect
        // TODO: vex::Border
        // TODO: vex::Circle
        // TODO: vex::Ellipse
    }

    // TODO: Turn into input -> update -> render loop
    void run() override {
        int key;
        vex::Text fps("", {0, 0});
        while((key = getch()) != 'q') {
            elapsedTime = clock.getElapsedTime(true);
            fps.setText("FPS: " + std::to_string(1.0 / elapsedTime));
            draw(fps);
        }
    }

};

int main() {

    DemoEngine * engine = new DemoEngine();

    engine->init();
    engine->run();

    delete engine;

    return 0;

}
