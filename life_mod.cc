#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

struct Cell {
    int age; // -1 means "not a microb"
    friend std::ostream& operator<<(std::ostream& out, const Cell& cell) {
        out << (cell.age <= 0 ? ' ' : '*');
        return out;
    }
    void kill() { if (age > 0) age = 0; }
    void older() { if (age >= 0) age = (age + 1) % 12; }
    bool alive() const { return age > 0; }
};

class Game {
public:
    Game(int x, int y):
        field_(x, std::vector<Cell>(y, Cell{-1}))
    {}

    void set(int x, int y) {
        field_[x % field_.size()][y % field_[0].size()] = Cell{1};
    }

    void render(std::ostream& out) const {
        for (const auto& r: field_) {
            for (const auto c: r)
                out << c;
            out << "\n";
        }
    }

    void next() {
        auto future = field_;
        for (auto& r: future)
            for (auto& c: r)
                c = Cell{-1};

        for (int x = 0; x < field_.size(); ++x)
            for (int y = 0; y < field_[x].size(); ++y) {
                future[x][y] = field_[x][y];
                future[x][y].older();
                switch (count_neighb(x, y)) {
                case 2:
                case 3:
                    break;
                default:
                    future[x][y].kill();
                }
            }

        std::swap(field_, future);
    }

private:
    int count_neighb(int x, int y) const {
        int n_nb = 0;
        for (auto mod: {
            std::make_pair(0, 1),
            std::make_pair(1, 0),
            std::make_pair(0, -1),
            std::make_pair(-1, 0),
            std::make_pair(1, 1),
            std::make_pair(-1, -1),
            std::make_pair(1, -1),
            std::make_pair(-1, 1)
        }) {
            int x_n = (x + mod.first + field_.size()) % field_.size();
            int y_n = (y + mod.second + field_[0].size()) % field_[0].size();
            if (field_[x_n][y_n].alive())
                ++n_nb;
        }
        return n_nb;
    }

    std::vector<std::vector<Cell>> field_;
};

int main(int argc, char** argv) try {
    Game g{std::stoi(argv[1]), std::stoi(argv[2])};
    for (int i = 0; i < std::stoi(argv[3]); ++i)
        g.set(rand(), rand());

    for (;; std::this_thread::sleep_for(std::chrono::milliseconds(100))) {
        g.next();
        g.render(std::cout);
        std::cout << "=============\n";
    }
    return 0;
} catch (const std::exception& x) {
    std::cerr << x.what() << "\n";
    return 0;
}
