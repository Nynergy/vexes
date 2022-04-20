#pragma once

#include <chrono>
#include <cmath>
#include <ncurses.h>
#include <string>
#include <unordered_map>
#include <vector>

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

        Vec2 operator * (const Vec2& other) const {
            return Vec2<T>(x * other.x, y * other.y);
        }
        Vec2 operator / (const Vec2& other) const {
            return Vec2<T>(x / other.x, y / other.y);
        }
    };

    typedef Vec2<int> Vec2i;
    typedef Vec2<float> Vec2f;

    template <typename T>
    struct Rect {
        T x, y, w, h;

        Rect(T x_, T y_, T w_, T h_)
            : x(x_), y(y_), w(w_), h(h_) {}
        Rect(T x_, T y_, Vec2<T> dim_)
            : x(x_), y(y_), w(dim_.x), h(dim_.y) {}
        Rect(Vec2<T> origin_, T w_, T h_)
            : x(origin_.x), y(origin_.y), w(w_), h(h_) {}
        Rect(Vec2<T> origin_, Vec2<T> dim_)
            : x(origin_.x), y(origin_.y), w(dim_.x), h(dim_.y) {}

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

        Vec2<T> origin() const {
            return Vec2<T>(x, y);
        }
        Vec2<T> dim() const {
            return Vec2<T>(w, h);
        }
        Vec2<T> center() const {
            return Vec2<T>(x + w / 2, y + h / 2);
        }

        bool operator == (const Rect& other) const {
            return (x == other.x && y == other.y &&
                    w == other.w && h == other.h);
        }
    };

    typedef Rect<int> IntRect;
    typedef Rect<float> FloatRect;

}

// BASE CLASS DEFINITIONS
namespace vex {

    // DECLARATIONS
    class Renderable;
    class Clock;
    class Engine;

    // DEFINITIONS
    class Renderable {

    protected:
        Vec2i pos;
        int attr = A_NORMAL;

    public:
        Renderable(Vec2i pos_);
        virtual ~Renderable();

        virtual void draw(Engine& engine) = 0;
        void setAttributes(int attr_);

    };

    class Clock {

    private:
        std::chrono::steady_clock stClock;
        std::chrono::time_point<std::chrono::steady_clock> t;

    public:
        Clock();

        double getElapsedTime(bool reset = false);

    };

    class Engine {

    protected:
        Clock clock;
        double elapsedTime = 0.0;

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

        int vecDistance(Vec2i a, Vec2i b);
        Vec2f vecLerp(Vec2i a, Vec2i b, double t);
        float lerp(double a, double b, double t);
        Vec2i vecRound(Vec2f v);

    public: // UTILITY METHODS
        Vec2i getMidpoint();
        std::vector<Vec2i> getPointsOnLine(Vec2i a, Vec2i b);

    public: // DRAWING METHODS
        int getAttribute(std::string name);
        void setAttributes(int attr, WINDOW * win = stdscr);
        void unsetAttributes(int attr, WINDOW * win = stdscr);
        int combineAttributes(int num, ...);

        void drawCharAtPoint(char ch, Vec2i p, WINDOW * win = stdscr);

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
        int half_length;
        bool centered = false;
        bool vertical = false;

    public:
        Text(std::string text_, Vec2i pos_);

        virtual void draw(Engine& engine) override;
        void setText(std::string text_);
        void setCentered(bool centered_);
        void setVertical(bool vertical_);

    };

    class Line : public Renderable {

    protected:
        char glyph;
        Vec2i a, b;
        std::vector<Glyph> line;

        void constructPoints(Engine& engine);

    public:
        Line(char glyph_, Vec2i a_, Vec2i b_);

        virtual void draw(Engine& engine) override;

    };

    class HLine : public Line {

    public:
        HLine(Vec2i a_, Vec2i b_);

        virtual void draw(Engine& engine) override;

    };

    class VLine : public Line {

    public:
        VLine(Vec2i a_, Vec2i b_);

        virtual void draw(Engine& engine) override;

    };

    class CustomQuad : public Renderable {

    private:
        char glyph;
        IntRect dim;

    public:
        CustomQuad(char glyph_, IntRect dim_);

        virtual void draw(Engine& engine) override;
        Vec2i center();

    };

    class Quad : public CustomQuad {

    public:
        Quad(IntRect dim_);

        virtual void draw(Engine& engine) override;

    };

    class CustomBorder : public Renderable {

    private:
        std::vector<char> glyphs;
        IntRect dim;
        std::vector<Line> lines;
        std::vector<Glyph> corners;

    public:
        CustomBorder(std::vector<char> glyphs_, IntRect dim_);

        virtual void draw(Engine& engine) override;
        Vec2i center();

    };

    class Border : public CustomBorder {

    public:
        Border(IntRect dim_);

        virtual void draw(Engine& engine) override;

    };

}

// IMPLEMENTATIONS
namespace vex {

    // RENDERABLES
    Renderable::Renderable(Vec2i pos_): pos(pos_) {}
    Renderable::~Renderable() {}
    void Renderable::setAttributes(int attr_) {
        attr = attr_;
    }

    // TEXT
    Text::Text(std::string text_, Vec2i pos_): Renderable(pos_), text(text_) {
        half_length = text.size() / 2;
    }
    void Text::draw(Engine& engine) {
        engine.setAttributes(attr);

        Vec2i origin = pos;

        if(centered) {
            if(vertical) { origin.y -= half_length; }
            else { origin.x -= half_length; }
        }

        for(char& c : text) {
            engine.drawCharAtPoint(c, origin);
            if(vertical) { origin.y++; } else { origin.x++; }
        }

        engine.unsetAttributes(attr);
    }
    void Text::setText(std::string text_) {
        text = text_;
    }
    void Text::setCentered(bool centered_) {
        centered = centered_;
    }
    void Text::setVertical(bool vertical_) {
        vertical = vertical_;
    }

    // GLYPH
    Glyph::Glyph(char glyph_, Vec2i pos_): Renderable(pos_), glyph(glyph_) {}
    void Glyph::draw(Engine& engine) {
        engine.setAttributes(attr);
        engine.drawCharAtPoint(glyph, pos);
        engine.unsetAttributes(attr);
    }

    // LINES
    Line::Line(char glyph_, Vec2i a_, Vec2i b_)
        : Renderable(a), glyph(glyph_), a(a_), b(b_) {}
    void Line::constructPoints(Engine& engine) {
        std::vector<Vec2i> points = engine.getPointsOnLine(a, b);
        for(auto& p : points) {
            line.push_back(Glyph(glyph, p));
        }
    }
    void Line::draw(Engine& engine) {
        if(line.size() == 0) { constructPoints(engine); }

        engine.setAttributes(attr);
        for(Glyph& g : line) {
            engine.draw(g);
        }
        engine.unsetAttributes(attr);
    }

    HLine::HLine(Vec2i a_, Vec2i b_): Line(ACS_HLINE, a_, b_) {
        // Ensure points have the same Y value
        a.y = b.y;
    }
    void HLine::draw(Engine& engine) {
        engine.setAttributes(engine.getAttribute("alternate"));
        Line::draw(engine);
        engine.unsetAttributes(engine.getAttribute("alternate"));
    }

    VLine::VLine(Vec2i a_, Vec2i b_): Line(ACS_VLINE, a_, b_) {
        // Ensure points have the same X value
        a.x = b.x;
    }
    void VLine::draw(Engine& engine) {
        engine.setAttributes(engine.getAttribute("alternate"));
        Line::draw(engine);
        engine.unsetAttributes(engine.getAttribute("alternate"));
    }

    // QUADS
    CustomQuad::CustomQuad(char glyph_, IntRect dim_)
        : Renderable(dim_.origin()), glyph(glyph_), dim(dim_) {}
    void CustomQuad::draw(Engine& engine) {
        engine.setAttributes(attr);
        for(int x = 0; x < dim.w; x++) {
            for(int y = 0; y < dim.h; y++) {
                engine.drawCharAtPoint(glyph, {x + dim.x, y + dim.y});
            }
        }
        engine.unsetAttributes(attr);
    }
    Vec2i CustomQuad::center() {
        return dim.center();
    }

    Quad::Quad(IntRect dim_): CustomQuad(' ', dim_) {}
    void Quad::draw(Engine& engine) {
        engine.setAttributes(engine.getAttribute("reverse"));
        CustomQuad::draw(engine);
        engine.unsetAttributes(engine.getAttribute("reverse"));
    }

    // BORDERS
    CustomBorder::CustomBorder(std::vector<char> glyphs_, IntRect dim_)
        : Renderable(dim_.origin()), glyphs(glyphs_), dim(dim_) {
        // Top, Bottom, Left, and Right Lines
        lines.push_back(Line(glyphs[0], dim.ul(), dim.ur()));
        lines.push_back(Line(glyphs[1], dim.ll(), dim.lr()));
        lines.push_back(Line(glyphs[2], dim.ul(), dim.ll()));
        lines.push_back(Line(glyphs[3], dim.ur(), dim.lr()));

        // UL, UR, LL, and LR Corners
        corners.push_back(Glyph(glyphs[4], dim.ul()));
        corners.push_back(Glyph(glyphs[5], dim.ur()));
        corners.push_back(Glyph(glyphs[6], dim.ll()));
        corners.push_back(Glyph(glyphs[7], dim.lr()));
    }
    void CustomBorder::draw(Engine& engine) {
        engine.setAttributes(attr);
        for(auto& line : lines) {
            engine.draw(line);
        }
        for(auto& glyph : corners) {
            engine.draw(glyph);
        }
        engine.unsetAttributes(attr);
    }
    Vec2i CustomBorder::center() {
        return dim.center();
    }

    Border::Border(IntRect dim_): CustomBorder({ACS_HLINE, ACS_HLINE,
                                                ACS_VLINE, ACS_VLINE,
                                                ACS_ULCORNER, ACS_URCORNER,
                                                ACS_LLCORNER, ACS_LRCORNER},
                                               dim_) {}
    void Border::draw(Engine& engine) {
        engine.setAttributes(engine.getAttribute("alternate"));
        CustomBorder::draw(engine);
        engine.unsetAttributes(engine.getAttribute("alternate"));
    }

    // CLOCK
    Clock::Clock() {
        t = stClock.now();
    }
    double Clock::getElapsedTime(bool reset) {
        std::chrono::time_point<std::chrono::steady_clock> n = stClock.now();
        std::chrono::duration<double> diff = n - t;

        if(reset) { t = n; }

        return diff.count();
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
        timeout(16);		    // Make getch a non-blocking call
                                // timeout of 16 is ~60 FPS
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
    std::vector<Vec2i> Engine::getPointsOnLine(Vec2i a, Vec2i b) {
        int n = vecDistance(a, b);
        std::vector<Vec2i> points = {};
        double step = 1.0 / std::max(n, 1);

        for(int i = 0; i <= n; i++) {
            points.push_back(vecRound(vecLerp(a, b, step * i)));
        }

        return points;
    }
    int Engine::vecDistance(Vec2i a, Vec2i b) {
        return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
    }
    Vec2f Engine::vecLerp(Vec2i a, Vec2i b, double t) {
        return Vec2f(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
    }
    float Engine::lerp(double a, double b, double t) {
        return a * (1 - t) + b * t;
    }
    Vec2i Engine::vecRound(Vec2f v) {
        return Vec2i(int(std::round(v.x)), int(std::round(v.y)));
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
        wattron(win, attr);
    }
    void Engine::unsetAttributes(int attr, WINDOW * win) {
        wattroff(win, attr);
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
        wmove(win, p.y, p.x);
        waddch(win, ch);
    }

}
