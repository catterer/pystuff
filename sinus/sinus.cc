#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Interval {
    Interval(float left, float right):
        left{left},
        right{right}
    {
        if (left >= right)
            throw std::invalid_argument("Invalid interval");
    }

    const float length() const { return right - left; }

    float left{};
    float right{};
};

struct Axis {
    Axis(Interval interval, int n_pixels):
        interval{interval},
        n_pixels{n_pixels}
    {
        if (n_pixels <= 0)
            throw std::invalid_argument("Invalid number of pixels");

        pixel_size = interval.length() / n_pixels;
    }

    // map float value into integer pixel position
    int approximate_pixel(float v) const {
        const int pixel_offset = trunc((v - interval.left) / pixel_size);
        const float lower_bound = interval.left + pixel_offset * pixel_size;
        const float upper_bound = interval.left + (pixel_offset+1) * pixel_size;
        const float middle = lower_bound + (upper_bound - lower_bound) / 2;

        if (v >= middle)
            return pixel_offset + 1;
        else
            return pixel_offset;
    }

    Interval interval;
    int n_pixels;
    float pixel_size;
};

struct Function {
    std::string name;
    char pixel;
    std::function<std::optional<float>(float)> func;
};

class Screen {
public:
    Screen(Axis x, Axis y):
        x_axis_{x},
        y_axis_{y}
    {
        add_function(Function{{}, '-', Screen::axis_x_func});
        add_reverse_function(Function{{}, '|', Screen::axis_y_func});
    }

    void add_function(Function f) { funcs_.emplace_back(f); }
    void add_reverse_function(Function f) { rfuncs_.emplace_back(f); }

    void render(std::ostream&) const;

private:
    static float axis_x_func(float x) { return 0; }
    static float axis_y_func(float y) { return 0; }

    using PixelRenderer = std::function<void(int x, int y, char c)>;
    void render_functions(const std::vector<Function>&, const Axis& x, const Axis& y, PixelRenderer&&) const;

    void render_legend(std::ostream&) const;

    const Axis x_axis_;
    const Axis y_axis_;
    std::vector<Function> funcs_;     // x -> y
    std::vector<Function> rfuncs_;    // y -> x (needed mostly for Y axis rendering)
};

void Screen::render_legend(std::ostream& out) const {
    auto leg = [&] (const Function& f) {
        if (f.name.empty())
            return;
        out << f.name << ": " << std::string(5, f.pixel) << "\n";
    };

    for (const auto& f: funcs_) leg(f);
    for (const auto& f: rfuncs_) leg(f);
}

void Screen::render(std::ostream& out) const {
    render_legend(out);

    std::vector<std::vector<char>> screen(
            x_axis_.n_pixels,
            std::vector<char>(y_axis_.n_pixels, ' '));

    auto draw_pixel = [&] (int x, int y, char c) {
        if (x >= 0 && x < (int)screen.size())
            if (y >= 0 && y < (int)screen[x].size())
                screen[x][y] = c;
    };

    render_functions(rfuncs_, y_axis_, x_axis_, [&] (int y_idx, int x_idx, char c) { draw_pixel(x_idx, y_idx, c); });
    render_functions( funcs_, x_axis_, y_axis_, [&] (int x_idx, int y_idx, char c) { draw_pixel(x_idx, y_idx, c); });

    for (int y = screen[0].size()-1; y >= 0; --y) {
        for (int x = 0; x < (int)screen.size(); ++x)
            out << screen[x][y];
        out << "\n";
    }
}

void Screen::render_functions(const std::vector<Function>& funcs, const Axis& x_axis, const Axis& y_axis, PixelRenderer&& pixre) const {
    for (float x = x_axis.interval.left; x <= x_axis.interval.right; x += x_axis.pixel_size) {
        for (auto& f: funcs) {
            std::optional<float> y = f.func(x);
            if (!y) // function not defined at this point
                continue;
            pixre(x_axis.approximate_pixel(x), y_axis.approximate_pixel(*y), f.pixel);
        }
    }
}

int main(int argc, char** argv) try {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <screen width, chars> <screen height, chars>" << "\n";
        return 0;
    }
    Axis x_axis{Interval{-10, 10}, std::stoi(argv[1])};
    Axis y_axis{Interval{-3, 3}, std::stoi(argv[2])};
    Screen s{x_axis, y_axis};
    s.add_function(Function{"sin", '.', [] (float x) { return sin(x); }});
    s.add_function(Function{"cos", '\'', [] (float x) { return cos(x); }});
    s.add_function(Function{"tan", ',', [] (float x) { return tan(x); }});
    s.render(std::cout);
    return 0;
} catch (const std::exception& x) {
    std::cout << "Exception: " << x.what() << "\n";
    return 1;
}
