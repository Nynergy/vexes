#pragma once

#include <ncurses.h>
#include <string>
#include <unordered_map>

// CONSTANTS
namespace vex {

    static const std::unordered_map<std::string, int> attributes = {
        {"standout", A_STANDOUT},
        {"underline", A_UNDERLINE},
        {"reverse", A_REVERSE},
        {"blink", A_BLINK},
        {"dim", A_DIM},
        {"bold", A_BOLD},
        {"protected", A_PROTECT},
        {"invisible", A_INVIS},
        {"alternate", A_ALTCHARSET},
        {"extract", A_CHARTEXT},
        {"black", COLOR_PAIR(0)},
        {"red", COLOR_PAIR(1)},
        {"green", COLOR_PAIR(2)},
        {"yellow", COLOR_PAIR(3)},
        {"blue", COLOR_PAIR(4)},
        {"magenta", COLOR_PAIR(5)},
        {"cyan", COLOR_PAIR(6)},
        {"white", COLOR_PAIR(7)}
    };

}

// STRUCTS
namespace vex {

    template <typename T>
    struct Vec2 {
        T x, y;

        Vec2(T x_, T y_): x(x_), y(y_) {}

        bool operator == (const Vec2& other) const {
            return (x == other.x && y == other.y);
        }

        Vec2 operator + (const Vec2& other) const {
            return Vec2<T>(x + other.x, y + other.y);
        }

        Vec2 operator - (const Vec2& other) const {
            return Vec2<T>(x - other.x, y - other.y);
        }
    };

    typedef Vec2<int> Vec2i;

    template <typename T>
    struct Rect {
        T x, y, w, h;

        Rect(T x_, T y_, T w_, T h_): x(x_), y(y_), w(w_), h(h_) {}
        Rect(T x_, T y_, Vec2<T> dim_): x(x_), y(y_), w(dim_.x), h(dim_.y) {}
        Rect(Vec2<T> origin_, T w_, T h_): x(origin_.x), y(origin_.y), w(w_), h(h_) {}
        Rect(Vec2<T> origin_, Vec2<T> dim_): x(origin_.x), y(origin_.y), w(dim_.x), h(dim_.y) {}

        Vec2<T> ul() const {
            return Vec2<T>(x, y);
        }
        Vec2<T> ur() const {
            return Vec2<T>(x + w, y);
        }
        Vec2<T> ll() const {
            return Vec2<T>(x, y + h);
        }
        Vec2<T> lr() const {
            return Vec2<T>(x + w, y + h);
        }

        bool operator == (const Rect& other) const {
            return (x == other.x && y == other.y && w == other.w && h == other.h);
        }
    };

    typedef Rect<int> IntRect;

}

// BASE CLASS DEFINITIONS
namespace vex {

    // DECLARATIONS
    class Renderable;
    class Engine;

    // DEFINITIONS
    class Renderable {

    protected:
        Vec2i pos;
        int attr = A_NORMAL;

    public:
        Renderable(Vec2i pos_);

        virtual void draw(Engine& engine) = 0;
        void setAttributes(int attr_);

    };

    class Engine {

    protected:
        void initializeScreenVariables();
        void initializeColorPairs();
        void setupCursesEnvironment();
        void teardownCursesEnvironment();

    public:
        Engine();
        virtual ~Engine();

        virtual void init() = 0;
        virtual void run() = 0;

        void draw(Renderable& obj);

    public: // UTILITY METHODS
        Vec2i getMidpoint();

    public: // DRAWING METHODS
        int getAttribute(std::string name);
        void setAttributes(int attr, WINDOW * win = NULL);
        void unsetAttributes(int attr, WINDOW * win = NULL);
        int combineAttributes(int num, ...);

        void drawCharAtPoint(char ch, Vec2i p, WINDOW * win = NULL);
        void drawStringAtPoint(std::string text, Vec2i p, WINDOW * win = NULL);
        void drawCenteredStringAtPoint(std::string text, Vec2i p, WINDOW * win = NULL);

        void drawCustomHLineBetweenPoints(char ch, Vec2i a, Vec2i b, WINDOW * win = NULL);
        void drawHLineBetweenPoints(Vec2i a, Vec2i b, WINDOW * win = NULL);
        void drawCustomVLineBetweenPoints(char ch, Vec2i a, Vec2i b, WINDOW * win = NULL);
        void drawVLineBetweenPoints(Vec2i a, Vec2i b, WINDOW * win = NULL);

        void drawCustomBox(IntRect b, char * chars, WINDOW * win = NULL);
        void drawBox(IntRect b, WINDOW * win = NULL);

    };

}

// RENDERABLE SUBCLASSES
namespace vex {

    class Glyph : public Renderable {

    private:
        char glyph;

    public:
        Glyph(char glyph_, Vec2i pos_);

        virtual void draw(Engine& engine) override;

    };

    class Text : public Renderable {

    private:
        std::string text;

    public:
        Text(std::string text_, Vec2i pos_);

        virtual void draw(Engine& engine) override;

    };

}

// IMPLEMENTATIONS
namespace vex {

    // RENDERABLES
    Renderable::Renderable(Vec2i pos_): pos(pos_) {}
    void Renderable::setAttributes(int attr_) {
        attr = attr_;
    }

    Text::Text(std::string text_, Vec2i pos_): Renderable(pos_), text(text_) {}
    void Text::draw(Engine& engine) {
        engine.setAttributes(attr);
        engine.drawStringAtPoint(text, pos);
        engine.unsetAttributes(attr);
    }

    Glyph::Glyph(char glyph_, Vec2i pos_): Renderable(pos_), glyph(glyph_) {}
    void Glyph::draw(Engine& engine) {
        engine.setAttributes(attr);
        engine.drawCharAtPoint(glyph, pos);
        engine.unsetAttributes(attr);
    }

    // ENGINE BASE CLASS
    Engine::Engine() { setupCursesEnvironment(); }
    Engine::~Engine() { teardownCursesEnvironment(); }

    // ENVIRONMENT SETUP AND TEARDOWN
    void Engine::initializeScreenVariables() {
        initscr();		        // Begin curses mode
        cbreak();		        // Disable line buffering
        keypad(stdscr, TRUE);	// Enable extra keys
        noecho();		        // Disable echoing keys to console
        start_color();		    // Enable color mode
        curs_set(0);		    // Set cursor to be invisible
        timeout(50);		    // Make getch a non-blocking call
    }
    void Engine::initializeColorPairs() {
        int backgroundColor = -1; // Transparency
        use_default_colors();

        init_pair(0, COLOR_BLACK,   backgroundColor);
        init_pair(1, COLOR_RED,     backgroundColor);
        init_pair(2, COLOR_GREEN,   backgroundColor);
        init_pair(3, COLOR_YELLOW,  backgroundColor);
        init_pair(4, COLOR_BLUE,    backgroundColor);
        init_pair(5, COLOR_MAGENTA, backgroundColor);
        init_pair(6, COLOR_CYAN,    backgroundColor);
        init_pair(7, COLOR_WHITE,   backgroundColor);
    }
    void Engine::setupCursesEnvironment() {
        initializeScreenVariables();
        initializeColorPairs();
    }
    void Engine::teardownCursesEnvironment() {
        endwin();
    }

    // UTILITY METHODS
    Vec2i Engine::getMidpoint() {
        return {COLS / 2, LINES / 2};
    }

    // DRAWING METHODS
    void Engine::draw(Renderable& obj) {
        obj.draw(*this);
    }
    int Engine::getAttribute(std::string name) {
        auto iter = attributes.find(name);
        // Check if name exists, otherwise return A_NORMAL
        if(iter == attributes.end()) {
            return A_NORMAL;
        } else {
            return iter->second;
        }
    }
    void Engine::setAttributes(int attr, WINDOW * win) {
        if(win != NULL) {
            wattron(win, attr);
        } else {
            attron(attr);
        }
    }
    void Engine::unsetAttributes(int attr, WINDOW * win) {
        if(win != NULL) {
            wattroff(win, attr);
        } else {
            attroff(attr);
        }
    }
    int Engine::combineAttributes(int num, ...) {
        va_list argList;
        int attr = A_NORMAL;
        va_start(argList, num);

        for(int i = 0; i < num; i++) {
            attr |= va_arg(argList, int);
        }

        va_end(argList);
        return attr;
    }
    void Engine::drawCharAtPoint(char ch, Vec2i p, WINDOW * win) {
        if(win != NULL) {
            wmove(win, p.y, p.x);
            waddch(win, ch);
        } else {
            move(p.y, p.x);
            addch(ch);
        }
    }
    void Engine::drawStringAtPoint(std::string text, Vec2i p, WINDOW * win) {
        if(win != NULL) {
            wmove(win, p.y, p.x);
            waddstr(win, text.c_str());
        } else {
            move(p.y, p.x);
            addstr(text.c_str());
        }
    }
    void Engine::drawCenteredStringAtPoint(std::string text, Vec2i p, WINDOW * win) {
        // Compute new point offset by half of string's length
        size_t offset = text.size() / 2;
        Vec2i newPoint(p.x - offset, p.y);
        
        // Delegate to drawStringAtPoint with new point
        drawStringAtPoint(text, newPoint, win);
    }
    void Engine::drawCustomHLineBetweenPoints(char ch, Vec2i a, Vec2i b, WINDOW * win) {
        // Only draw line if points are on the same Y level
        if(a.y != b.y) { return; }

        // To avoid issues of order, we check which x is larger
        if(a.x < b.x) {
            for(int i = a.x; i < b.x + 1; i++) {
                drawCharAtPoint(ch, {i, a.y}, win);
            }
        } else if(a.x > b.x) {
            for(int i = b.x; i < a.x + 1; i++) {
                drawCharAtPoint(ch, {i, b.y}, win);
            }
        } else {
            // x is equal, so we only draw a single character
            drawCharAtPoint(ch, a, win);
        }
    }
    void Engine::drawHLineBetweenPoints(Vec2i a, Vec2i b, WINDOW * win) {
        // Delegate to the custom line with the HLINE character
        setAttributes(getAttribute("alternate"), win);
        drawCustomHLineBetweenPoints(ACS_HLINE, a, b, win);
        unsetAttributes(getAttribute("alternate"), win);
    }
    void Engine::drawCustomVLineBetweenPoints(char ch, Vec2i a, Vec2i b, WINDOW * win) {
        // Only draw line if points are on the same X level
        if(a.x != b.x) { return; }

        // To avoid issues of order, we check which y is larger
        if(a.y < b.y) {
            for(int i = a.y; i < b.y + 1; i++) {
                drawCharAtPoint(ch, {a.x, i}, win);
            }
        } else if(a.y > b.y) {
            for(int i = b.y; i < a.y + 1; i++) {
                drawCharAtPoint(ch, {b.x, i}, win);
            }
        } else {
            // y is equal, so we only draw a single character
            drawCharAtPoint(ch, a, win);
        }
    }
    void Engine::drawVLineBetweenPoints(Vec2i a, Vec2i b, WINDOW * win) {
        // Delegate to the custom line with the VLINE character
        setAttributes(getAttribute("alternate"), win);
        drawCustomVLineBetweenPoints(ACS_VLINE, a, b, win);
        unsetAttributes(getAttribute("alternate"), win);
    }
    void Engine::drawCustomBox(IntRect b, char * chars, WINDOW * win) {
        // Draw top and bottom of box
        drawCustomHLineBetweenPoints(chars[0], b.ul(), b.ur(), win);
        drawCustomHLineBetweenPoints(chars[1], b.ll(), b.lr(), win);

        // Draw left and right of box
        drawCustomVLineBetweenPoints(chars[2], b.ul(), b.ll(), win);
        drawCustomVLineBetweenPoints(chars[3], b.ur(), b.lr(), win);

        // Draw corners of the box
        drawCharAtPoint(chars[4], b.ul(), win);
        drawCharAtPoint(chars[5], b.ur(), win);
        drawCharAtPoint(chars[6], b.ll(), win);
        drawCharAtPoint(chars[7], b.lr(), win);
    }
    void Engine::drawBox(IntRect b, WINDOW * win) {
        // Define alternate characters to use and set attribute on
        char alts[] = {
                        ACS_HLINE, ACS_HLINE, ACS_VLINE, ACS_VLINE,
                        ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER
                      };
        setAttributes(getAttribute("alternate"), win);
        drawCustomBox(b, alts, win);
        unsetAttributes(getAttribute("alternate"), win);
    }

}
