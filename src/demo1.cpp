#include "vexes.hpp"

class DemoEngine : public vex::Engine {

public:
    void init() override {
        vex::Vec2i center = getMidpoint();
        drawCharAtPoint('#', center);

        // TODO: Mimic the old demo1 file
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
