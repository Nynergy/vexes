#pragma once

#include <chrono>
#include <cmath>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// CONSTANTS
namespace vex {

    static const std::unordered_map<std::string, int> attributes = {
        { "standout",   A_STANDOUT    },
        { "underline",  A_UNDERLINE   },
        { "reverse",    A_REVERSE     },
        { "blink",      A_BLINK       },
        { "dim",        A_DIM         },
        { "bold",       A_BOLD        },
        { "protected",  A_PROTECT     },
        { "invisible",  A_INVIS       },
        { "alternate",  A_ALTCHARSET  },
        { "extract",    A_CHARTEXT    },
        { "black",      COLOR_PAIR(0) },
        { "red",        COLOR_PAIR(1) },
        { "green",      COLOR_PAIR(2) },
        { "yellow",     COLOR_PAIR(3) },
        { "blue",       COLOR_PAIR(4) },
        { "magenta",    COLOR_PAIR(5) },
        { "cyan",       COLOR_PAIR(6) },
        { "white",      COLOR_PAIR(7) }
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

        Rect(): x(0), y(0), w(0), h(0) {}
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
    class Glyph;
    class Text;
    class Line;
    class HLine;
    class VLine;
    class CustomQuad;
    class Quad;
    class CustomBorder;
    class Border;

    class Clock;
    class Engine;
    class Panel;

    // DEFINITIONS
    class Renderable {

    protected:
        Vec2i pos;
        int attr = A_NORMAL;

    public:
        Renderable(Vec2i pos_);
        virtual ~Renderable();

        virtual void draw(Engine& engine, WINDOW * win) = 0;
        void setPosition(Vec2i pos_);
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

        void draw(Renderable * obj, WINDOW * win = stdscr);
        void draw(Panel * panel);

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

    class Panel {

    protected:
        WINDOW * win;
        Text * title;
        Border * border;
        IntRect globalDim;  // Dimensions in relation to stdscr
        IntRect localDim;   // Dimensions in relation to internal WINDOW

        void setupWindow();
        void teardownWindow();
        void refreshWindow();
        void replaceWindow();

    public:
        Panel(IntRect globalDim_, std::string title_ = "");
        virtual ~Panel();

        virtual void draw(Engine& engine);
        void resizePanel(IntRect newGlobalDim);

        WINDOW * getWin();

        void setTitle(std::string title_);

    };

}

// RENDERABLE SUBCLASSES
namespace vex {

    class Glyph : public Renderable {

    private:
        char glyph;

    public:
        Glyph(char glyph_, Vec2i pos_);

        virtual void draw(Engine& engine, WINDOW * win = stdscr) override;

    };

    class Text : public Renderable {

    private:
        std::string text;
        int half_length;
        bool centered = false;
        bool vertical = false;

    public:
        Text(std::string text_, Vec2i pos_);

        virtual void draw(Engine& engine, WINDOW * win = stdscr) override;
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

        virtual void draw(Engine& engine, WINDOW * win = stdscr) override;

    };

    class HLine : public Line {

    public:
        HLine(Vec2i a_, Vec2i b_);

        virtual void draw(Engine& engine, WINDOW * win = stdscr) override;

    };

    class VLine : public Line {

    public:
        VLine(Vec2i a_, Vec2i b_);

        virtual void draw(Engine& engine, WINDOW * win = stdscr) override;

    };

    class CustomQuad : public Renderable {

    private:
        char glyph;
        IntRect dim;

    public:
        CustomQuad(char glyph_, IntRect dim_);

        virtual void draw(Engine& engine, WINDOW * win = stdscr) override;
        Vec2i center();

    };

    class Quad : public CustomQuad {

    public:
        Quad(IntRect dim_);

        virtual void draw(Engine& engine, WINDOW * win = stdscr) override;

    };

    class CustomBorder : public Renderable {

    private:
        std::vector<char> glyphs;
        IntRect dim;
        std::vector<Line> lines;
        std::vector<Glyph> corners;

        void constructPieces();

    public:
        CustomBorder(std::vector<char> glyphs_, IntRect dim_);

        virtual void draw(Engine& engine, WINDOW * win = stdscr) override;
        Vec2i center();
        void setDimensions(IntRect dim_);

    };

    class Border : public CustomBorder {

    public:
        Border(IntRect dim_);

        virtual void draw(Engine& engine, WINDOW * win = stdscr) override;

    };

}

// IMPLEMENTATIONS
namespace vex {

    // RENDERABLES
    Renderable::Renderable(Vec2i pos_): pos(pos_) {}
    Renderable::~Renderable() {}
    void Renderable::setPosition(Vec2i pos_) {
        pos = pos_;
    }
    void Renderable::setAttributes(int attr_) {
        attr = attr_;
    }

    // TEXT
    Text::Text(std::string text_, Vec2i pos_): Renderable(pos_), text(text_) {
        half_length = text.size() / 2;
    }
    void Text::draw(Engine& engine, WINDOW * win) {
        engine.setAttributes(attr, win);

        Vec2i origin = pos;

        if(centered) {
            if(vertical) { origin.y -= half_length; }
            else { origin.x -= half_length; }
        }

        for(char& c : text) {
            engine.drawCharAtPoint(c, origin, win);
            if(vertical) { origin.y++; } else { origin.x++; }
        }

        engine.unsetAttributes(attr, win);
    }
    void Text::setText(std::string text_) {
        text = text_;
        half_length = text.size() / 2;
    }
    void Text::setCentered(bool centered_) {
        centered = centered_;
    }
    void Text::setVertical(bool vertical_) {
        vertical = vertical_;
    }

    // GLYPH
    Glyph::Glyph(char glyph_, Vec2i pos_): Renderable(pos_), glyph(glyph_) {}
    void Glyph::draw(Engine& engine, WINDOW * win) {
        engine.setAttributes(attr, win);
        engine.drawCharAtPoint(glyph, pos, win);
        engine.unsetAttributes(attr, win);
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
    void Line::draw(Engine& engine, WINDOW * win) {
        if(line.size() == 0) { constructPoints(engine); }

        engine.setAttributes(attr, win);
        for(Glyph& g : line) {
            engine.draw(&g, win);
        }
        engine.unsetAttributes(attr, win);
    }

    HLine::HLine(Vec2i a_, Vec2i b_): Line(ACS_HLINE, a_, b_) {
        // Ensure points have the same Y value
        a.y = b.y;
    }
    void HLine::draw(Engine& engine, WINDOW * win) {
        engine.setAttributes(engine.getAttribute("alternate"), win);
        Line::draw(engine, win);
        engine.unsetAttributes(engine.getAttribute("alternate"), win);
    }

    VLine::VLine(Vec2i a_, Vec2i b_): Line(ACS_VLINE, a_, b_) {
        // Ensure points have the same X value
        a.x = b.x;
    }
    void VLine::draw(Engine& engine, WINDOW * win) {
        engine.setAttributes(engine.getAttribute("alternate"), win);
        Line::draw(engine, win);
        engine.unsetAttributes(engine.getAttribute("alternate"), win);
    }

    // QUADS
    CustomQuad::CustomQuad(char glyph_, IntRect dim_)
        : Renderable(dim_.origin()), glyph(glyph_), dim(dim_) {}
    void CustomQuad::draw(Engine& engine, WINDOW * win) {
        engine.setAttributes(attr, win);
        for(int x = 0; x < dim.w; x++) {
            for(int y = 0; y < dim.h; y++) {
                engine.drawCharAtPoint(glyph, {x + dim.x, y + dim.y}, win);
            }
        }
        engine.unsetAttributes(attr, win);
    }
    Vec2i CustomQuad::center() {
        return dim.center();
    }

    Quad::Quad(IntRect dim_): CustomQuad(' ', dim_) {}
    void Quad::draw(Engine& engine, WINDOW * win) {
        engine.setAttributes(engine.getAttribute("reverse"), win);
        CustomQuad::draw(engine, win);
        engine.unsetAttributes(engine.getAttribute("reverse"), win);
    }

    // BORDERS
    CustomBorder::CustomBorder(std::vector<char> glyphs_, IntRect dim_)
        : Renderable(dim_.origin()), glyphs(glyphs_), dim(dim_) {
        constructPieces();
    }
    void CustomBorder::constructPieces() {
        // Top, Bottom, Left, and Right Lines
        if(lines.size()) { lines.clear(); }
        lines.push_back(Line(glyphs[0], dim.ul(), dim.ur()));
        lines.push_back(Line(glyphs[1], dim.ll(), dim.lr()));
        lines.push_back(Line(glyphs[2], dim.ul(), dim.ll()));
        lines.push_back(Line(glyphs[3], dim.ur(), dim.lr()));

        // UL, UR, LL, and LR Corners
        if(corners.size()) { corners.clear(); }
        corners.push_back(Glyph(glyphs[4], dim.ul()));
        corners.push_back(Glyph(glyphs[5], dim.ur()));
        corners.push_back(Glyph(glyphs[6], dim.ll()));
        corners.push_back(Glyph(glyphs[7], dim.lr()));
    }
    void CustomBorder::draw(Engine& engine, WINDOW * win) {
        engine.setAttributes(attr, win);
        for(auto& line : lines) {
            engine.draw(&line, win);
        }
        for(auto& glyph : corners) {
            engine.draw(&glyph, win);
        }
        engine.unsetAttributes(attr, win);
    }
    Vec2i CustomBorder::center() {
        return dim.center();
    }
    void CustomBorder::setDimensions(IntRect dim_) {
        dim = dim_;
        constructPieces();
    }

    Border::Border(IntRect dim_): CustomBorder({ACS_HLINE, ACS_HLINE,
                                                ACS_VLINE, ACS_VLINE,
                                                ACS_ULCORNER, ACS_URCORNER,
                                                ACS_LLCORNER, ACS_LRCORNER},
                                               dim_) {}
    void Border::draw(Engine& engine, WINDOW * win) {
        engine.setAttributes(engine.getAttribute("alternate"), win);
        CustomBorder::draw(engine, win);
        engine.unsetAttributes(engine.getAttribute("alternate"), win);
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
    void Engine::draw(Renderable * obj, WINDOW * win) {
        obj->draw(*this, win);
    }
    void Engine::draw(Panel * panel) {
        panel->draw(*this);
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

    // PANEL BASE CLASS
    Panel::Panel(IntRect globalDim_, std::string title_): globalDim(globalDim_) {
        localDim = IntRect({0, 0}, globalDim.dim());

        title = new Text(title_, {localDim.w / 2, 0});
        title->setCentered(true);
        border = new Border(localDim);

        setupWindow();
    }
    Panel::~Panel() {
        delete title;
        delete border;
        teardownWindow();
    }

    void Panel::draw(Engine& engine) {
        engine.draw(border, win);
        engine.draw(title, win);
        refreshWindow();
    }
    void Panel::resizePanel(IntRect newGlobalDim) {
        globalDim = newGlobalDim;
        localDim = IntRect({0, 0}, globalDim.dim());

        title->setPosition({localDim.w / 2, 0});
        border->setDimensions(localDim);

        replaceWindow();
    }

    WINDOW * Panel::getWin() {
        return win;
    }

    void Panel::setTitle(std::string title_) {
        title->setText(title_);
    }

    void Panel::setupWindow() {
        win = newwin(globalDim.h + 1, globalDim.w + 1, globalDim.y, globalDim.x);
    }
    void Panel::teardownWindow() {
        delwin(win);
    }
    void Panel::refreshWindow() {
        wrefresh(win);
    }
    void Panel::replaceWindow() {
        teardownWindow();
        setupWindow();
    }

}

// DYNAMIC LAYOUTS
namespace vex {
    /*
     * Custom exception for denoting invalid ratio strings
     */
    struct InvalidRatioException : public std::exception {

    private:
        const char * message;

    public:
        InvalidRatioException(const char * messageIn) : message(messageIn) {}
        const char * what() throw () {
            return message;
        }

    };

    /*
     * The Layouts class is a utility class designed so that users can either ask
     * for a custom layout by passing it a valid ratio string (e.g., "1:1:2"), or
     * they can use one of the default basic layouts. All the class does is return
     * a vector of Boxes, so the user can use them when making their Panels.
     */
    class Layouts {

    private:
        // This function is used to see if the numbers in ratio strings are ints
        static bool isInteger(const std::string & s) {
           if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) {
               return false;
           }

           char * p;
           strtol(s.c_str(), &p, 10);

           return (*p == 0);
        }

        /*
         * A ratio string is valid if and only if it contains an arbitrary series
         * of integer values delimited by a colon (:) character, e.g. "1:1:2".
         * Colons cannot appear more than once in a row (so "1::1:2" is not valid)
         * and cannot appear on either end of the string (":1:1:2" isn't valid).
         */
        static void validateRatio(std::string ratio) {
            // Check for at least one colon
            if(ratio.find(':') == std::string::npos) {
                const char * message = "Ratios must contain at least one colon.";
                throw InvalidRatioException(message);
            }

            // Check for colons on either end
            char first = *(ratio.begin());
            char last = *(--(ratio.end()));
            if(first == ':' || last == ':') {
                const char * message = "Ratios cannot begin or end with a colon.";
                throw InvalidRatioException(message);
            }

            // Check for double colons
            int length = ratio.length();
            for(int i = 1; i < length; i++) {
                if(ratio[i] == ':' && ratio[i - 1] == ':') {
                    const char * message = "Ratios can only be delimited by single colons.";
                    throw InvalidRatioException(message);
                }
            }

            // Split string on colons and check for integers
            std::stringstream ss(ratio);
            std::string token;
            while(std::getline(ss, token, ':')) {
                // Check if token is integer
                if(!isInteger(token)) {
                    const char * message = "Ratios can only contain valid integers.";
                    throw InvalidRatioException(message);
                }
                // Integer cannot be 0
                int num = std::stoi(token);
                if(num == 0) {
                    const char * message = "Ratios cannot contain 0 as an integer.";
                    throw InvalidRatioException(message);
                }
            }

            // If all the above checks out, ratio is valid
            return;
        }

        // Split the ratio string on colons and cast numbers to ints.
        static std::vector<int> extractNumsFromString(std::string ratio) {
            std::vector<int> nums;
            std::stringstream ss(ratio);
            std::string token;
            while(std::getline(ss, token, ':')) {
                int num = std::stoi(token);
                nums.push_back(num);
            }

            return nums;
        }

        /*
         * Create Box dimensions and track the position on the screen so far,
         * resulting in a set of distinct Boxes that take up the proper space
         * and positions desired.
         */
        static std::vector<IntRect> calculateHBoxes(std::vector<int> nums, IntRect * dim) {
            // Get an overall ratio base
            int base = 0;
            for(int num : nums) {
                base += num;
            }

            std::vector<IntRect> boxes;

            // Initialize dimensions based on dimensions (stdscr by default)
            int fullWidth, fullHeight;
            int startX, startY;
            if(dim != NULL) {
                // Base width and height on passed window
                fullWidth = dim->w;
                fullHeight = dim->h;
                startX = dim->x;
                startY = dim->y;
            } else {
                // Base width and height on stdscr
                fullWidth = COLS - 1;
                fullHeight = LINES - 1;
                startX = 0;
                startY = 0;
            }

            int lastX = startX - 1;
            for(int num : nums) {
                // Calculate how many rows and columns this box takes
                int rows = fullHeight;
                float frac = (float) num / base;
                int columns = (int) (frac * fullWidth);

                // Truncate columns if they would go offscreen
                if(lastX + 1 + columns >= fullWidth) {
                    columns = fullWidth - (lastX + 1);
                }

                // Turn those rows and columns into dimensions
                IntRect box_dim({lastX + 1, startY}, {columns, rows});
                boxes.push_back(box_dim);

                // Update lastX to move new dimensions over by old width
                lastX += columns + 1;
            }

            return boxes;
        }

        // Similar to calculateHBoxes, but for vertical layouts.
        static std::vector<IntRect> calculateVBoxes(std::vector<int> nums, IntRect * dim) {
            // Get an overall ratio base
            int base = 0;
            for(int num : nums) {
                base = base + num;
            }

            std::vector<IntRect> boxes;

            // Initialize dimensions based on dimensions (stdscr by default)
            int fullWidth, fullHeight;
            int startX, startY;
            if(dim != NULL) {
                // Base width and height on passed window
                fullWidth = (dim->ur().x - dim->ul().x);
                fullHeight = (dim->lr().y - dim->ur().y);
                startX = dim->ul().x;
                startY = dim->ul().y;
            } else {
                // Base width and height on stdscr
                fullWidth = COLS - 1;
                fullHeight = LINES - 1;
                startX = 0;
                startY = 0;
            }

            int lastY = startY - 1;
            for(int num : nums) {
                // Calculate how many rows and columns this box takes
                float frac = (float) num / base;
                int rows = (int) (frac * fullHeight);
                int columns = fullWidth;

                // Truncate rows if they would go offscreen
                if(lastY + 1 + rows >= fullHeight) {
                    rows = fullHeight - (lastY + 1);
                }

                // Turn those rows and columns into dimensions
                IntRect box_dim({startX, lastY + 1}, {columns, rows});
                boxes.push_back(box_dim);

                // Update lastY to move new dimensions over by old height
                lastY += rows + 1;
            }

            return boxes;
        }

    public:
        /*
         * The user must pass a proper ratio string, and it will return a vector
         * of Boxes, each one representing the position of a Panel in the given
         * layout. An optional Box can be passed, which can be used to make
         * "sub-layouts" within a user-defined area. A good use case for this is
         * to make layouts within a Panel's globalDimensions.
         *
         * NOTE: The user is responsible for catching any InvalidRatioExceptions
         * that are thrown, otherwise the Engine class' automatic curses cleanup
         * will not run, and the terminal may become stuck in curses mode upon
         * a runtime error.
         */
        static std::vector<IntRect> customHLayout(std::string ratio, IntRect * dim = NULL) {
            std::vector<IntRect> boxes;
            // Check for proper ratio string
            try {
                validateRatio(ratio);
            } catch(InvalidRatioException& e) {
                throw InvalidRatioException(e.what());
            }

            // Extract numbers from ratio
            std::vector<int> nums = extractNumsFromString(ratio);

            // Populate vector with boxes based on ratio nums
            boxes = calculateHBoxes(nums, dim);

            return boxes;
        }

        /* 
         * This is similar to customHLayout(), but for vertical layouts. The user
         * is still responsible for catching InvalidRatioExceptions.
         */
        static std::vector<IntRect> customVLayout(std::string ratio, IntRect * dim = NULL) {
            std::vector<IntRect> boxes;
            // Check for proper ratio string
            try {
                validateRatio(ratio);
            } catch(InvalidRatioException& e) {
                throw InvalidRatioException(e.what());
            }

            // Extract numbers from ratio
            std::vector<int> nums = extractNumsFromString(ratio);

            // Populate vector with boxes based on ratio nums
            boxes = calculateVBoxes(nums, dim);

            return boxes;
        }

        /*
         * What follows are a bunch of default layouts in both orientations. Since
         * these functions will use valid ratio strings, there is no need to catch
         * any InvalidRatioExceptions.
         */
        static std::vector<IntRect> HSplit(IntRect * dim = NULL) {
            return customHLayout("1:1", dim);
        }
        static std::vector<IntRect> HTwoThirdsLeft(IntRect * dim = NULL) {
            return customHLayout("2:1", dim);
        }
        static std::vector<IntRect> HTwoThirdsRight(IntRect * dim = NULL) {
            return customHLayout("1:2", dim);
        }
        static std::vector<IntRect> HThirds(IntRect * dim = NULL) {
            return customHLayout("1:1:1", dim);
        }
        static std::vector<IntRect> VSplit(IntRect * dim = NULL) {
            return customVLayout("1:1", dim);
        }
        static std::vector<IntRect> VTwoThirdsAbove(IntRect * dim = NULL) {
            return customVLayout("2:1", dim);
        }
        static std::vector<IntRect> VTwoThirdsBelow(IntRect * dim = NULL) {
            return customVLayout("1:2", dim);
        }
        static std::vector<IntRect> VThirds(IntRect * dim = NULL) {
            return customVLayout("1:1:1", dim);
        }

    };

}
