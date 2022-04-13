#include "vexes.hpp"

class DemoEngine : public vex::Engine {

public:
    void init() override {
        vex::Vec2i center = getMidpoint();
        drawCharAtPoint('#', center);

        vex::Vec2i p1(center.x, center.y - 4);
        vex::Vec2i p2(center.x, center.y - 2);
        drawStringAtPoint("This string is not centered.", p1);
        drawCenteredStringAtPoint("This string is centered!", p2);

        vex::Vec2i a1(3, 3);
        vex::Vec2i b1(9, 3);
        drawHLineBetweenPoints(a1, b1);
        vex::Vec2i a2(3, 5);
        vex::Vec2i b2(3, 11);
        drawVLineBetweenPoints(a2, b2);
        vex::Vec2i p3(11, 3);
        vex::Vec2i p4(5, 8);
        drawStringAtPoint("We have HLines", p3);
        drawStringAtPoint("And VLines!", p4);
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
