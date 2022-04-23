/*
 * In this example, we show how to use Panels and the automatic layouts
 * provided by vexes.
 */

#include "vexes.hpp"

class DemoEngine : public vex::Engine {

private:
    std::vector<vex::Panel *> panels;

    void resizePanels() {
        std::vector<vex::IntRect> outer = vex::Layouts::customHLayout("1:1:2");
        std::vector<vex::IntRect> inner = vex::Layouts::VThirds(&(outer[1]));

        std::vector<std::pair<vex::IntRect, vex::Panel *>> new_dims = {
            {outer[0], panels[0]},
            {outer[2], panels[1]},
            {inner[0], panels[2]},
            {inner[1], panels[3]},
            {inner[2], panels[4]}
        };

        for(auto& p : new_dims) {
            vex::IntRect dim = p.first;
            vex::Panel * panel = p.second;
            panel->resizePanel(dim);
        }
    }

public:
    ~DemoEngine() {
        for(auto& p : panels) {
            delete p;
        }
    }

    void init() override {
        std::vector<vex::IntRect> dims = vex::Layouts::customHLayout("1:1:2");

        panels.push_back(new vex::Panel(dims[0]));
        panels.push_back(new vex::Panel(dims[2]));

        panels[0]->setTitle("Small Panel");
        panels[1]->setTitle("Large Panel");

        std::vector<vex::IntRect> inner_dims = vex::Layouts::VThirds(&(dims[1]));

        for(auto& dim : inner_dims) {
            vex::Panel * panel = new vex::Panel(dim);
            panels.push_back(panel);
        }

        panels[2]->setTitle("Inner Panel 1");
        panels[3]->setTitle("Inner Panel 2");
        panels[4]->setTitle("Inner Panel 3");
    }

    void run() override {
        int key;
        while((key = getch()) != 'q') {
            switch(key) {
                case KEY_RESIZE:
                    resizePanels();
                    break;
                default:
                    break;
            }

            for(auto& p : panels) {
                draw(p);
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
