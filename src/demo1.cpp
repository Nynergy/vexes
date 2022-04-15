#include "vexes.hpp"

class DemoEngine : public vex::Engine {

public:
    void init() override {
        vex::Text init_txt("Engine initialized successfully!", {0, 0});
        init_txt.setAttributes(getAttribute("green"));
        draw(init_txt);

        int quit_attr = combineAttributes(3, getAttribute("red"),
                                             getAttribute("reverse"),
                                             getAttribute("bold"));
        vex::Text quit_txt("Press Q to quit.", {0, 1});
        quit_txt.setAttributes(quit_attr);
        draw(quit_txt);

        vex::Vec2i midpoint = getMidpoint();
        vex::Glyph mid_glyph('#', midpoint);
        draw(mid_glyph);

        vex::Text reg_txt("This string is not centered.", {midpoint.x, midpoint.y - 4});
        vex::Text center_txt("This string is centered!", {midpoint.x, midpoint.y - 2});
        center_txt.setCentered(true);
        draw(reg_txt);
        draw(center_txt);

        vex::HLine h_line({3, 3}, {9, 3});
        h_line.setAttributes(getAttribute("yellow"));
        draw(h_line);
        vex::Text h_text("We have HLines", {11, 3});
        draw(h_text);

        vex::VLine v_line({3, 5}, {3, 11});
        v_line.setAttributes(getAttribute("cyan"));
        draw(v_line);
        vex::Text v_text("And VLines", {5, 6});
        draw(v_text);

        vex::Line a_line('*', {5, 8}, {14, 14});
        a_line.setAttributes(getAttribute("magenta"));
        draw(a_line);
        vex::Text a_text("And Arbitrary Lines!", {12, 10});
        draw(a_text);
    }

    void run() override {
        int key;
        while((key = getch()) != 'q') {
            // Do nothing until user presses 'q'
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
