#include "vexes.hpp"

class DemoEngine : public vex::Engine {

private:
    std::vector<vex::Renderable *> renderables;

public:
    ~DemoEngine() {
        for(auto& obj : renderables) {
            delete obj;
        }
    }

    // TODO: Push Renderables into an array, iterated over in the run() method
    void init() override {
        // vex::Text
        vex::Text * init_txt = new vex::Text("Engine initialized successfully!", {0, LINES - 2});
        init_txt->setAttributes(getAttribute("green"));
        renderables.push_back(init_txt);

        int quit_attr = combineAttributes(3, getAttribute("red"),
                                             getAttribute("reverse"),
                                             getAttribute("bold"));
        vex::Text * quit_txt = new vex::Text("Press Q to quit.", {0, LINES - 1});
        quit_txt->setAttributes(quit_attr);
        renderables.push_back(quit_txt);

        vex::Vec2i midpoint = getMidpoint();
        vex::Text * reg_txt = new vex::Text("This string is not centered.", {midpoint.x, midpoint.y - 4});
        renderables.push_back(reg_txt);

        vex::Text * center_txt = new vex::Text("This string is centered!", {midpoint.x, midpoint.y - 2});
        center_txt->setCentered(true);
        renderables.push_back(center_txt);

        // vex::Glyph
        vex::Glyph * mid_glyph = new vex::Glyph('#', midpoint);
        renderables.push_back(mid_glyph);

        // vex::Line
        vex::HLine * h_line = new vex::HLine({3, 3}, {9, 3});
        h_line->setAttributes(getAttribute("yellow"));
        renderables.push_back(h_line);
        vex::Text * h_text = new vex::Text("We have HLines", {11, 3});
        renderables.push_back(h_text);

        vex::VLine * v_line = new vex::VLine({3, 5}, {3, 16});
        v_line->setAttributes(getAttribute("cyan"));
        renderables.push_back(v_line);
        vex::Text * v_text = new vex::Text("And VLines", {5, 6});
        v_text->setVertical(true);
        renderables.push_back(v_text);

        vex::Line * a_line = new vex::Line('*', {9, 8}, {16, 14});
        a_line->setAttributes(getAttribute("magenta"));
        renderables.push_back(a_line);
        vex::Text * a_text = new vex::Text("And Arbitrary Lines!", {14, 10});
        renderables.push_back(a_text);

        // TODO: vex::FillRect
        // TODO: vex::Border
        // TODO: vex::Circle
        // TODO: vex::Ellipse
    }

    // TODO: Turn into input -> update -> render loop
    void run() override {
        int key;
        vex::Text * fps = new vex::Text("", {0, 0});
        renderables.push_back(fps);
        while((key = getch()) != 'q') {
            elapsedTime = clock.getElapsedTime(true);
            fps->setText("FPS: " + std::to_string(1.0 / elapsedTime));

            for(auto& obj : renderables) {
                draw(*obj);
            }
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
