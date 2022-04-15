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
